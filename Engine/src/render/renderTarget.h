#pragma once
#include <GL/glew.h>
#include <math/mat3.h>


namespace ce {

	struct Drawable;

	struct RenderTarget {
		inline static bool logging = false;

		virtual ~RenderTarget() = default;

		virtual void load(int width, int height);
		void draw(const Drawable* object);

		GLuint getFramebuffer() { return framebuffer; }
		mat3f getProjectionMat() { return projection; }

		void setView(mat3f view) { this->view = view; };
			 
	protected:
		GLuint framebuffer = 0;

		int width = 0;
		int height = 0;

		mat3f view = mat3f(1);
		mat3f projection;
	};

}