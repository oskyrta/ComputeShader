#pragma once
#include "GL/glew.h"

namespace ce::gl {

	struct SSBO {
		template<typename T>
		SSBO& create(GLsizeiptr size) {
			return create(size * sizeof(T));
		}

		SSBO& create(GLsizeiptr size) {
			if (ssbo == 0) {
				glGenBuffers(1, &ssbo);
			}

			buffer_size = size;
			bind();
			glBufferData(GL_SHADER_STORAGE_BUFFER, size, (void*)0, GL_DYNAMIC_DRAW);
			
			return *this;
		}

		SSBO& remove() {
			glDeleteBuffers(1, &ssbo); 
		}
		
		SSBO& bind() {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

			return *this;
		}
		
		SSBO& unbind() {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			return *this;
		}
		
		SSBO& bindBase(GLuint base) {
			bind(); 
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, base, ssbo);

			return *this;
		}

		template<typename T>
		SSBO& setData(const void* data, GLsizeiptr size, GLintptr offset = 0) {
			return setData(data, size * sizeof(T), offset * sizeof(T));
		}

		SSBO& setData(const void* data, GLsizeiptr size, GLintptr offset = 0) {
			bind(); 
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);

			return *this;
		}

		template<typename T>
		SSBO& getData(void* data, GLsizeiptr size, GLintptr offset = 0) {
			return getData(data, size * sizeof(T), offset * sizeof(T));
		}

		SSBO& getData(void* data, GLsizeiptr size, GLintptr offset = 0) {
			bind();
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);

			return *this;
		}

		template<typename T>
		std::vector<T> copyData() {
			std::vector<T> result;

			T* v = mapBuffer<T>();
			result.insert(result.end(), v, v + size<T>());
			unmapBuffer();

			return result;
		}

		template<typename T>
		T* mapBuffer(GLenum access = GL_READ_ONLY) {
			bind();
			return static_cast<T*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, access));
		}

		bool unmapBuffer() {
			bind();
			return glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		}

		GLsizeiptr size() {
			return buffer_size;
		}

		template<typename T>
		GLsizeiptr size() {
			return buffer_size / sizeof(T);
		}

		GLuint getBufferId() {
			return ssbo;
		}

	private:
		GLuint ssbo = 0;
		GLsizeiptr buffer_size = 0;
	};

}