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

	ce::vec2f contact_point_a;
	ce::vec2f contact_point_b;
	ce::vec2f escape_vector;
	float distance;
};

bool isIntersectig(const pc::AABB& a, const pc::AABB& b) {
	return (a.bottom_left.x < b.top_right.x&& a.top_right.x > b.bottom_left.x) && (a.bottom_left.y <b.top_right.y&& a.top_right.y > b.bottom_left.y);
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
					quickHull( minkowskiDifference )
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
		float dot = dotProduct(vertices[i], direction);

		if (dot > distance) {
			distance = dot;
			index = i;
		}
	}

	return index;
}

void clipLeft(const ce::vec2f& point, const ce::vec2f& direction, ce::vec2f& v1, ce::vec2f& v2) {
	bool v1_cw = clockwise(direction, v1 - point);
	bool v2_cw = clockwise(direction, v2 - point);

	ce::vec2f t = v2 - point;

	float x, y;

	if (v1_cw != v2_cw) {
		ce::vec2f point2 = point + direction;

		if (point.x - point2.x == 0) {
			x = point.x;
			float n = (point.x - v1.x) / (v2.x - v1.x);
			y = v1.y + n * (v2.y - v1.y);
		}
		else if (v1.x - v2.x == 0) {
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

	float dot_l = dotProduct((v - l).normalized(), direction);
	float dot_r = dotProduct((v - r).normalized(), direction);

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
			const ce::vec2f& next  = pair.minkowskiDifference[(i + 1) % pair.minkowskiDifference.size()];

			ce::vec2f line = next - point;

			if (!clockwise(line, -point)) {
				colliding = false;
				break;
			}

			// Search for the closest edge
			float dot = dotProduct(-point, perpendicular(line).normalized());

			if (dot < depth) {
				depth = dot;
				perp = perpendicular(line);
			}
		}

		if (!colliding) continue;

		ce::vec2f escape_vector = -perp.normalized();

		int vertex_a1 = getFarthestVertexInDirecton(pair.transformed_vertices_a,  escape_vector);
		int vertex_b1 = getFarthestVertexInDirecton(pair.transformed_vertices_b, -escape_vector);

		// Search for the most parallel interacting edges
		ce::vec2f ref1, ref2;
		float dot_a = getMostParallelEdge(pair.transformed_vertices_a, vertex_a1, escape_vector, ref1, ref2);

		ce::vec2f inc1, inc2;
		float dot_b = getMostParallelEdge(pair.transformed_vertices_b, vertex_b1, -escape_vector, inc1, inc2);

		// Find reference and incident edges
		bool swapped = false;
		if (std::abs(dot_b) < std::abs(dot_a)) {
			swapped = true;
			std::swap(ref1, inc1);
			std::swap(ref2, inc2);
		}

		ce::vec2f ref_perp = perpendicular(ref2 - ref1);
		clipLeft(ref1, -ref_perp, inc1, inc2);
		clipLeft(ref2,  ref_perp, inc1, inc2);


		float dot_inc1 = dotProduct(inc1 - ref1, ref_perp);
		float dot_inc2 = dotProduct(inc2 - ref1, ref_perp);

		ce::vec2f contact_b;
		if (dot_inc1 > 0 && dot_inc2 > 0 && dot_inc1 == dot_inc2) {
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
			Collision {
				pair.a,
				pair.b,
				contact_a,
				contact_b,
				escape_vector,
				depth
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

	/*for (int i = 0; i < 30; ++i) {
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
	}*/

	pc::Polygon polygon1;
	polygon1.load({ {200, 200}, {200, 230}, {240, 230}, {240, 200} });
	polygon1.setProgram(&line_shader);

	pc::Polygon polygon2;
	polygon2.load({ {200, 240}, {210, 250}, {230, 240}, {220, 220} });
	polygon2.setProgram(&line_shader);

	polygons.push_back(polygon2);
	polygons.push_back(polygon1);

	auto last = std::chrono::high_resolution_clock::now();
	double time_past = 0;
	int passed_frames = 0;
	while (!window.shouldClose()) {
		window.pollEvents();

		glClear(GL_COLOR_BUFFER_BIT);
		for (const auto& polygon : polygons) {
			window.getRenderTarget()->draw(&polygon);
		}

		auto collisions = checkCollisions(getPossibleCollisions(polygons));

		passed_frames++;
		auto now = std::chrono::high_resolution_clock::now();
		time_past += std::chrono::duration<double, std::milli>( now - last ).count();
		last = now;

		if (time_past > 1000.) {
			time_past -= 1000;
			std::cout << "FPS: " << passed_frames << std::endl;
			passed_frames = 0;
		}

#ifdef _DEBUG
		window.swapBuffers();
#else
		window.flush();
#endif

	}

	return 0;
}