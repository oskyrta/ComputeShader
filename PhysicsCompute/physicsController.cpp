#include <iostream>
#include "physicsController.h"
#include "polygon.h"
#include "aabb.h"
#include "quickhull.h"
#include "math/math.h"
#include <chrono>

namespace pc {


	struct PolygonsPair {
		pc::Polygon* a;
		pc::Polygon* b;

		std::vector<ce::vec2f> transformed_vertices_a;
		std::vector<ce::vec2f> transformed_vertices_b;

		std::vector<ce::vec2f> minkowskiDifference;
	};

	struct Collision {
		pc::Polygon* a;
		pc::Polygon* b;

		ce::vec2f contact_point_a;
		ce::vec2f contact_point_b;
		ce::vec2f normal;
		float depth;
	};

	bool isIntersectig(const pc::AABB& a, const pc::AABB& b) {
		return (a.bottom_left.x < b.top_right.x && a.top_right.x > b.bottom_left.x) && (a.bottom_left.y <b.top_right.y&& a.top_right.y > b.bottom_left.y);
	}

	std::vector<ce::vec2f> getMinkowskiDifference(std::vector<ce::vec2f> vertices_a, std::vector<ce::vec2f> vertices_b) {
		std::vector<ce::vec2f> result = std::vector<ce::vec2f>(vertices_a.size() * vertices_b.size());

		for (int i = 0; i < vertices_a.size(); ++i) {
			for (int j = 0; j < vertices_b.size(); ++j) {
				result[i * vertices_b.size() + j] = vertices_a[i] - vertices_b[j];
			}
		}

		return result;
	}

	std::vector<PolygonsPair> getPossibleCollisions(std::vector<pc::Polygon>& polygons) {
		std::vector<PolygonsPair> collisions;

		std::vector<pc::AABB> bounding_boxes = std::vector<pc::AABB>(polygons.size());
		for (int i = 0; i < polygons.size(); ++i) {
			bounding_boxes[i] = polygons[i].getBoundingBox();
		}

		for (int i = 0; i < polygons.size() - 1; ++i) {
			for (int j = i + 1; j < polygons.size(); ++j) {

				if (isIntersectig(bounding_boxes[i], bounding_boxes[j])) {
					auto vertices_a = polygons[i].getTransformedVertices();
					auto vertices_b = polygons[j].getTransformedVertices();
					auto minkowskiDifference = getMinkowskiDifference(vertices_a, vertices_b);

					collisions.push_back({
						&polygons[i],
						&polygons[j],
						vertices_a,
						vertices_b,
						quickHull(minkowskiDifference)
					});
				}

			}
		}

		return collisions;
	}

	int getFarthestVertexInDirecton(const std::vector<ce::vec2f>& vertices, ce::vec2f direction) {
		int index = 0;
		float distance = std::numeric_limits<float>::lowest();

		for (int i = 0; i < vertices.size(); ++i) {
			float dot = vertices[i].dot(direction);

			if (dot > distance) {
				distance = dot;
				index = i;
			}
		}

		return index;
	}

	void clipLeft(const ce::vec2f& point, const ce::vec2f& direction, ce::vec2f& v1, ce::vec2f& v2) {
		bool v1_cw = direction.clockwise(v1 - point);
		bool v2_cw = direction.clockwise(v2 - point);

		float x, y;

		if (v1_cw != v2_cw) {
			ce::vec2f point2 = point + direction;

			if (point.x == point2.x) {
				x = point.x;
				float n = (point.x - v1.x) / (v2.x - v1.x);
				y = v1.y + n * (v2.y - v1.y);
			}
			else if (v1.x == v2.x) {
				x = v1.x;
				float n = (v1.x - point.x) / (point2.x - point.x);
				y = point.y + n * (point2.y - point.y);
			}
			else {
				float k1 = (point.y - point2.y) / (point.x - point2.x);
				float b1 = point2.y - k1 * point2.x;

				float k2 = (v1.y - v2.y) / (v1.x - v2.x);
				float b2 = v2.y - k2 * v2.x;

				x = (b2 - b1) / (k1 - k2);
				y = k1 * x + b1;
			}

			if (v2_cw) {
				v1 = { x, y };
			}
			else {
				v2 = { x, y };
			}
		}
	}

	float getMostParallelEdge(const std::vector<ce::vec2f>& vertices, int vertexId, ce::vec2f direction, ce::vec2f& v1, ce::vec2f& v2) {
		ce::vec2f v = vertices[vertexId];
		ce::vec2f l = vertices[(vertexId + 1) % vertices.size()];
		ce::vec2f r = vertices[(vertexId - 1 + vertices.size()) % vertices.size()];

		float dot_l = (v - l).normalized().dot(direction);
		float dot_r = (v - r).normalized().dot(direction);

		if (dot_r <= dot_l) {
			v1 = r;
			v2 = v;
			return dot_r;
		}
		else {
			v1 = v;
			v2 = l;
			return dot_l;
		}
	}

