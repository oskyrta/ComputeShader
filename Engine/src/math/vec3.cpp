#include "vec3.h"
#include <iostream>
#include <assert.h>

namespace ce {

	template<typename T>
	T& vec3<T>::operator[](int i) {
		assert(i >= 0 && i < 3);
		switch (i) {
		default:
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
	}

	template<typename T>
	const T& vec3<T>::operator[](int i) const {
		assert(i >= 0 && i < 3);
		switch (i) {
		default:
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const vec3<T>& v) {
		os << v.x << ", " << v.y << ", " << v.z;
	}

}