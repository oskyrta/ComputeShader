#pragma once
#include <vector>;
#include "math/vec2.h";
#include "render/drawble.h"
#include "transform.h"
#include "aabb.h"

namespace pc {

	class Polygon : public ce::Drawable {
	public:
		void load(const std::vector<ce::vec2f>& points);
		void draw() const;

		AABB getBoundingBox() const;
		std::vector<ce::vec2f> getTransformedVertices();
		const std::vector<ce::vec2f>& getVertices() const { return vertices; }

	private:
		unsigned int vao = 0;
		unsigned int vbo = 0;

		std::vector<ce::vec2f> vertices;
	};

}