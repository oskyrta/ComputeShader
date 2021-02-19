#include <iostream>
#include "render/program.h"
#include "window.h"
#include "math/vec2.h"
#include <cmath>
#include <ctime>
#include <algorithm>
#include "polygon.h"
#include "aabb.h"

const int width = 512;
const int height = 512;

ce::Window window;
ce::Program line_shader;

#pragma region Helping functions

float q_sqrt(float num) {
	uint32_t i;
	float x2, y;
	const float threehalves = 1.5f;

	x2 = num * 0.5f;
	y = num;
	memcpy(&i, &num, 4);
	i = 0x5f3759df - (i >> 1);
	memcpy(&y, &i, 4);
	y = y * (threehalves - (x2 * y * y));

	return y;
}

float crossProduct(const ce::vec2f& a, const ce::vec2f& b) {
	return a.x * b.y - a.y * b.x;
}

float dotProduct(const ce::vec2f& a, const ce::vec2f& b) {
	return a.x * b.x + a.y * b.y;
}

float projection(const ce::vec2f& a, const ce::vec2f& b) {
	return (a.x * b.x + a.y * b.y) / b.len();
}

ce::vec2f perpendicular(const ce::vec2f& vec) {
	return ce::vec2f{ vec.y, -vec.x };
}

bool clockwise(const ce::vec2f& vec, const ce::vec2f& point) {
	return vec.y * point.x - vec.x * point.y > 0;
}

#pragma endregion

#pragma region Quick hull

void findHull(std::vector<ce::vec2f>& hull, const std::vector<ce::vec2f>& points, ce::vec2f left, ce::vec2f right) {
	if (points.size() == 0) return;

	if (points.size() == 1) {
		hull.push_back(points[0]);
		return;
	}

	float dist = 0;
	ce::vec2f farthest{};
	ce::vec2f perp = perpendicular(left - right);

	for (int i = 0; i < points.size(); ++i) {
		float dot = dotProduct(points[i], perp);
		if (dot > dist) {
			dist = dot;
			farthest = points[i];
		}
	}

	ce::vec2f rf = farthest - right;
	ce::vec2f fl = left - farthest;
	std::vector<ce::vec2f> a;
	std::vector<ce::vec2f> b;

	for (int i = 0; i < points.size(); ++i) {
		if (clockwise(fl, points[i] - farthest)) {
			a.push_back(points[i]);
		}

		if (clockwise(rf, points[i] - right)) {
			b.push_back(points[i]);
		}
	}

	findHull(hull, a, left, farthest);
	hull.push_back(farthest);
	findHull(hull, b, farthest, right);
}

std::vector<ce::vec2f> quickHull(std::vector<ce::vec2f>& points) {
	std::vector<ce::vec2f> hull;

	std::sort(points.begin(), points.end());

	ce::vec2f left = points[0];
	ce::vec2f right = points.back();

	ce::vec2f line = right - left;
	std::vector<ce::vec2f> top;
	std::vector<ce::vec2f> bottom;

	for (int i = 1; i < points.size() - 1; i++) {
		float cross = crossProduct(points[i], line);

		if (cross > 0) {
			bottom.push_back(points[i]);
		}
		else if (cross < 0) {
			top.push_back(points[i]);
		}
	}

	hull.push_back(left);
	findHull(hull, top, left, right);
	hull.push_back(right);
	findHull(hull, bottom, right, left);

	return hull;
}

#pragma endregion

#pragma region Collisions

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

	ce::vec2f contact_point;
	ce::vec2f escape_vector;
	float distance;
};

bool isIntersectig(const pc::AABB& a, const pc::AABB& b) {
	return (a.bottom_left.x < b.top_right.x&& a.top_right.x > b.bottom_left.x) && (a.bottom_left.y <b.top_right.y&& a.top_right.y > b.bottom_left.y);
}

std::vector<ce::vec2f> getMinkowskiDifference(std::vector<ce::vec2f> vertices_a, std::vector<ce::vec2f> vertices_b) {
	std::vector<ce::vec2f> result = std::vector<ce::vec2f>(vertices_a.size() * vertices_b.size());

	for (int i = 0; i < vertices_a.size() - 1; ++i) {
		for (int j = i + 1 ; j < vertices_b.size(); ++j) {
			result[i * vertices_b.size() + j] = vertices_a[i] - vertices_b[j];
		}
	}
}

std::vector<PolygonsPair> getPossibleCollisions(std::vector<pc::Polygon>& polygons) {
	std::vector<PolygonsPair> collisions;

	std::vector<pc::AABB> bounding_boxes = std::vector<pc::AABB>(polygons.size());
	for (int i = 0; i < polygons.size() - 1; ++i) {
		bounding_boxes[i] = polygons[i].getBoundingBox();
	}

	for (int i = 0; i < polygons.size() - 1; ++i) {
		for (int j = i + 1; j < polygons.size(); ++j) {

			if (isIntersectig(bounding_boxes[i], bounding_boxes[j])) {
				auto vertices_a = polygons[i].getTransformedVertices();
				auto vertices_b = polygons[j].getTransformedVertices();

				collisions.push_back({
					&polygons[i],
					&polygons[j],
					vertices_a,
					vertices_b,
					getMinkowskiDifference(vertices_a, vertices_b)
				});
			}

		}
	}

	return collisions;
}

