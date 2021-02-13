#include "core.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace ce {
	Core::Core() {
		glfwInit();
	}

	Core::~Core() {
		glfwTerminate();
	}
}