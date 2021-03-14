#include <iostream>
#include "render/program.h"
#include "render/texture.h"
#include "render/renderTexture.h"
#include "render/sprite.h"
#include "window.h"
#include <windows.h>
#include <cmath>

using namespace ce;

Window window;
ce::Program shader;

namespace mandelbrot {

	ce::Program ray_shader;
	ce::Texture texture;
	GLuint vbo, vao, ebo;

	bool lbutton_pressed = false;
	vec2d last_mouse_pos;
	vec2f pos = vec2f(-2, 0);
	double scale = 1;
	float size = 1;
	bool updated = true;

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		scale += yoffset;
		size = std::powf(1.1, scale);
		updated = true;
	}
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
		if (lbutton_pressed) pos += (last_mouse_pos - vec2d(xpos, 512 - ypos)) * (size);
		last_mouse_pos = vec2d(xpos, 512 - ypos);
		updated = true;
	}
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (action == GLFW_PRESS) lbutton_pressed = true;
			else if (action == GLFW_RELEASE) lbutton_pressed = false;
		}
	}

	void compute() {

		window.load(512, 512, "test");

		glfwGetCursorPos(window.getGLFWwindow(), &last_mouse_pos.x, &last_mouse_pos.y);
		glfwSetScrollCallback(window.getGLFWwindow(), scroll_callback);
		glfwSetCursorPosCallback(window.getGLFWwindow(), cursor_position_callback);
		glfwSetMouseButtonCallback(window.getGLFWwindow(), mouse_button_callback);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		Program::logging = Texture::logging = RenderTarget::logging = true;

		shader.attachShaderFromFile("simple.vert", ShaderType::Vertex);
		shader.attachShaderFromFile("simple.frag", ShaderType::Fragment);
		shader.link();

		ray_shader.attachShaderFromFile("mandelbrot.comp", ShaderType::Compute);
		ray_shader.link();

		ce::Texture ray_texture;
		ray_texture.load(512, 512, GL_RGBA32F);
		glBindImageTexture(0, ray_texture.getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		Sprite sprite;
		sprite.load({ 512, 512 }, &ray_texture);
		sprite.setProgram(&shader);

		while (!window.shouldClose()) {
			window.pollEvents();

			glClear(GL_COLOR_BUFFER_BIT);

			if (updated) {
				ray_shader.bind();
				ray_shader.setVector("size", size, size);
				ray_shader.setVector("pos", pos.x / 512, pos.y / 512);
				glDispatchCompute(32, 32, 1);
				glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
				updated = false;
			}

			window.getRenderTarget()->draw(&sprite);

			window.swapBuffers();
		}
	}

}

int main() {
	mandelbrot::compute();
	return 0;
}