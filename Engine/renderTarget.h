#pragma once
#include <GL/glew.h>
#include <glm/vec2.hpp>

namespace ce {
	
	struct Drawable;

	struct RenderTarget {
		void load(int width, int height);
		void draw(const Drawable* object);

	private:
		int width;
		int height;
		GLuint framebuffer;
	};

}