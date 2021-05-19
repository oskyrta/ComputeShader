#pragma once
#include "GL/glew.h"
#include <math/mat3.h>

namespace ce {
	enum class ShaderType : unsigned int {
		Vertex		= GL_VERTEX_SHADER,
		Fragment	= GL_FRAGMENT_SHADER,
		Compute		= GL_COMPUTE_SHADER
	};

	struct Program {
#ifdef _DEBUG
		inline static bool logging = true;
#else
		inline static bool logging = false;
#endif
		
		GLint getId() { return program; }

		bool attachShaderFromFile(const char* path, ShaderType type);
		bool attachShader(const char* source, ShaderType type);
		bool link();
		void bind() const { glUseProgram(program); };
		void unbind() const { glUseProgram(0); };

		void setInt(const char* name, GLint v1) const { bind();  glUniform1i(glGetUniformLocation(program, name), v1); };
		void setUint(const char* name, GLuint v1) const { bind();  glUniform1ui(glGetUniformLocation(program, name), v1); };
		void setFloat(const char* name, GLfloat v1) const { bind();  glUniform1f(glGetUniformLocation(program, name), v1); };
		void setVector(const char* name, GLfloat v1, GLfloat v2) const { bind(); glUniform2f(glGetUniformLocation(program, name), v1, v2); };
		void setVector(const char* name, GLfloat v1, GLfloat v2, GLfloat v3) const { bind(); glUniform3f(glGetUniformLocation(program, name), v1, v2, v3); };
		void setVector(const char* name, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4) const { bind(); glUniform4f(glGetUniformLocation(program, name), v1, v2, v3, v4); };

		//void setMatrix(const char* name, const glm::mat4& mat) const { use(); glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(mat)); }
		void setMatrix(const char* name, const mat3f& mat) const { bind(); glUniformMatrix3fv(glGetUniformLocation(program, name), 1, GL_FALSE, mat.data); }

	private:
		GLint program = 0;
	};

}