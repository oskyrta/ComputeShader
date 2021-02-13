#include "program.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace ce {

	const char* shaderNameByType(ShaderType type) {
		switch (type) {
			case ce::ShaderType::Vertex:	return "VERTEX";
			case ce::ShaderType::Fragment:	return "FRAGMENT";
			case ce::ShaderType::Compute:	return "COMPUTE";
			default: return "";
		}
	}

	bool Program::attachShaderFromFile(const char* path, ShaderType type) {
		std::ifstream source_file;
		std::string source;

		source_file.exceptions(std::ifstream::failbit);
		try {
			source_file.open(path);

			std::stringstream shaderStream;
			shaderStream << source_file.rdbuf();

			source_file.close();

			source = shaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << path << "\n" << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		return attachShader(source.c_str(), type);
	}

	bool Program::attachShader(const char* source, ShaderType type) {
		GLint shader = glCreateShader((GLenum)type);
		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);

		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (logging && !success) {
			GLchar info[512];
			glGetShaderInfoLog(shader, 512, nullptr, info);
			std::cout << "ERROR::SHADER::" << shaderNameByType(type) << "::COMPILATION_FAILED\n" << info << std::endl;
		}

		if (!success) return false;

		if (!program) program = glCreateProgram();

		glAttachShader(program, shader);
		glDeleteShader(shader);

		return true;
	}

	bool Program::link() {
		glLinkProgram(program);

		GLint success;
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		if (logging && !success) {
			GLchar info[512];
			glGetProgramInfoLog(program, 512, nullptr, info);
			std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << info << std::endl;
		}

		return success;
	}

}
