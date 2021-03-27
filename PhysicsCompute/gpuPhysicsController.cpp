#include "gpuPhysicsController.h"
#include "opengl/ssbo.h"
#include "gpuPolygon.h"
#include <chrono>
#include <iostream>

namespace pc::gpu {
	typedef unsigned int uint;

	enum BufferBindBase {
		Buffer_SSBO,
		Buffer_Vertices,
		Buffer_Indices,
		Buffer_VerticesOffsets,
		Buffer_TransformedVertices,
		Buffer_Matrices,
		Buffer_AABBs,
		Buffer_PossibleCollisions,
		Buffer_Variables,
		Buffer_Normals,
		Buffer_TranformedNormals,
		Buffer_Collisions,
		Buffer_Rigidbodies
	};

	struct Collision {
		uint a;
		uint b;
		uint v_a;
		uint v_b;
		float dist;
		float align;
		ce::vec2f normal;
	};

	struct GPURigidbody {
		float e;
		float rotation;
		float inv_mass;
		float inv_moment_of_inertia;

		ce::vec2f position;

		float angular_velocity;
		float torque;

		ce::vec2f velocity;
		ce::vec2f force;
	};


	ce::Program sqr_shader;
	ce::Program line_shader;
	ce::Program calculate_aabbs_shader;
	ce::Program transformed_line_shader;
	ce::Program resolve_collisions_shader;
	ce::Program update_rigidbodies_shader;
	ce::Program transform_vertices_shader;
	ce::Program possible_collisions_shader;
	ce::Program separated_axis_check_shader;

	ce::gl::SSBO ssbo;
	ce::gl::SSBO ssbo_vertices;
	ce::gl::SSBO ssbo_indices;
	ce::gl::SSBO ssbo_vertices_offsets;
	ce::gl::SSBO ssbo_transformed_vertices;
	ce::gl::SSBO ssbo_matrices;
	ce::gl::SSBO ssbo_aabbs;
	ce::gl::SSBO ssbo_possible_collisions;
	ce::gl::SSBO ssbo_variables;
	ce::gl::SSBO ssbo_normals;
	ce::gl::SSBO ssbo_transformed_normals;
	ce::gl::SSBO ssbo_collisions;
	ce::gl::SSBO ssbo_rigidbodies;

	std::vector<uint> indices;
	std::vector<ce::vec2f> vertices;
	std::vector<ce::vec2f> normals;
	std::vector<uint> vertices_offsets;
	std::vector<pc::GPUPolygon> gpu_polygons;

	uint polygons_count;
	size_t possible_collisions_max_count;

	uint getPairsCount(uint objects_count) {
		return objects_count * (objects_count - 1) / 2;
	}

	void startup() {
		transform_vertices_shader.attachShaderFromFile("transform_vertices.comp", ce::ShaderType::Compute);
		transform_vertices_shader.link();

		calculate_aabbs_shader.attachShaderFromFile("calculate_aabbs.comp", ce::ShaderType::Compute);
		calculate_aabbs_shader.link();

		possible_collisions_shader.attachShaderFromFile("possible_collisions.comp", ce::ShaderType::Compute);
		possible_collisions_shader.link();

		separated_axis_check_shader.attachShaderFromFile("separated_axis_check.comp", ce::ShaderType::Compute);
		separated_axis_check_shader.link();

		resolve_collisions_shader.attachShaderFromFile("resolve_collisions.comp", ce::ShaderType::Compute);
		resolve_collisions_shader.link();

		update_rigidbodies_shader.attachShaderFromFile("update_rigidbodies.comp", ce::ShaderType::Compute);
		update_rigidbodies_shader.link();

		transformed_line_shader.attachShaderFromFile("transformed_line.vert", ce::ShaderType::Vertex);
		transformed_line_shader.attachShaderFromFile("line.frag", ce::ShaderType::Fragment);
		transformed_line_shader.link();


		ssbo.create<ce::vec2f>(1000)
			.bindBase(Buffer_SSBO);
	}

