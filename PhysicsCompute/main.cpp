#include <iostream>
#include "render/program.h"
#include "window.h"
#include "math/vec2.h"
#include <cmath>
#include <ctime>
#include <algorithm>
#include "polygon.h"
#include "aabb.h"
#include <chrono>
#include "quickhull.h"
#include "math/math.h"
#include "physicsController.h"
#include "opengl/ssbo.h"
#include "gpuPolygon.h"
#include "gpuPhysicsController.h"

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

const int width = 512;
const int height = 512;

bool lbutton_pressed = false;

ce::Window window;

//
//struct Collision {
//	uint a;
//	uint b;
//	uint v_a;
//	uint v_b;
//	float dist;
//	float align;
//	ce::vec2f normal;
//};
//
//struct GPURigidbody {
//	float e;
//	float rotation;
//	float inv_mass;
//	float inv_moment_of_inertia;
//
//	ce::vec2f position;
//
//	float angular_velocity;
//	float torque;
//
//	ce::vec2f velocity;
//	ce::vec2f force;
//};
//
//
//ce::Program sqr_shader;
ce::Program line_shader;
//ce::Program update_rigidbodies;
//ce::Program resolve_collisions;
//ce::Program calculate_aabbs_shader;
ce::Program transformed_line_shader;
//ce::Program transform_vertices_shader;
//ce::Program possible_collisions_shader;
//ce::Program separated_axis_check_shader;
//
//ce::gl::SSBO ssbo;
//ce::gl::SSBO ssbo_vertices;
//ce::gl::SSBO ssbo_indices;
//ce::gl::SSBO ssbo_vertices_offsets;
//ce::gl::SSBO ssbo_transformed_vertices;
//ce::gl::SSBO ssbo_matrices;
//ce::gl::SSBO ssbo_aabbs;
//ce::gl::SSBO ssbo_possible_collisions;
//ce::gl::SSBO ssbo_variables;
//ce::gl::SSBO ssbo_normals;
//ce::gl::SSBO ssbo_transformed_normals;
//ce::gl::SSBO ssbo_collisions;
//ce::gl::SSBO ssbo_rigidbodies;
//
//std::vector<uint> indices;
//std::vector<ce::vec2f> vertices;
//std::vector<ce::vec2f> normals;
//std::vector<uint> vertices_offsets;
std::vector<pc::Polygon> polygons;
//std::vector<pc::GPUPolygon> gpu_polygons;
//
//size_t possible_collisions_max_count;
//
//unsigned int getPairsCount(unsigned int objects_count) {
//	return objects_count * (objects_count - 1) / 2;
//}

pc::Polygon createPolygon() {
	pc::Polygon polygon;

	std::vector<ce::vec2f> points;
	for (int i = 0; i < 20; ++i) {
		points.push_back(ce::vec2{ rand() % 40 - 20, rand() % 40 - 20 });
	}

	auto hull = quickHull(points);

	polygon.load(hull);
	polygon.setProgram(&line_shader);

	return polygon;
}

void initScene() {
	polygons.clear();

	for (int i = 0; i < 10; ++i) {
		pc::Polygon polygon = createPolygon();

		polygon.transform.setPosition(ce::vec2{ rand() % 512, rand() % 512 });

		polygon.getRigidbody()->velocity = ce::vec2{ rand() % 100 - 50, rand() % 100 - 50 };
		polygon.getRigidbody()->force = { 0, -250 };

		polygons.push_back(polygon);
	}

	pc::Polygon floor;
	floor.load({ {0, 0}, {0, 10}, {512, 10}, {512, 0} });
	floor.transform.setPosition({ 256, 0 });
	floor.getRigidbody()->makeStatic();
	floor.setProgram(&line_shader);

	pc::Polygon wall1;
	wall1.load({ {0, 0}, {0, 512}, {10, 512}, {10, 0} });
	wall1.transform.setPosition({ 507, 256 });
	wall1.getRigidbody()->makeStatic();
	wall1.setProgram(&line_shader);

	pc::Polygon wall2;
	wall2.load({ {0, 0}, {0, 512}, {10, 512}, {10, 0} });
	wall2.transform.setPosition({ 5, 256 });
	wall2.getRigidbody()->makeStatic();
	wall2.setProgram(&line_shader);

	polygons.push_back(floor);
	polygons.push_back(wall1);
	polygons.push_back(wall2);
}

