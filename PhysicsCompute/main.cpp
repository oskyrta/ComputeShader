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


const int screen_width = 720;
const int screen_height = 512;

bool use_gpu = false;
int objects_count = 25;
int points_count = 0;

bool lbutton_pressed = false;

ce::Window window;
ce::Program line_shader;
std::vector<pc::Polygon> polygons;


pc::Polygon createPolygon() {
	pc::Polygon polygon;

	std::vector<ce::vec2f> points;
	for (int i = 0; i < 20; ++i) {
		points.push_back(ce::vec2{ rand() % 40 - 20, rand() % 40 - 20 });
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

		polygon.transform.setPosition(ce::vec2{ rand() % screen_width, rand() % screen_height });

		polygon.getRigidbody()->velocity = ce::vec2{ rand() % 100 - 50, rand() % 100 - 50 };
		polygon.getRigidbody()->force = { 0, -250 };

		polygons.push_back(polygon);
	}

	pc::Polygon left_mill;
	left_mill.load({ {0, 0}, {0, 250}, {10, 250}, {10, 0} });
	left_mill.transform.setPosition({ 150, 150 });
	left_mill.setProgram(&line_shader);
	left_mill.getRigidbody()->angular_velocity = 3;
	left_mill.getRigidbody()->makeStatic();

	pc::Polygon right_mill;
	right_mill.load({ {0, 0}, {0, 250}, {10, 250}, {10, 0} });
	right_mill.transform.setPosition({ 570, 150 });
	right_mill.setProgram(&line_shader);
	right_mill.transform.setRotation(ce::math::to_radians(90));
	right_mill.getRigidbody()->angular_velocity = -3;
	right_mill.getRigidbody()->makeStatic();
		
	polygons.push_back(left_mill);
	polygons.push_back(right_mill);

	// Boundaries
	pc::Polygon floor;
	floor.load({ {0, 0}, {0, 10}, {screen_width, 10}, {screen_width, 0} });
	floor.transform.setPosition({ screen_width / 2, 5 });
	floor.getRigidbody()->makeStatic();
	floor.setProgram(&line_shader);

	pc::Polygon wall1;
	wall1.load({ {0, 0}, {0, screen_height}, {10, screen_height}, {10, 0} });
	wall1.transform.setPosition({ screen_width - 5, screen_height / 2 });
	wall1.getRigidbody()->makeStatic();
	wall1.setProgram(&line_shader);

	pc::Polygon wall2;
	wall2.load({ {0, 0}, {0, screen_height}, {10, screen_height}, {10, 0} });
	wall2.transform.setPosition({ 5, screen_height / 2 });
	wall2.getRigidbody()->makeStatic();
	wall2.setProgram(&line_shader);

	polygons.push_back(floor);
	polygons.push_back(wall1);
	polygons.push_back(wall2);
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

	window.load(screen_width, screen_height, "Physics computation");

	window.getTransform()->setScale({ 0.7f, 0.7f });
	window.getTransform()->setPosition({ 108, 76 });

	pc::gpu::startup();

	line_shader.attachShaderFromFile("line.vert", ce::ShaderType::Vertex);
	line_shader.attachShaderFromFile("line.frag", ce::ShaderType::Fragment);
	line_shader.link();

	srand(time(0));

	initScene();
	pc::gpu::init(polygons);

	const float physics_deltatime = 1.f / 300;

	using namespace std::chrono;
	auto last = high_resolution_clock::now();
	int passed_frames = 0;
	double time_past = 0;

	while (!window.shouldClose()) {
		window.pollEvents();
		glClear(GL_COLOR_BUFFER_BIT);

		auto current_time = high_resolution_clock::now();
		double deltatime = duration<double>(current_time - last).count();
		last = current_time;

		countFPS(deltatime);

		time_past += deltatime;

		time_past = std::min(time_past, 1.);
		while (time_past > physics_deltatime) {
			auto start = high_resolution_clock::now();
			
			if (use_gpu) {
				pc::gpu::updatePhysics(physics_deltatime);
			}
			else {
				pc::updatePhysics(polygons, physics_deltatime);
			}

			time_past -= physics_deltatime;
		}

		if (use_gpu) {
			pc::gpu::draw(window.getRenderTarget());
		}
		else {
			for (const auto& polygon : polygons) {
				window.getRenderTarget()->draw(&polygon);
			}
		}

		window.swapBuffers();
	}

	return 0;
}