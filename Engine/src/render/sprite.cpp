#include "sprite.h"
#include "texture.h"

namespace ce {

	void Sprite::load(vec2f size, const Texture* texture) {
		this->size = size;
		this->texture = texture;
		
		vec2f data[8] = {
			{     0,      0}, {0, 0},
			{	  0, size.y}, {0, 1},
			{size.x,      0}, {1, 0},
			{size.x, size.y}, {1, 1}
		};

		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &vao);

		glBindVertexArray(vao);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)(2 * sizeof(float)) );
			glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}

	void Sprite::draw() const {
		texture->bind();

		program->setMatrix("model", transform.getTranslate());
		program->setFloat("depth", transform.getDepth() / 1000);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

}