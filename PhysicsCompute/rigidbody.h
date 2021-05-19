#pragma once
#include <vector>
#include "transform.h"
#include "math/vec2.h"

namespace pc {

	class Rigidbody {
	public:
		Rigidbody(ce::Transform* transform) : transform(transform) {}

		void load(const std::vector<ce::vec2f>& vertices, ce::vec2f centroid, float area);
		void makeStatic() { mass = inv_mass = moment_of_inertia = inv_moment_of_inertia = 0; }

		ce::Transform* transform;

		float e;
		float mass;
		float inv_mass;
		float moment_of_inertia;
		float inv_moment_of_inertia;

		ce::vec2f centroid;

		ce::vec2f velocity;
		ce::vec2f force;

		float angular_velocity;
		float torque;
	};

}