#pragma once
#include <ostream>

namespace ce {

	template<typename T>
	struct vec3 {
		T x;
		T y;
		T z;
		
		vec3() = default;
		vec3(const vec3& v) = default;
		explicit vec3(T scalar) :
			x(scalar),
			y(scalar),
			z(scalar)
		{ };
		vec3(T x, T y, T z) :
			x(x),
			y(y),
			z(z)
		{ };

		vec3& operator=(const vec3& v) {
			x = v.x;
			y = v.y;
			z = v.z;

			return *this;
		};

		template<typename U>
		vec3& operator=(const vec3<U>& v) {
			x = static_cast<T>(v.x);
			y = static_cast<T>(v.y);
			z = static_cast<T>(v.z);

			return *this;
		};
		template<typename U>
		vec3& operator=(U scalar) {
			x = static_cast<T>(scalar);
			y = static_cast<T>(scalar);
			z = static_cast<T>(scalar);

			return *this;
		};
		template<typename U>
		vec3& operator+=(const vec3<U>& v) {
			x += static_cast<T>(v.x);
			y += static_cast<T>(v.y);
			z += static_cast<T>(v.z);

			return *this;
		};
		template<typename U>
		vec3& operator+=(U scalar) {
			x += static_cast<T>(scalar);
			y += static_cast<T>(scalar);
			z += static_cast<T>(scalar);

			return *this;
		};
		template<typename U>
		vec3& operator-=(const vec3<U>& v) {
			x -= static_cast<T>(v.x);
			y -= static_cast<T>(v.y);
			z -= static_cast<T>(v.z);

			return *this;
		};
		template<typename U>
		vec3& operator-=(U scalar) {
			x -= static_cast<T>(scalar);
			y -= static_cast<T>(scalar);
			z -= static_cast<T>(scalar);

			return *this;
		};
		template<typename U>
		vec3& operator*=(const vec3<U>& v) {
			x *= static_cast<T>(v.x);
			y *= static_cast<T>(v.y);
			z *= static_cast<T>(v.z);

			return *this;
		};
		template<typename U>
		vec3& operator*=(U scalar) {
			x *= static_cast<T>(scalar);
			y *= static_cast<T>(scalar);
			z *= static_cast<T>(scalar);

			return *this;
		};
		template<typename U>
		vec3& operator/=(const vec3<U>& v) {
			x /= static_cast<T>(v.x);
			y /= static_cast<T>(v.y);
			z /= static_cast<T>(v.z);

			return *this;
		};
		template<typename U>
		vec3& operator/=(U scalar) {
			x /= static_cast<T>(scalar);
			y /= static_cast<T>(scalar);
			z /= static_cast<T>(scalar);

			return *this;
		};

		vec3& operator++() {
			++x;
			++y;
			++z;
			return *this;
		};
		vec3& operator--() {
			--x;
			--y;
			--z;
			return *this;
		};
		vec3 operator++(int) {
			vec3<T> ret(*this);
			++x;
			++y;
			++z;
			return ret;
		};
		vec3 operator--(int) {
			vec3<T> ret(*this);
			--x;
			--y;
			--z;
			return ret;
		};

		template<typename U>
		friend vec3 operator+(vec3 a, const vec3<U>& b) {
			a.x += static_cast<U>(b.x);
			a.y += static_cast<U>(b.y);
			a.z += static_cast<U>(b.z);
			return a;
		};
		template<typename U>
		friend vec3 operator+(vec3 a, U scalar) {
			a.x += static_cast<U>(scalar);
			a.y += static_cast<U>(scalar);
			a.z += static_cast<U>(scalar);
			return a;
		};
		template<typename U>
		friend vec3& operator-(vec3 a, const vec3<U>& b) {
			a.x -= static_cast<U>(b.x);
			a.y -= static_cast<U>(b.y);
			a.z -= static_cast<U>(b.z);
			return a;

		};
		template<typename U>
		friend vec3& operator-(vec3 a, U scalar) {
			a.x -= static_cast<U>(scalar);
			a.y -= static_cast<U>(scalar);
			a.z -= static_cast<U>(scalar);
			return a;

		};
		template<typename U>
		friend vec3& operator*(vec3 a, const vec3<U>& b) {
			a.x *= static_cast<U>(b.x);
			a.y *= static_cast<U>(b.y);
			a.z *= static_cast<U>(b.z);
			return a;
		};
		template<typename U>
		friend vec3& operator*(vec3 a, U scalar) {
			a.x *= static_cast<U>(scalar);
			a.y *= static_cast<U>(scalar);
			a.z *= static_cast<U>(scalar);
			return a;
		};
		template<typename U>
		friend vec3& operator/(vec3 a, const vec3<U>& b) {
			a.x /= static_cast<U>(b.x);
			a.y /= static_cast<U>(b.y);
			a.z /= static_cast<U>(b.z);
			return a;
		}
		template<typename U>
		friend vec3& operator/(vec3 a, U scalar) {
			a.x /= static_cast<U>(scalar);
			a.y /= static_cast<U>(scalar);
			a.z /= static_cast<U>(scalar);
			return a;
		};

		vec3 operator+(const vec3& v) {
			return *this;
		}

		vec3 operator-(const vec3& v) {
			return vec3(-v.x, -v.y, -v.z);
		}

		T& operator[](int i);
		const T& operator[](int i) const;

	};

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const vec3<T>& v);

}