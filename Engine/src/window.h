#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <render/renderTarget.h>

namespace ce {

	struct Window {
		Window() {};
		Window(int width, int height, const char* name);

		void load(int width, int height, const char* name);

		bool shouldClose() { return glfwWindowShouldClose(window); }
		void pollEvents() { glfwPollEvents(); }
		void swapBuffers() { glfwSwapBuffers(window); }
		void flush() { glFlush(); }

		RenderTarget* getRenderTarget() { return &renderTarget; }
		GLFWwindow* getGLFWwindow() { return window; }
			   
	private:
		GLFWwindow* window = nullptr;
		RenderTarget renderTarget;
	};

}
