#include <GL/glew.h>
#include "renderTarget.h"
#include "drawble.h"
#include <iostream>

namespace ce {

	void RenderTarget::load(int width, int height) { 
		this->width = width;
		this->height = height;

		projection = mat3f::ortho(0, width, 0, height);
	}
	
	void RenderTarget::draw(const Drawable* object) {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glViewport(0, 0, width, height);

		const Program* program = object->getProgram();

		program->bind();
		program->setMatrix("projection", projection);
		program->setMatrix("view", view);

		object->draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}