#include "vec2.h"
#include <limits>

namespace ce {

	template<typename T>
	T& vec2<T>::operator[](int i) {
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
		switch (i) {
		default:
		case 0:
			return x;
		case 1:
			return y;
		}
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const vec2<T>& v)

	template<typename T>
	vec2<T> ce::vec2<T>::max_value() 

	template<typename T>
	vec2<T> ce::vec2<T>::min_value() 

	//template<typename T>
	//float ce::vec2<T>::len() const

	//template<typename T>
	//float ce::vec2<T>::sqrlen() const
}