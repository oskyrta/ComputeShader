#include <iostream>
#include "render/program.h"
#include "window.h"
#include "math/vec2.h"
#include <cmath>
#include <ctime>
#include <algorithm>
#include "polygon.h"
#include "aabb.h"
#include <chrono>
#include "quickhull.h"
#include "math/math.h"
#include "physicsController.h"
#include "opengl/ssbo.h"
#include "gpuPolygon.h"
#include "gpuPhysicsController.h"
#include <numeric>
#include <fstream>
#include <string>

typedef unsigned int uint;


const int width = 512;
const int height = 512;

bool use_gpu = true;
int objects_count = 200;
int points_count = 0;

bool lbutton_pressed = false;

ce::Window window;
ce::Program line_shader;
std::vector<pc::Polygon> polygons;

pc::Polygon createPolygon() {
	pc::Polygon polygon;

	std::vector<ce::vec2f> points;
	for (int i = 0; i < 20; ++i) {
		points.push_back(ce::vec2{ rand() % 20 - 10, rand() % 20 - 10 });
	}

	auto hull = quickHull(points);

	polygon.load(hull);
	polygon.setProgram(&line_shader);

	return polygon;
}

void initScene() {
	polygons.clear();

	for (int i = 0; i < objects_count; ++i) {
		pc::Polygon polygon = createPolygon();

		points_count += polygon.getVertices().size();

		polygon.transform.setPosition(ce::vec2{ rand() % 512, rand() % 512 });

		polygon.getRigidbody()->velocity = ce::vec2{ rand() % 100 - 50, rand() % 100 - 50 };
		polygon.getRigidbody()->force = { 0, -250 };

		polygons.push_back(polygon);
	}

	pc::Polygon floor;
	floor.load({ {0, 0}, {0, 10}, {512, 10}, {512, 0} });
	floor.transform.setPosition({ 256, 0 });
	floor.getRigidbody()->makeStatic();
	floor.setProgram(&line_shader);

	pc::Polygon wall1;
	wall1.load({ {0, 0}, {0, 512}, {10, 512}, {10, 0} });
	wall1.transform.setPosition({ 507, 256 });
	wall1.getRigidbody()->makeStatic();
	wall1.setProgram(&line_shader);

	pc::Polygon wall2;
	wall2.load({ {0, 0}, {0, 512}, {10, 512}, {10, 0} });
	wall2.transform.setPosition({ 5, 256 });
	wall2.getRigidbody()->makeStatic();
	wall2.setProgram(&line_shader);

	polygons.push_back(floor);
	polygons.push_back(wall1);
	polygons.push_back(wall2);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	//if (button == GLFW_MOUSE_BUTTON_LEFT) {
	//	if (action == GLFW_PRESS) {
	//		init();
	//		lbutton_pressed = true;
	//	}
	//	else if (action == GLFW_RELEASE) {
	//		lbutton_pressed = false;
	//	}
	//}
}

void countFPS(double deltatime) {
	static int passed_frames = 0;
	static double time_past = 0;

	passed_frames++;
	time_past += deltatime;

	while (time_past > 1.) {
		std::cout << "FPS: " << passed_frames << std::endl;
		passed_frames = 0;
		time_past -= 1;
	}
}

int main(int argc, char** argv) {
	if (argc > 1) {
		use_gpu = std::string("gpu").compare(argv[1]) == 0;
		objects_count = std::stoi(argv[2]);
	}

	window.load(width, height, "Physics compute");
	glfwSetMouseButtonCallback(window.getGLFWwindow(), mouse_button_callback);
	//glClearColor(1, 1, 1, 1);

	pc::gpu::startup();

	line_shader.attachShaderFromFile("line.vert", ce::ShaderType::Vertex);
	line_shader.attachShaderFromFile("line.frag", ce::ShaderType::Fragment);
	line_shader.link();

	srand(time(0));


	//pc::Polygon polygon1;
	//polygon1.load({ {200, 150}, {200, 250}, {205, 250}, {205, 150} });
	//polygon1.getTransform()->setPosition({ 200, 220 });
	//polygon1.setProgram(&line_shader);
	//polygon1.getRigidbody()->angular_velocity = 3;
	//polygon1.getRigidbody()->velocity = { 50, 0 };
	////polygon1.getRigidbody()->force = { 0, -150 };
	//
	//pc::Polygon polygon2;
	//polygon2.load({ {200, 240}, {210, 250}, {230, 240}, {220, 220} });
	//polygon2.getTransform()->setPosition({ 250, 200 });
	//polygon2.setProgram(&line_shader);
	//
	//polygons.push_back(polygon1);
	//polygons.push_back(polygon2);

	initScene();
	pc::gpu::init(polygons);

	const float physics_deltatime = 1.f / 500;

	auto last = std::chrono::high_resolution_clock::now();
	int passed_frames = 0;
	double time_past = 0;

	std::vector<double> update_times;

	int i = 0;
	double time = 0, total_time = 0;
	while (!window.shouldClose()) {
		window.pollEvents();
		glClear(GL_COLOR_BUFFER_BIT);

		auto current_time = std::chrono::high_resolution_clock::now();
		double deltatime = std::chrono::duration<double>(current_time - last).count();
		last = current_time;

		countFPS(deltatime);

		time_past += deltatime;

		time_past = std::min(time_past, 1.);
		while (time_past > physics_deltatime) {
			auto start = std::chrono::high_resolution_clock::now();
			
			if (use_gpu) {
				pc::gpu::updatePhysics(physics_deltatime);
			}
			else {
				pc::updatePhysics(polygons, physics_deltatime);
			}

			time_past -= physics_deltatime;

			double time_elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start).count();

			update_times.push_back(time_elapsed);
			//std::cout << time << std::endl;
			total_time = time = 0;
		}

		

		if (update_times.size() > 5000) {
			window.close();
			break;
		}


		if (use_gpu) {
			//pc::gpu::draw(window.getRenderTarget());
		}
		else {
			for (const auto& polygon : polygons) {
				window.getRenderTarget()->draw(&polygon);
			}
		}

		window.swapBuffers();
	}

	int size = update_times.size();
	std::sort(update_times.begin(), update_times.end());

	total_time = std::accumulate(update_times.begin() + size * 0., update_times.end() - size * 0., 0.);

	std::cout << "Total time: " << total_time << std::endl;
	std::cout << "Average time: " << total_time / size << std::endl;

	//std::ofstream logfile;
	//logfile.open("nvidia_log.json", std::ios_base::app);
	//logfile << "{" << std::endl;
	//logfile << "\t" << R"("type": ")" << (use_gpu ? "gpu" : "cpu") << R"(",)" << std::endl;
	//logfile << "\t" << R"("avg_time": ")" << total_time / (size ) << R"(",)" << std::endl;
	//logfile << "\t" << R"("updates_count": ")" << update_times.size() << R"(",)" << std::endl;
	//logfile << "\t" << R"("points_count": ")" << points_count << R"(",)" << std::endl;
	//logfile << "\t" << R"("objects_count": ")" << objects_count << R"(")" << std::endl;
	//logfile << "}," << std::endl;


	return 0;
}