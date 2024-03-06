#pragma once

/**
* @headerfile Random number generator functions and classes
* @author Alexis
*/

#include <vector>
#include <random>

/**
 * @brief A namespace for random number generator functions and classes
 */
namespace Random
{
    [[nodiscard]] float Range(float min, float max) noexcept;
    [[nodiscard]] int Range(int min, int max) noexcept;
	template <typename T>
	void Shuffle(std::vector<T>& vec) noexcept
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::shuffle(vec.begin(), vec.end(), gen);
	}
}