int getFarthestVertexInDirecton(const std::vector<ce::vec2f>& vertices, ce::vec2f direction) {
	int index = 0;
	float distance = 0;

	for (int i = 1; i < vertices.size(); ++i) {
		float dot = dotProduct(vertices[i], direction);

		if (dot > distance) {
			distance = dot;
			index = i;
		}
	}

	return index;
}

void clipLeft(const ce::vec2f& point, const ce::vec2f& direction, ce::vec2f& v1, ce::vec2f& v2) {

}

std::vector<Collision> checkCollisions(const std::vector<PolygonsPair>& pairs) {
	std::vector<Collision> collisions;

	for (const auto& pair : pairs) {
		bool colliding = true;
		ce::vec2f perp;
		float closest_distance = std::numeric_limits<float>::max();

		// Check if minkowski difference contains the origin
		for (int i = 0; i < pair.minkowskiDifference.size(); ++i) {
			const ce::vec2f& point = pair.minkowskiDifference[i];
			const ce::vec2f& next  = pair.minkowskiDifference[i % pair.minkowskiDifference.size()];

			ce::vec2f line = next - point;

			if (!clockwise(line, -point)) {
				colliding = false;
				break;
			}

			// Search for the closest edge
			float dot = dotProduct(-point, perpendicular(line));

			if (dot < closest_distance) {
				closest_distance = dot;
				perp = perpendicular(line);
			}
		}

		if (!colliding) continue;

		ce::vec2f escape_vector = -perp.normalized();
		float distance = closest_distance / perp.len();

		int vertex_a1 = getFarthestVertexInDirecton(pair.transformed_vertices_a,  escape_vector);
		int vertex_b1 = getFarthestVertexInDirecton(pair.transformed_vertices_b, -escape_vector);

		// Search for the most parallel interacting edges
		ce::vec2f ref1 = pair.transformed_vertices_a[vertex_a1];
		ce::vec2f l_a = pair.transformed_vertices_a[(vertex_a1 - 1 + pair.transformed_vertices_a.size()) % pair.transformed_vertices_a.size()];
		ce::vec2f r_a = pair.transformed_vertices_a[(vertex_a1 + 1) % pair.transformed_vertices_a.size()];

		ce::vec2f ref2;
		float cross_a = 0;
		if (crossProduct(l_a - ref1, escape_vector) > crossProduct(r_a, escape_vector)) {
			ref2 = l_a;
			cross_a = crossProduct(l_a - ref1, escape_vector);
		}
		else {
			ref2 = r_a;
			cross_a = crossProduct(r_a - ref1, escape_vector);
		}

		ce::vec2f inc1 = pair.transformed_vertices_b[vertex_b1];
		ce::vec2f l_b = pair.transformed_vertices_b[(vertex_b1 - 1 + pair.transformed_vertices_b.size()) % pair.transformed_vertices_b.size()];
		ce::vec2f r_b = pair.transformed_vertices_b[(vertex_b1 + 1) % pair.transformed_vertices_b.size()];

		ce::vec2f inc2;
		float cross_b = 0;
		if (crossProduct(l_b - inc2, escape_vector) > crossProduct(r_b - inc2, escape_vector)) {
			inc2 = l_b;
			cross_b = crossProduct(l_b - inc2, escape_vector);
		}
		else {
			inc2 = r_b;
			cross_b = crossProduct(r_b - inc2, escape_vector);
		}

		// Find reference and incident edges
		bool swapped = false;
		if (cross_b > cross_a) {
			swapped = true;
			std::swap(ref1, inc1);
			std::swap(ref2, inc2);
		}



		collisions.push_back(
			Collision {
				pair.a,
				pair.b,
				ce::vec2f(),
				escape_vector,
				distance
			}
		);
	}

	return collisions;
}

#pragma endregion


int main() {
	window.load(width, height, "Physics compute");

	line_shader.attachShaderFromFile("line.vert", ce::ShaderType::Vertex);
	line_shader.attachShaderFromFile("line.frag", ce::ShaderType::Fragment);
	line_shader.link();

	srand(time(0));

	float t = static_cast<float>(10);
	std::vector<pc::Polygon> polygons;

	for (int i = 0; i < 30; ++i) {
		pc::Polygon polygon;

		std::vector<ce::vec2f> points;
		for (int i = 0; i < 20; ++i) {
			points.push_back(ce::vec2{ rand() % 40 - 20, rand() % 40 - 20 });
		}

		auto hull = quickHull(points);

		polygon.load(hull);
		polygon.setProgram(&line_shader);
		polygon.getTransform()->setPosition(ce::vec2{ rand() % 512, rand() % 512 });

		polygons.push_back(polygon);
	}

	while (!window.shouldClose()) {
		window.pollEvents();

		glClear(GL_COLOR_BUFFER_BIT);
		for (const auto& polygon : polygons) {
			window.getRenderTarget()->draw(&polygon);
		}

		window.swapBuffers();
	}

	return 0;
}