uint* variables;
//void initShaders() {
//	std::vector<GPURigidbody> rigidbodies;
//
//	vertices.clear();
//	indices.clear();
//	normals.clear();
//	vertices_offsets.clear();
//
//	unsigned int polygons_count = polygons.size();
//
//	possible_collisions_max_count = getPairsCount(polygons_count);
//
//	uint i = 0;
//	for (const pc::Polygon& polygon : polygons) {
//		vertices_offsets.push_back(vertices.size());
//
//		const auto& p_vertices = polygon.getVertices();
//
//		for (int v = 0; v < p_vertices.size(); ++v) {
//			ce::vec2f normal = -(p_vertices[(v + 1) % p_vertices.size()] - p_vertices[v]).perpendicular();
//			normals.push_back(normal.normalized());
//
//			vertices.push_back(p_vertices[v] - polygon.transform.getOrigin());
//		}
//
//		const pc::Rigidbody* rigidbody = polygon.getRigidbody();
//		rigidbodies.push_back(GPURigidbody {
//			rigidbody->e,
//			polygon.transform.getRotation(),
//			rigidbody->inv_mass,
//			rigidbody->inv_moment_of_inertia,
//
//			polygon.transform.getPosition(),
//
//			rigidbody->angular_velocity,
//			rigidbody->torque,
//			
//			rigidbody->velocity,
//			rigidbody->force
//		});
//		indices.insert(indices.end(), p_vertices.size(), i++);
//	}
//	vertices_offsets.push_back(vertices.size());
//
//	ssbo_vertices
//		.create<ce::vec2f>(vertices.size())
//		.bindBase(Buffer_Vertices)
//		.setData<ce::vec2f>(vertices.data(), vertices.size());
//
//	ssbo_vertices_offsets
//		.create<uint>(vertices_offsets.size())
//		.bindBase(Buffer_VerticesOffsets)
//		.setData<uint>(vertices_offsets.data(), vertices_offsets.size());
//
//	ssbo_indices
//		.create<uint>(indices.size())
//		.bindBase(Buffer_Indices)
//		.setData<uint>(indices.data(), indices.size());
//
//	ssbo_transformed_vertices
//		.create<ce::vec2f>(vertices.size())
//		.bindBase(Buffer_TransformedVertices);
//
//	ssbo_matrices
//		.create<float>(4 * 3 * polygons.size())
//		.bindBase(Buffer_Matrices);
//
//	ssbo_aabbs
//		.create<pc::AABB>(polygons.size())
//		.bindBase(Buffer_AABBs);
//
//	ssbo_possible_collisions
//		.create<uint>((possible_collisions_max_count + 1) * 2)
//		.bindBase(Buffer_PossibleCollisions);
//
//	ssbo_variables
//		.create<uint>(6)
//		.bindBase(Buffer_Variables);
//
//	ssbo_normals
//		.create<ce::vec2f>(normals.size())
//		.bindBase(Buffer_Normals)
//		.setData<ce::vec2f>(normals.data(), normals.size());
//
//	ssbo_transformed_normals
//		.create<ce::vec2f>(normals.size())
//		.bindBase(Buffer_TranformedNormals);
//
//	ssbo_collisions
//		.create<Collision>(possible_collisions_max_count)
//		.bindBase(Buffer_Collisions);
//
//	ssbo_rigidbodies
//		.create<GPURigidbody>(polygons.size())
//		.bindBase(Buffer_Rigidbodies)
//		.setData<GPURigidbody>(rigidbodies.data(), rigidbodies.size());
//
//	for (uint i = 0; i < polygons.size(); ++i) {
//		pc::GPUPolygon p;
//		p.load(ssbo_transformed_vertices.getBufferId(), vertices_offsets[i], polygons[i].getVertices().size());
//		p.setProgram(&transformed_line_shader);
//
//		gpu_polygons.push_back(p);
//	}
//
//	update_rigidbodies.bind();
//	update_rigidbodies.setFloat("deltatime", 0);
//	glDispatchCompute(polygons.size(), 1, 1);
//	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//
//	transform_vertices_shader.bind();
//	glDispatchCompute(vertices.size(), 1, 1);
//	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//}

void init() {
	initScene();
	pc::gpu::init(polygons);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			init();
			lbutton_pressed = true;
		}
		else if (action == GLFW_RELEASE) {
			lbutton_pressed = false;
		}
	}
}

void countFPS(double deltatime) {
	static int passed_frames = 0;
	static double time_past = 0;

	passed_frames++;
	time_past += deltatime;

	while (time_past > 1.) {
		std::cout << "FPS: " << passed_frames << std::endl;
		passed_frames = 0;
		time_past -= 1;
	}
}
//
//void updatePhysicsGPU(const std::vector<pc::Polygon>& polygons, float deltatime) {
//	auto start = std::chrono::high_resolution_clock::now();
//
//	// transform vertices
//	transform_vertices_shader.bind();
//	glDispatchCompute(vertices.size(), 1, 1);
//	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//
//	// calculate aabb for each polygon
//	calculate_aabbs_shader.bind();
//	calculate_aabbs_shader.setUint("points_count", vertices.size());
//	calculate_aabbs_shader.setUint("polygons_count", polygons.size());
//	glDispatchCompute(1, 1, 1);
//	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//
//	// find possible collisions
//	possible_collisions_shader.bind();
//	possible_collisions_shader.setUint("possible_collisions_count", possible_collisions_max_count);
//	glDispatchCompute(polygons.size(), getPairsCount(polygons.size() + 1) / polygons.size(), 1);
//	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//
//	glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, ssbo_variables.getBufferId());
//		separated_axis_check_shader.bind();
//		glDispatchComputeIndirect(0);
//		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//
//		resolve_collisions.bind();
//		glDispatchComputeIndirect(3 * sizeof(uint));
//		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//	glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
//
//	update_rigidbodies.bind();
//	update_rigidbodies.setFloat("deltatime", deltatime);
//	glDispatchCompute(polygons.size(), 1, 1);
//	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//
//	double time_elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count();
//
//	//std::vector<uint> possible_collisions = ssbo_possible_collisions.copyData<uint>();
//	std::vector<GPURigidbody> rigidbodies = ssbo_rigidbodies.copyData<GPURigidbody>();
//	std::vector<Collision> collisions = ssbo_collisions.copyData<Collision>();
//	std::vector<uint> variables = ssbo_variables.copyData<uint>();
//	//std::vector<uint> offsets = ssbo_vertices_offsets.copyData<uint>();
//	//std::vector<ce::vec2f> v = ssbo.copyData<ce::vec2f>();
//	//std::vector<ce::vec2f> t_v = ssbo_transformed_normals.copyData<ce::vec2f>();
//
//	for (int i = 0; i < polygons.size(); ++i) {
//		if (rigidbodies[i].velocity.sqrlen() > 500000) {
//			int a = 0;
//		}
//	}
//	//if (variables[3] != 0) {
//	//	int a = 0;
//	//}
//
//	//std::cout << time_elapsed << std::endl;
//}

