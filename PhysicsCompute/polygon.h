#pragma once
#include <vector>
#include "math/vec2.h"
#include "render/drawble.h"
#include "transform.h"
#include "aabb.h"
#include "rigidbody.h"
#include "transform.h"

namespace pc {

	class Polygon : public ce::Drawable {
	public:
		Polygon() : rigidbody(&transform) {}

		void load(const std::vector<ce::vec2f>& points);
		void draw() const;

		AABB getBoundingBox() const;
		std::vector<ce::vec2f> getTransformedVertices() const;

		const std::vector<ce::vec2f>& getVertices() const { return vertices; }
		Rigidbody* getRigidbody() { return &rigidbody; }
		const Rigidbody* getRigidbody() const { return &rigidbody; }

	public:
		ce::Transform transform;

	private:
		unsigned int vao = 0;
		unsigned int vbo = 0;

		Rigidbody rigidbody;

		float area;
		ce::vec2f centroid;

		std::vector<ce::vec2f> vertices;
	};

}