#include <algorithm>
#include "quickhull.h"

void findHull(std::vector<ce::vec2f>& hull, const std::vector<ce::vec2f>& points, ce::vec2f left, ce::vec2f right) {
	if (points.size() == 0) return;

	if (points.size() == 1) {
		hull.push_back(points[0]);
		return;
	}

	float dist = 0;
	ce::vec2f farthest{};
	ce::vec2f perp = (left - right).perpendicular();

	for (int i = 0; i < points.size(); ++i) {
		float dot = points[i].dot(perp);
		if (dot > dist) {
			dist = dot;
			farthest = points[i];
		}
	}

	ce::vec2f rf = farthest - right;
	ce::vec2f fl = left - farthest;
	std::vector<ce::vec2f> a;
	std::vector<ce::vec2f> b;

	for (int i = 0; i < points.size(); ++i) {
		if (fl.clockwise(points[i] - farthest)) {
			a.push_back(points[i]);
		}

		if (rf.clockwise(points[i] - right)) {
			b.push_back(points[i]);
		}
	}

	findHull(hull, a, left, farthest);
	hull.push_back(farthest);
	findHull(hull, b, farthest, right);
}

std::vector<ce::vec2f> quickHull(std::vector<ce::vec2f>& points) {
	std::vector<ce::vec2f> hull;

	std::sort(points.begin(), points.end());

	ce::vec2f left = points[0];
	ce::vec2f right = points.back();

	ce::vec2f line = right - left;
	std::vector<ce::vec2f> top;
	std::vector<ce::vec2f> bottom;

	for (int i = 1; i < points.size() - 1; ++i) {
		float cross = points[i].cross(line);

		if (cross > 0) {
			bottom.push_back(points[i]);
		}
		else if (cross < 0) {
			top.push_back(points[i]);
		}
	}

	hull.push_back(left);
	findHull(hull, top, left, right);
	hull.push_back(right);
	findHull(hull, bottom, right, left);

	return hull;
}