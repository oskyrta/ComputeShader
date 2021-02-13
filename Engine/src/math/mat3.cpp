#include "mat3.h"
#include <cmath>

#define PI 3.14159265

namespace ce {

	//template<typename T>
	//static constexpr mat3<T> mat3<T>::ortho(T left, T right, T bottom, T top) {
	//	mat3 ret;

	//	ret.data[0] = 2 / (right - left);				ret.data[1] = 0;								ret.data[2] = 0;
	//	ret.data[3] = 0;								ret.data[4] = 2 / (top - bottom);				ret.data[5] = 0;
	//	ret.data[6] = -(right + left) / (right - left); ret.data[7] = -(top + bottom) / (top - bottom); ret.data[8] = 1;

	//	return ret;
	//}

	//template<typename T>
	//static constexpr mat3<T> mat3<T>::translate(mat3 m, vec2<T> offset) {
	//	mat3<T> t(1);

	//	t.data[6] += offset.x;
	//	t.data[7] += offset.y;

	//	return m * t;
	//}

	//// 0, 3, 6
	//// 1, 4, 7
	//// 2, 5, 8

	//template<typename T>
	//constexpr mat3<T> mat3<T>::rotate(mat3 m, double degrees) {
	//	mat3<T> t(1);
	//	double s = sin(degrees * PI / 180);
	//	double c = cos(degrees * PI / 180);

	//	t.data[0] = c; t.data[1] = s;
	//	t.data[3] = -s; t.data[4] = c;

	//	return m * t;
	//}

	//template<typename T>
	//constexpr mat3<T> mat3<T>::scale(mat3 m, vec2<T> scale) {
	//	mat3<T> t(1);

	//	t.data[0] *= scale.x;
	//	t.data[4] *= scale.y;

	//	return m * t;
	//}

}