int main() {
	window.load(width, height, "Physics compute");
	glfwSetMouseButtonCallback(window.getGLFWwindow(), mouse_button_callback);


	pc::gpu::startup();

	//transform_vertices_shader.attachShaderFromFile("transform_vertices.comp", ce::ShaderType::Compute);
	//transform_vertices_shader.link();

	//calculate_aabbs_shader.attachShaderFromFile("calculate_aabbs.comp", ce::ShaderType::Compute);
	//calculate_aabbs_shader.link();

	//possible_collisions_shader.attachShaderFromFile("possible_collisions.comp", ce::ShaderType::Compute);
	//possible_collisions_shader.link();

	//separated_axis_check_shader.attachShaderFromFile("separated_axis_check.comp", ce::ShaderType::Compute);
	//separated_axis_check_shader.link();

	//resolve_collisions.attachShaderFromFile("resolve_collisions.comp", ce::ShaderType::Compute);
	//resolve_collisions.link();

	//update_rigidbodies.attachShaderFromFile("update_rigidbodies.comp", ce::ShaderType::Compute);
	//update_rigidbodies.link();

	line_shader.attachShaderFromFile("line.vert", ce::ShaderType::Vertex);
	line_shader.attachShaderFromFile("line.frag", ce::ShaderType::Fragment);
	line_shader.link();

	//int numbers[] = { 2, 3, 4, 5, 6 };
	//int sqr_numbers[10000] = { };

	//sqr_shader.attachShaderFromFile("sqr.comp", ce::ShaderType::Compute);
	//sqr_shader.link();
	//sqr_shader.bind();

	//sqr_shader.setUint("t", 0);
	//glDispatchCompute(1, 1, 1);
	//
	//sqr_shader.setUint("t", 1);
	//glDispatchCompute(1, 1, 1);

	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	//

	//ssbo.getData(sqr_numbers, sizeof(sqr_numbers));



	srand(time(0));


	//pc::Polygon polygon1;
	//polygon1.load({ {200, 150}, {200, 250}, {205, 250}, {205, 150} });
	//polygon1.getTransform()->setPosition({ 200, 220 });
	//polygon1.setProgram(&line_shader);
	//polygon1.getRigidbody()->angular_velocity = 3;
	//polygon1.getRigidbody()->velocity = { 50, 0 };
	////polygon1.getRigidbody()->force = { 0, -150 };

	//pc::Polygon polygon2;
	//polygon2.load({ {200, 240}, {210, 250}, {230, 240}, {220, 220} });
	//polygon2.getTransform()->setPosition({ 250, 200 });
	//polygon2.setProgram(&line_shader);

	//polygons.push_back(polygon1);
	//polygons.push_back(polygon2);

	initScene();
	pc::gpu::init(polygons);

	const float physics_deltatime = 1.f / 500;

	//updatePhysicsGPU(polygons, physics_deltatime);

	auto last = std::chrono::high_resolution_clock::now();
	int passed_frames = 0;
	double time_past = 0;

	while (!window.shouldClose()) {
		window.pollEvents();
		glClear(GL_COLOR_BUFFER_BIT);

		pc::gpu::draw(window.getRenderTarget());

		//for (const auto& polygon : gpu_polygons) {
		//	window.getRenderTarget()->draw(&polygon);
		//}

		auto current_time = std::chrono::high_resolution_clock::now();
		double deltatime = std::chrono::duration<double>(current_time - last).count();
		last = current_time;

		countFPS(deltatime);

		time_past += deltatime;

		time_past = std::min(time_past, 2.);

		while (time_past > physics_deltatime) {
			//pc::updatePhysics(polygons, physics_deltatime);
			pc::gpu::updatePhysics(physics_deltatime);
			time_past -= physics_deltatime;
		}

		window.swapBuffers();
	}

	return 0;
}