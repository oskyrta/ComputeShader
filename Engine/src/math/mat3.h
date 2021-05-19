#pragma once
#include "vec2.h"
#include "vec3.h"
#include <cmath>

namespace ce {

	template<typename T>
	struct mat3 {
		T data[9];

		mat3() = default;
		mat3(const mat3& m) = default;

		mat3(T scalar) {
			data[0] = data[4] = data[8] = scalar;
			data[1] = data[2] = data[3] = data[5] = data[6] = data[7] = 0;
		}

		mat3 fill(T scalar) {
			data[0] = data[1] = data[2] = 
			data[3] = data[4] = data[5] = 
			data[6] = data[7] = data[8] = scalar;
		}

		friend mat3 operator+(mat3 a, const mat3& b) {
			a.data[0] += b.data[0]; a.data[1] += b.data[1]; a.data[2] += b.data[2];
			a.data[3] += b.data[3]; a.data[4] += b.data[4]; a.data[5] += b.data[5];
			a.data[6] += b.data[6]; a.data[7] += b.data[7]; a.data[8] += b.data[8];
		
			return a;
		}

		friend mat3 operator-(mat3 a, const mat3& b) {
			a.data[0] -= b.data[0]; a.data[1] -= b.data[1]; a.data[2] -= b.data[2];
			a.data[3] -= b.data[3]; a.data[4] -= b.data[4]; a.data[5] -= b.data[5];
			a.data[6] -= b.data[6]; a.data[7] -= b.data[7]; a.data[8] -= b.data[8];
		
			return a;
		}

		friend mat3 operator*(const mat3& a, const mat3& b) {
			mat3<T> ret;

			ret.data[0] = a.data[0] * b.data[0] + a.data[3] * b.data[1] + a.data[6] * b.data[2];
			ret.data[3] = a.data[0] * b.data[3] + a.data[3] * b.data[4] + a.data[6] * b.data[5];
			ret.data[6] = a.data[0] * b.data[6] + a.data[3] * b.data[7] + a.data[6] * b.data[8];

			ret.data[1] = a.data[1] * b.data[0] + a.data[4] * b.data[1] + a.data[7] * b.data[2];
			ret.data[4] = a.data[1] * b.data[3] + a.data[4] * b.data[4] + a.data[7] * b.data[5];
			ret.data[7] = a.data[1] * b.data[6] + a.data[4] * b.data[7] + a.data[7] * b.data[8];

			ret.data[2] = a.data[2] * b.data[0] + a.data[5] * b.data[1] + a.data[8] * b.data[2];
			ret.data[5] = a.data[2] * b.data[3] + a.data[5] * b.data[4] + a.data[8] * b.data[5];
			ret.data[8] = a.data[2] * b.data[6] + a.data[5] * b.data[7] + a.data[8] * b.data[8];
		
			return ret;
		}

		friend vec3<T> operator*(const mat3& m, const vec3<T>& v) {
			vec3<T> ret;

			ret.x = m.data[0] * v.x + m.data[3] * v.y + m.data[6] * v.z;
			ret.y = m.data[1] * v.x + m.data[4] * v.y + m.data[7] * v.z;
			ret.z = m.data[2] * v.x + m.data[5] * v.y + m.data[8] * v.z;

			return ret;
		}

		friend vec2<T> operator*(const mat3& m, const vec2<T>& v) {
			vec2<T> ret;

			ret.x = m.data[0] * v.x + m.data[3] * v.y + m.data[6];
			ret.y = m.data[1] * v.x + m.data[4] * v.y + m.data[7];

			return ret;
		}

		T* operator[](int i) {
			return data + (i * 3);
		}

		static mat3 ortho(T left, T right, T bottom, T top) {
			mat3<T> ret;

			ret.data[0] = 2 / (right - left);				ret.data[1] = 0;								ret.data[2] = 0;
			ret.data[3] = 0;								ret.data[4] = 2 / (top - bottom);				ret.data[5] = 0;
			ret.data[6] = -(right + left) / (right - left); ret.data[7] = -(top + bottom) / (top - bottom); ret.data[8] = 1;

			return ret;
		};
		static mat3 translate(mat3 m, vec2<T> offset) {
			mat3<T> t(1);

			t.data[6] = offset.x;
			t.data[7] = offset.y;

			return t * m;
		};
		static mat3 rotate(mat3 m, double radians) {
			mat3<T> t(1);
			
			double s = sin(radians);
			double c = cos(radians);
			
			// double s = sin( (degrees * 3.14159265) / 180);
			// double c = cos( (degrees * 3.14159265) / 180);

			t.data[0] = c; t.data[1] = s;
			t.data[3] = -s; t.data[4] = c;

			return t * m;
		};
		static mat3 scale(mat3 m, vec2<T> scale) {
			mat3<T> t(1);

			t.data[0] = scale.x;
			t.data[4] = scale.y;

			return t * m;
		};

	};

	typedef mat3<int> mat3i;
	typedef mat3<float> mat3f;
	typedef mat3<double> mat3d;


}