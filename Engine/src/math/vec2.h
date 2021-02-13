#pragma once
#include <ostream>

namespace ce {

	template<typename T>
	struct vec2 {
		T x;
		T y;
		
		vec2() = default;
		vec2(const vec2& v) = default;
		explicit vec2(T scalar) :
			x(scalar),
			y(scalar)
		{ };
		vec2(T x, T y) :
			x(x),
			y(y)
		{ };

		template<typename U>
		vec2(const vec2<U>& v) {
			x = static_cast<T>(v.x);
			y = static_cast<T>(v.y);
		}

		vec2& operator=(const vec2& v) {
			x = v.x;
			y = v.y;

			return *this;
		};

		template<typename U>
		vec2& operator=(const vec2<U>& v) {
			x = static_cast<T>(v.x);
			y = static_cast<T>(v.y);

			return *this;
		};
		template<typename U>
		vec2& operator=(U scalar) {
			x = static_cast<T>(scalar);
			y = static_cast<T>(scalar);

			return *this;
		};
		template<typename U>
		vec2& operator+=(const vec2<U>& v) {
			x += static_cast<T>(v.x);
			y += static_cast<T>(v.y);

			return *this;
		};
		template<typename U>
		vec2& operator+=(U scalar) {
			x += static_cast<T>(scalar);
			y += static_cast<T>(scalar);

			return *this;
		};
		template<typename U>
		vec2& operator-=(const vec2<U>& v) {
			x -= static_cast<T>(v.x);
			y -= static_cast<T>(v.y);

			return *this;
		};
		template<typename U>
		vec2& operator-=(U scalar) {
			x -= static_cast<T>(scalar);
			y -= static_cast<T>(scalar);

			return *this;
		};
		template<typename U>
		vec2& operator*=(const vec2<U>& v) {
			y *= static_cast<T>(v.y);
			x *= static_cast<T>(v.x);

			return *this;
		};
		template<typename U>
		vec2& operator*=(U scalar) {
			y *= static_cast<T>(scalar);
			x *= static_cast<T>(scalar);

			return *this;
		};
		template<typename U>
		vec2& operator/=(const vec2<U>& v) {
			x /= static_cast<T>(v.x);
			y /= static_cast<T>(v.y);

			return *this;
		};
		template<typename U>
		vec2& operator/=(U scalar) {
			x /= static_cast<T>(scalar);
			y /= static_cast<T>(scalar);

			return *this;
		};

		vec2& operator++() {
			++x;
			++y;
			return *this;
		};
		vec2& operator--() {
			--x;
			--y;
			return *this;
		};
		vec2 operator++(int) {
			vec2<T> ret(*this);
			++x;
			++y;
			return ret;
		};
		vec2 operator--(int) {
			vec2<T> ret(*this);
			--x;
			--y;
			return ret;
		};

		template<typename U>
		friend vec2 operator+(vec2 a, const vec2<U>& b) {
			a.x += static_cast<U>(b.x);
			a.y += static_cast<U>(b.y);
			return a;
		};
		template<typename U>
		friend vec2 operator+(vec2 a, U scalar) {
			a.x += static_cast<U>(scalar);
			a.y += static_cast<U>(scalar);
			return a;
		};
		template<typename U>
		friend vec2 operator-(vec2 a, const vec2<U>& b) {
			a.x -= static_cast<U>(b.x);
			a.y -= static_cast<U>(b.y);
			return a;

		};
		template<typename U>
		friend vec2 operator-(vec2 a, U scalar) {
			a.x -= static_cast<U>(scalar);
			a.y -= static_cast<U>(scalar);
			return a;

		};
		template<typename U>
		friend vec2 operator*(vec2 a, const vec2<U>& b) {
			a.x *= static_cast<U>(b.x);
			a.y *= static_cast<U>(b.y);
			return a;
		};
		template<typename U>
		friend vec2 operator*(vec2 a, U scalar) {
			a.x *= static_cast<U>(scalar);
			a.y *= static_cast<U>(scalar);
			return a;
		};
		template<typename U>
		friend vec2 operator/(vec2 a, const vec2<U>& b) {
			a.x /= static_cast<U>(b.x);
			a.y /= static_cast<U>(b.y);
			return a;
		}
		template<typename U>
		friend vec2 operator/(vec2 a, U scalar) {
			a.x /= static_cast<U>(scalar);
			a.y /= static_cast<U>(scalar);
			return a;
		};

		vec2 operator+() {
			return *this;
		}

		vec2 operator-() {
			return vec2(-x, -y);
		}

		T& operator[](int i);
		const T& operator[](int i) const;

	};

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const vec2<T>& v);

	typedef vec2<int> vec2i;
	typedef vec2<float> vec2f;
	typedef vec2<double> vec2d;

}