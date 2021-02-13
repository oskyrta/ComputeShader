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
		inline static bool logging = false;

		bool attachShaderFromFile(const char* path, ShaderType type);
		bool attachShader(const char* source, ShaderType type);
		bool link();
		void use() const { glUseProgram(program); };

		void setFloat(const char* name, GLfloat v1) const { use();  glUniform1f(glGetUniformLocation(program, name), v1); };
		void setVector(const char* name, GLfloat v1, GLfloat v2) const { use(); glUniform2f(glGetUniformLocation(program, name), v1, v2); };
		void setVector(const char* name, GLfloat v1, GLfloat v2, GLfloat v3) const { use(); glUniform3f(glGetUniformLocation(program, name), v1, v2, v3); };
		void setVector(const char* name, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4) const { use(); glUniform4f(glGetUniformLocation(program, name), v1, v2, v3, v4); };

		//void setMatrix(const char* name, const glm::mat4& mat) const { use(); glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(mat)); }
		void setMatrix(const char* name, const mat3f& mat) const { use(); glUniformMatrix3fv(glGetUniformLocation(program, name), 1, GL_FALSE, mat.data); }

	private:
		GLint program = 0;
	};

}