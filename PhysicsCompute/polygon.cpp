#pragma once
#include "polygon.h"
#include <limits>

namespace pc {

	AABB Polygon::getBoundingBox() const {
		AABB aabb = AABB(
			ce::vec2f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
			ce::vec2f(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest())
		);

		auto translate = transform.getTranslate();

		for (int i = 0; i < vertices.size(); ++i) {
			auto point = translate * ce::vec3f(vertices[i].x, vertices[i].y, 1);

			aabb.bottom_left.x = std::fmin(aabb.bottom_left.x, point.x);
			aabb.bottom_left.y = std::fmin(aabb.bottom_left.y, point.y);
			aabb.top_right.x = std::fmax(aabb.top_right.x, point.x);
			aabb.top_right.y = std::fmax(aabb.top_right.y, point.y);
		}

		return aabb;
	}

	std::vector<ce::vec2f> pc::Polygon::getTransformedVertices() const
	{
		std::vector<ce::vec2f> result = std::vector<ce::vec2f>(vertices.size());
		ce::mat3f translate = transform.getTranslate();

		for (int i = 0; i < vertices.size(); ++i) {
			result[i] = translate * vertices[i];
		}

		return result;
	}

	void Polygon::draw() const {
		program->setMatrix("model", transform.getTranslate());
		program->setFloat("depth", transform.getDepth() / 1000);

		glBindVertexArray(vao);
		glDrawArrays(GL_LINE_LOOP, 0, vertices.size());
		glBindVertexArray(0);
	}

	void Polygon::load(const std::vector<ce::vec2f>& vertices) {
		this->vertices = vertices;

		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &vao);

		glBindVertexArray(vao);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ce::vec2f) * this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ce::vec2f), (GLvoid*)0);
			glEnableVertexAttribArray(0);

		glBindVertexArray(0);


		area = 0;
		centroid = { 0, 0 };

		for (int i = 0; i < vertices.size(); ++i)
		{
			ce::vec2f p1 = vertices[i];
			int next = (i + 1) % vertices.size();
			ce::vec2f p2 = vertices[next];

			float cross = p1.cross(p2);
			float triangleArea = 0.5f * cross;

			area += triangleArea;

			centroid += (p1 + p2) * triangleArea / 3.f;
		}

		centroid /= area;

		area = std::abs(area);

		transform.setOrigin(centroid);
		rigidbody.load(vertices, centroid, area);
	}

}