#ifndef HIGHEST_RESOLUTION_STEADY_CLOCK_HPP
#define HIGHEST_RESOLUTION_STEADY_CLOCK_HPP

#include <chrono>

using highest_resolution_steady_clock = std::conditional_t<
    std::chrono::high_resolution_clock::is_steady,
    std::chrono::high_resolution_clock,
    std::chrono::steady_clock
>;

#endif
