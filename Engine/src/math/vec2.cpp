#include "vec2.h"
#include <assert.h>

namespace ce {

	template<typename T>
	T& vec2<T>::operator[](int i) {
		assert(i >= 0 && i < 2);
		switch (i) {
		default:
		case 0:
			return x;
		case 1:
			return y;
		}
	}

	template<typename T>
	const T& vec2<T>::operator[](int i) const {
		assert(i >= 0 && i < 2);
		switch (i) {
		default:
		case 0:
			return x;
		case 1:
			return y;
		}
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const vec2<T>& v) {
		os << v.x << ", " << v.y;
	}

}