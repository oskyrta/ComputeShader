#include "rigidbody.h"

namespace pc {

	void Rigidbody::load(const std::vector<ce::vec2f>& vertices, ce::vec2f centroid, float area) {
        this->centroid = centroid;

        e = 0.45f;
        float density = 1;
        moment_of_inertia = 0.0f;

        for (int i = 0; i < vertices.size(); ++i)
        {
            ce::vec2f p1 = vertices[i] - centroid;
            int next = (i + 1) % vertices.size();
            ce::vec2f p2 = vertices[next] - centroid;

            float cross = p1.cross(p2);

            float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
            float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
            moment_of_inertia += cross * (intx2 + inty2);
        }

        mass = area * density;
        inv_mass = mass ? 1.f / mass : 0;

        moment_of_inertia = (mass * std::abs(moment_of_inertia)) / (12 * area);
        inv_moment_of_inertia = moment_of_inertia ? 1.f / moment_of_inertia : 0;
	}

}