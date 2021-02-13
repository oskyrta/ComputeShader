#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"
#include <iostream>
#include <stb/stb_image.h>

namespace ce {
	
	void Texture::init() {
		if (texture) terminate();

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void Texture::load(int width, int height, GLuint internal_format, GLuint format) {
		init();

		this->width = width;
		this->height = height;

		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::load(const char* path) {
		int nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
		
		if (data) {
			init();

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glBindTexture(GL_TEXTURE_2D, 0);

			stbi_image_free(data);
		}
		else if (logging) {
			std::cout << path << std::endl << "ERROR::TEXTURE::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
	}

}
