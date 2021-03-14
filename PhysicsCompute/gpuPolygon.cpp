#include "gpuPolygon.h"

namespace pc {

	void GPUPolygon::load(GLuint buffer, unsigned int offset, unsigned int size) {
		this->buffer = buffer;
		this->offset = offset;
		this->size = size;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ce::vec2f), (GLvoid*)0);
			glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	void pc::GPUPolygon::draw() const {
		glBindVertexArray(vao);
			glDrawArrays(GL_LINE_LOOP, offset, size);
		glBindVertexArray(0);
	}

}