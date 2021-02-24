#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "core.h"

namespace ce {

	Window* current_window = nullptr;
	
	Window::Window(int width, int height, const char* name) {
		load(width, height, name);
	}

	void Window::load(int width, int height, const char* name) {
		static Core core;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 

#ifndef _DEBUG
		glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
#endif // !_DEBUG

		window = glfwCreateWindow(width, height, name, nullptr, nullptr);
		glfwMakeContextCurrent(window);
				
		glewExperimental = true;
		glewInit();

		//wglSwapIntervalEXT(1);
		glfwSwapInterval(1);

		int fbWidth, fbHeight;
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		glViewport(0, 0, fbWidth, fbHeight);

		renderTarget.load(width, height);
	}

}