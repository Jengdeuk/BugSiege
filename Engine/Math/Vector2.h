#pragma once

#include "Common/Common.h"

#include <Windows.h>
#include <cmath>

namespace JD
{
	template<typename T>
	class Vector2
	{
	public:
		Vector2() {}
		Vector2(T x, T y)
			: x(x), y(y)
		{
		}
		~Vector2() {}

		template<typename U>
		explicit Vector2(const Vector2<U>& other)
			: x(static_cast<T>(std::round(other.x))), y(static_cast<T>(std::round(other.y)))
		{
		}

		Vector2& Normalized()
		{
			const T magnitude = sqrt(x * x + y * y);
			x /= magnitude;
			y /= magnitude;

			return *this;
		}

		operator COORD() const
		{
			return COORD{ static_cast<short>(x), static_cast<short>(y) };
		}

		Vector2& operator+=(const Vector2& other)
		{
			x += other.x;
			y += other.y;

			return *this;
		}

		Vector2& operator-=(const Vector2& other)
		{
			x -= other.x;
			y -= other.y;

			return *this;
		}

		template<typename U>
		Vector2 operator*(const U& other) const
		{
			return Vector2(x * static_cast<T>(other), y * static_cast<T>(other));
		}

		template<typename U>
		Vector2 operator/(const U& other) const
		{
			return Vector2(x / static_cast<T>(other), y / static_cast<T>(other));
		}

	public:
		T x = 0;
		T y = 0;
	};

	template<typename T>
	Vector2<T> operator+(const Vector2<T>& left, const Vector2<T>& right)
	{
		return Vector2<T>(left.x + right.x, left.y + right.y);
	}

	template<typename T>
	Vector2<T> operator-(const Vector2<T>& left, const Vector2<T>& right)
	{
		return Vector2<T>(left.x - right.x, left.y - right.y);
	}

	template<typename T>
	bool operator==(const Vector2<T>& left, const Vector2<T>& right)
	{
		return left.x == right.x && left.y == right.y;
	}

	template<typename T>
	bool operator!=(const Vector2<T>& left, const Vector2<T>& right)
	{
		return !(left == right);
	}

	template<typename T>
	float Dot(const Vector2<T>& a, const Vector2<T>& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	template<typename T>
	float LengthSq(const Vector2<T>& v)
	{
		return Dot(v, v);
	}

	inline float Clamp(float t)
	{
		if (t < 0.f) return 0.f;
		if (t > 1.f) return 1.f;
		return t;
	}
}
