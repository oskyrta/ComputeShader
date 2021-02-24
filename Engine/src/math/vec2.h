#pragma once
#include <ostream>
#include <limits>

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

#pragma region operators

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
		friend vec2 operator+(const vec2& a, const vec2<U>& b) {
			return vec2{
				a.x + static_cast<T>(b.x),
				a.y + static_cast<T>(b.y)
			};
		};

		template<typename U>
		friend vec2 operator+(const vec2& a, U scalar) {
			return vec2{
				a.x + static_cast<T>(scalar),
				a.y + static_cast<T>(scalar)
			};
		};

		template<typename U>
		friend vec2 operator-(const vec2& a, const vec2<U>& b) {
			return vec2{
				a.x - static_cast<T>(b.x),
				a.y - static_cast<T>(b.y)
			};
		};

		template<typename U>
		friend vec2 operator-(const vec2& a, U scalar) {
			return vec2{
				a.x - static_cast<T>(scalar),
				a.y - static_cast<T>(scalar)
			};
		};

		template<typename U>
		friend vec2 operator*(const vec2& a, const vec2<U>& b) {
			return vec2{
				a.x * static_cast<T>(b.x),
				a.y * static_cast<T>(b.y)
			};
		};

		template<typename U>
		friend vec2 operator*(const vec2& a, U scalar) {
			return vec2{
				a.x * static_cast<T>(scalar),
				a.y * static_cast<T>(scalar)
			};
		};

		template<typename U>
		friend vec2 operator/(const vec2& a, const vec2<U>& b) {
			return vec2{
				a.x / static_cast<T>(b.x),
				a.y / static_cast<T>(b.y)
			};
		};

		template<typename U>
		friend vec2 operator/(vec2 a, U scalar) {
			return vec2{
				a.x / static_cast<T>(scalar),
				a.y / static_cast<T>(scalar)
			};
		};

		template<typename U>
		friend bool operator<(const vec2& a, const vec2<U>& b) {
			return a.x < static_cast<T>(b.x) || (a.x == static_cast<T>(b.x) && a.y < static_cast<T>(b.y));
		}

		template<typename U>
		friend bool operator<=(const vec2& a, const vec2<U>& b) {
			return a.x <= static_cast<T>(b.x) || (a.x == static_cast<T>(b.x) && a.y <= static_cast<T>(b.y));
		}

		template<typename U>
		friend bool operator>(const vec2& a, const vec2<U>& b) {
			return !a <= b;
		}

		template<typename U>
		friend bool operator>=(const vec2& a, const vec2<U>& b) {
			return !a < b;
		}


		vec2 operator+() const {
			return *this;
		}

		vec2 operator-() const {
			return vec2(-x, -y);
		}

		T& operator[](int i) {
			switch (i) {
			default:
			case 0:
				return x;
			case 1:
				return y;
			}
		}

		const T& operator[](int i) const {
			switch (i) {
			default:
			case 0:
				return x;
			case 1:
				return y;
			}
		}

#pragma endregion

#pragma region functions

		float len() const {
			return std::sqrtf(x * x + y * y);
		}

		float sqrlen() const {
			return x * x + y * y;
		}

		vec2<float> normalized() const {
			float length = len();

			if (length == 0) {
				return *this;
			}

			return vec2<float>{
				x / length,
				y / length
			};
		}

#pragma endregion

#pragma region static

		static vec2 max_value() {
			return vec2<T>{  (std::numeric_limits<T>::max)(), (std::numeric_limits<T>::max)() };
		}

		static vec2 min_value() {
			return vec2<T>{ (std::numeric_limits<T>::min)(), (std::numeric_limits<T>::min)() };
		}

#pragma endregion

	};

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const vec2<T>& v) {
		os << v.x << ", " << v.y;
		return os;
	}

	typedef vec2<int> vec2i;
	typedef vec2<float> vec2f;
	typedef vec2<double> vec2d;

}