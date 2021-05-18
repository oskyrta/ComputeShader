#include "gpuContactPoints.h"

namespace pc {

	void GPUContactPoints::load(GLuint buffer) {
		this->buffer = buffer;
		
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
			glPointSize(3);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ce::vec2f), (GLvoid*)0);
			glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	void GPUContactPoints::draw() const {
		program->setVector("line_color", 1, 0, 0, 1);

		glBindVertexArray(vao);
			glDrawArrays(GL_POINTS, 0, count);
		glBindVertexArray(0);
	}

}