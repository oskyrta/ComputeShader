#pragma once
#include "drawble.h"
#include <math/vec2.h>

namespace ce {

	struct Texture;

	struct Sprite : public Drawable {
		void load(vec2f size, const Texture* texture);
		void draw() const;

	public:
		Transform transform;

	private:
		unsigned int vao = 0;
		unsigned int vbo = 0;

		vec2f size;
		const Texture* texture = nullptr;
	};

}