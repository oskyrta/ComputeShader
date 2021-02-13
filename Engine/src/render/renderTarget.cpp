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

		program->use();
		program->setMatrix("projection", projection);
		program->setMatrix("view", view);
		program->setMatrix("model", object->getTransform()->getTranslate());
		program->setFloat("depth", object->getTransform()->getDepth() / 1000);

		object->draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}