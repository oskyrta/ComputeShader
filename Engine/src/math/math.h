#pragma once
#include <cstdint>

namespace ce::math {

	template<typename T>
	inline T sqr(T v) {
		return v * v;
	}

	inline float q_sqrt(float num) {
		uint32_t i;
		float x2, y;
		const float threehalves = 1.5f;

		x2 = num * 0.5f;
		y = num;
		memcpy(&i, &num, 4);
		i = 0x5f3759df - (i >> 1);
		memcpy(&y, &i, 4);
		y = y * (threehalves - (x2 * y * y));

		return y;
	}

	inline float to_radians(float degrees) {
		return degrees * 0.0174532925;
	}

}