#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <render/renderTarget.h>
#include "transform.h"

namespace ce {

	struct Window {
		Window() {};
		Window(int width, int height, const char* name);

		void load(int width, int height, const char* name);

		bool shouldClose() { return glfwWindowShouldClose(window); }
		void pollEvents() { glfwPollEvents(); renderTarget.setView(transform.getTranslate()); }
		void swapBuffers() { glfwSwapBuffers(window); }

		void close() { glfwDestroyWindow(window); }
		void flush() { glFlush(); }

		RenderTarget* getRenderTarget() { return &renderTarget; }
		GLFWwindow* getGLFWwindow() { return window; }
			   
		Transform* getTransform() { return &transform; }
		const Transform* getTransform() const { return &transform; }

	private:
		GLFWwindow* window = nullptr;
		Transform transform;
		RenderTarget renderTarget;
	};

}
