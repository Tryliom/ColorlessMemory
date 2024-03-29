#include "Random.h"

#include <random>

namespace Random
{
	[[nodiscard]] float Range(float min, float max) noexcept
	{
		if (min > max)
		{
			float temp = min;
			min = max;
			max = temp;
		}

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(min, max);

		return dis(gen);
	}

	[[nodiscard]] int Range(int min, int max) noexcept
	{
		if (min > max)
		{
			int temp = min;
			min = max;
			max = temp;
		}

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(min, max);

		return dis(gen);
	}
}