	void init(const std::vector<Polygon>& polygons) {
		polygons_count = polygons.size();
		std::vector<GPURigidbody> rigidbodies;

		vertices.clear();
		indices.clear();
		normals.clear();
		vertices_offsets.clear();

		unsigned int polygons_count = polygons.size();

		possible_collisions_max_count = getPairsCount(polygons_count);

		uint i = 0;
		for (const pc::Polygon& polygon : polygons) {
			vertices_offsets.push_back(vertices.size());

			const auto& p_vertices = polygon.getVertices();

			for (int v = 0; v < p_vertices.size(); ++v) {
				ce::vec2f normal = -(p_vertices[(v + 1) % p_vertices.size()] - p_vertices[v]).perpendicular();
				normals.push_back(normal.normalized());

				vertices.push_back(p_vertices[v] - polygon.transform.getOrigin());
			}

			const pc::Rigidbody* rigidbody = polygon.getRigidbody();
			rigidbodies.push_back(GPURigidbody{
				rigidbody->e,
				polygon.transform.getRotation(),
				rigidbody->inv_mass,
				rigidbody->inv_moment_of_inertia,

				polygon.transform.getPosition(),

				rigidbody->angular_velocity,
				rigidbody->torque,

				rigidbody->velocity,
				rigidbody->force
				});
			indices.insert(indices.end(), p_vertices.size(), i++);
		}
		vertices_offsets.push_back(vertices.size());

		ssbo_vertices
			.create<ce::vec2f>(vertices.size())
			.bindBase(Buffer_Vertices)
			.setData<ce::vec2f>(vertices.data(), vertices.size());

		ssbo_vertices_offsets
			.create<uint>(vertices_offsets.size())
			.bindBase(Buffer_VerticesOffsets)
			.setData<uint>(vertices_offsets.data(), vertices_offsets.size());

		ssbo_indices
			.create<uint>(indices.size())
			.bindBase(Buffer_Indices)
			.setData<uint>(indices.data(), indices.size());

		ssbo_transformed_vertices
			.create<ce::vec2f>(vertices.size())
			.bindBase(Buffer_TransformedVertices);

		ssbo_matrices
			.create<float>(4 * 3 * polygons.size())
			.bindBase(Buffer_Matrices);

		ssbo_aabbs
			.create<pc::AABB>(polygons.size())
			.bindBase(Buffer_AABBs);

		ssbo_possible_collisions
			.create<uint>((possible_collisions_max_count + 1) * 2)
			.bindBase(Buffer_PossibleCollisions);

		ssbo_variables
			.create<uint>(6)
			.bindBase(Buffer_Variables);

		ssbo_normals
			.create<ce::vec2f>(normals.size())
			.bindBase(Buffer_Normals)
			.setData<ce::vec2f>(normals.data(), normals.size());

		ssbo_transformed_normals
			.create<ce::vec2f>(normals.size())
			.bindBase(Buffer_TranformedNormals);

		ssbo_collisions
			.create<Collision>(possible_collisions_max_count)
			.bindBase(Buffer_Collisions);

		ssbo_rigidbodies
			.create<GPURigidbody>(polygons.size())
			.bindBase(Buffer_Rigidbodies)
			.setData<GPURigidbody>(rigidbodies.data(), rigidbodies.size());

		for (uint i = 0; i < polygons.size(); ++i) {
			pc::GPUPolygon p;
			p.load(ssbo_transformed_vertices.getBufferId(), vertices_offsets[i], polygons[i].getVertices().size());
			p.setProgram(&transformed_line_shader);

			gpu_polygons.push_back(p);
		}

		update_rigidbodies_shader.bind();
		update_rigidbodies_shader.setFloat("deltatime", 0);
		glDispatchCompute(polygons.size(), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		transform_vertices_shader.bind();
		glDispatchCompute(vertices.size(), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void gpu::draw(ce::RenderTarget* renderTarget) {
		for (const auto& polygon : gpu_polygons) {
			renderTarget->draw(&polygon);
		}
	}

	void updatePhysics(float deltatime) {
		// transform vertices
		transform_vertices_shader.bind();
		glDispatchCompute(vertices.size(), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		//auto start = std::chrono::high_resolution_clock::now();

		// calculate aabb for each polygon
		calculate_aabbs_shader.bind();
		calculate_aabbs_shader.setUint("points_count", vertices.size());
		calculate_aabbs_shader.setUint("polygons_count", polygons_count);
		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		//double time_elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count();


		// find possible collisions
		possible_collisions_shader.bind();
		possible_collisions_shader.setUint("possible_collisions_count", possible_collisions_max_count);
		glDispatchCompute(polygons_count, getPairsCount(polygons_count + 1) / polygons_count, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, ssbo_variables.getBufferId());
			separated_axis_check_shader.bind();
			glDispatchComputeIndirect(0);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			resolve_collisions_shader.bind();
			glDispatchComputeIndirect(3 * sizeof(uint));
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);

		update_rigidbodies_shader.bind();
		update_rigidbodies_shader.setFloat("deltatime", deltatime);
		glDispatchCompute(polygons_count, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


		//std::vector<uint> possible_collisions = ssbo_possible_collisions.copyData<uint>();
		//std::vector<GPURigidbody> rigidbodies = ssbo_rigidbodies.copyData<GPURigidbody>();
		//std::vector<Collision> collisions = ssbo_collisions.copyData<Collision>();
		//std::vector<uint> variables = ssbo_variables.copyData<uint>();
		//std::vector<uint> offsets = ssbo_vertices_offsets.copyData<uint>();
		//std::vector<ce::vec2f> v = ssbo.copyData<ce::vec2f>();
		//std::vector<ce::vec2f> t_v = ssbo_transformed_vertices.copyData<ce::vec2f>();

	/*	for (int i = 0; i < polygons_count; ++i) {
			if (rigidbodies[i].velocity.sqrlen() > 500000) {
				int a = 0;
			}
		}*/
		//if (variables[3] != 0) {
		//	int a = 0;
		//}

		//std::cout << time_elapsed << std::endl;
	}

}