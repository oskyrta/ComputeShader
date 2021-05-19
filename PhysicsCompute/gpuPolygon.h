#pragma once
#include "render/drawble.h"

namespace pc {

	class GPUPolygon : public ce::Drawable{
	public:
		void load(GLuint buffer, unsigned int offset, unsigned int size);
		void draw() const;

	private:
		unsigned int vao = 0;

		GLuint buffer;
		unsigned int offset;
		unsigned int size;
	};

}