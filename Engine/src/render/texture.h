#pragma once
#include "GL/glew.h"
#include <math/vec2.h>

namespace ce {

	struct Texture {
		inline static bool logging = false;

		void load(int width, int height, GLuint internal_format = GL_RGBA, GLuint format = GL_RGBA);
		void load(const char* path);
		void bind() const { glBindTexture(GL_TEXTURE_2D, texture); }
		void terminate() { glDeleteTextures(1, &texture); texture = 0; };

		vec2f getSize() const { return vec2f(width, height); }
		GLuint getID() { return texture; }

	private:
		void init();

		int width;
		int height;
		GLuint texture;
	};

}
