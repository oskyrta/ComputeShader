#pragma once
#include "render/drawble.h"

namespace pc {

	class GPUContactPoints : public ce::Drawable {
	public:
		void load(GLuint buffer);
		void draw() const;

		void setCount(unsigned int count) { this->count = count; }

	private:
		unsigned int vao = 0;

		GLuint buffer;
		unsigned int count = 0;
	};

}