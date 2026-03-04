#pragma once

#include <time.h>

namespace Util
{
	inline void SetRandomSeed()
	{
		srand(static_cast<unsigned int>(time(nullptr)));
	}

	inline int Random(int min, int max)
	{
		int diff = (max - min) + 1;
		return ((diff * rand()) / (RAND_MAX + 1)) + min;
	}

	inline float Randomf(float min, float max)
	{
		float random
			= static_cast<float>(rand())
			/ static_cast<float>(RAND_MAX);

		float diff = (max - min);
		return (random * diff) + min;
	}

	template<typename T>
	T Clamp(T value, T min, T max)
	{
		if (value < min)
		{
			value = min;
		}
		else if (value > max)
		{
			value = max;
		}

		return value;
	}
}

template<typename T>
void SafeDelete(T*& t)
{
	if (t)
	{
		delete t;
		t = nullptr;
	}
}

template<typename T>
void SafeDeleteArray(T*& t)
{
	if (t)
	{
		delete[] t;
		t = nullptr;
	}
}
