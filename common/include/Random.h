#pragma once

/**
* @headerfile Random number generator functions and classes
* @author Alexis
*/

#include <random>

/**
 * @brief A namespace for random number generator functions and classes
 */
namespace Random
{
    [[nodiscard]] inline float Range(float min, float max) noexcept;
    [[nodiscard]] inline int Range(int min, int max) noexcept;
}