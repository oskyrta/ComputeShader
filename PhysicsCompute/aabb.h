#pragma once
#include "math/vec2.h"

namespace pc {

	struct AABB {
		AABB() {}

		AABB(ce::vec2f bottom_left, ce::vec2f top_right) :
			bottom_left(bottom_left),
			top_right(top_right)
		{}

		ce::vec2f bottom_left;
		ce::vec2f top_right;
	};

}