	std::vector<Collision> checkCollisions(const std::vector<PolygonsPair>& pairs) {
		std::vector<Collision> collisions;

		for (const auto& pair : pairs) {
			bool colliding = true;
			ce::vec2f perp{};
			float depth = std::numeric_limits<float>::max();

			// Check if minkowski difference contains the origin
			for (int i = 0; i < pair.minkowskiDifference.size(); ++i) {
				const ce::vec2f& point = pair.minkowskiDifference[i];
				const ce::vec2f& next = pair.minkowskiDifference[(i + 1) % pair.minkowskiDifference.size()];

				ce::vec2f line = next - point;

				if (!line.clockwise(-point)) {
					colliding = false;
					break;
				}

				// Search for the closest edge
				float dot = -point.dot(line.perpendicular().normalized());

				if (dot < depth) {
					depth = dot;
					perp = line.perpendicular();
				}
			}

			if (!colliding) continue;

			ce::vec2f escape_vector = -perp.normalized();

			int vertex_a1 = getFarthestVertexInDirecton(pair.transformed_vertices_a, escape_vector);
			int vertex_b1 = getFarthestVertexInDirecton(pair.transformed_vertices_b, -escape_vector);

			// Search for the most parallel interacting edges
			ce::vec2f ref1, ref2;
			float dot_a = getMostParallelEdge(pair.transformed_vertices_a, vertex_a1, escape_vector, ref1, ref2);

			ce::vec2f inc1, inc2;
			float dot_b = getMostParallelEdge(pair.transformed_vertices_b, vertex_b1, -escape_vector, inc1, inc2);

			// Find reference and incident edges
			bool swapped = std::abs(dot_b) < std::abs(dot_a);
			if (swapped) {
				std::swap(ref1, inc1);
				std::swap(ref2, inc2);
			}

			ce::vec2f ref_perp = (ref2 - ref1).perpendicular();
			clipLeft(ref1, -ref_perp, inc1, inc2);
			clipLeft(ref2, ref_perp, inc1, inc2);


			float dot_inc1 = (inc1 - ref1).dot(ref_perp);
			float dot_inc2 = (inc2 - ref1).dot(ref_perp);

			ce::vec2f contact_b;
			if (dot_inc1 > 0 && dot_inc2 > 0 && std::abs(dot_inc1 - dot_inc2) < 0.1f) {
				contact_b = (inc1 + inc2) / 2.f;
			}
			else {
				if (dot_inc1 > dot_inc2) {
					contact_b = inc1;
				}
				else {
					contact_b = inc2;
				}
			}
			ce::vec2f contact_a = contact_b + escape_vector * depth * (swapped ? -1 : 1);

			if (swapped) {
				std::swap(contact_a, contact_b);
			}

			collisions.push_back(
				Collision{
					pair.a,
					pair.b,
					contact_a - pair.a->transform.getPosition(),
					contact_b - pair.b->transform.getPosition(),
					escape_vector,
					depth
				}
			);
		}

		return collisions;
	}

	void updatePhysics(std::vector<Polygon>& polygons, float deltatime) {
		auto collisions = checkCollisions( getPossibleCollisions(polygons) );

		for (Collision& collision : collisions) {
			Rigidbody* rigidbody_a = collision.a->getRigidbody();
			Rigidbody* rigidbody_b = collision.b->getRigidbody();

			if (rigidbody_a->inv_mass == 0 && rigidbody_b->inv_mass == 0) continue;

			float e = std::min(rigidbody_a->e, rigidbody_b->e);
			ce::vec2f velocity_ab = 
				(rigidbody_a->velocity - collision.contact_point_a.perpendicular() * rigidbody_a->angular_velocity) - 
				(rigidbody_b->velocity - collision.contact_point_b.perpendicular() * rigidbody_b->angular_velocity);

			float n_velocity_ab = (velocity_ab).dot(collision.normal);

			float j = -(1 + e) * n_velocity_ab;
			j /= rigidbody_a->inv_mass + 
				 rigidbody_b->inv_mass + 
				 ce::math::sqr(collision.contact_point_a.cross(collision.normal)) * rigidbody_a->inv_moment_of_inertia + 
				 ce::math::sqr(collision.contact_point_b.cross(collision.normal)) * rigidbody_b->inv_moment_of_inertia;

			rigidbody_a->velocity += collision.normal * rigidbody_a->inv_mass * j;
			rigidbody_b->velocity -= collision.normal * rigidbody_b->inv_mass * j;

			rigidbody_a->angular_velocity += collision.contact_point_a.cross(collision.normal * j) * rigidbody_a->inv_moment_of_inertia;
			rigidbody_b->angular_velocity -= collision.contact_point_b.cross(collision.normal * j) * rigidbody_b->inv_moment_of_inertia;

			const float percent = 0.2f;
			const float slop = 0.8f;
			ce::vec2f correction = collision.normal * percent * (std::max(collision.depth - slop, 0.0f) / (collision.a->getRigidbody()->inv_mass + collision.b->getRigidbody()->inv_mass));
			collision.a->transform.move(-correction * collision.a->getRigidbody()->inv_mass);
			collision.b->transform.move( correction * collision.b->getRigidbody()->inv_mass);
		}

		for (auto& polygon : polygons) {
			polygon.getRigidbody()->velocity += polygon.getRigidbody()->force * deltatime;
			polygon.getRigidbody()->angular_velocity += polygon.getRigidbody()->torque * deltatime;

			polygon.transform.move(polygon.getRigidbody()->velocity * deltatime);
			polygon.transform.rotate(polygon.getRigidbody()->angular_velocity * deltatime);
		}

		return;
	}

}