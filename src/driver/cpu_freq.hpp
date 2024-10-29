#ifndef CPU_FREQ_HPP
#define CPU_FREQ_HPP

#include <tuple>

namespace ietubench::util {
std::tuple<double, double>
cpu_freq(unsigned int mghz = 0);
}

#endif // CPU_FREQ_HPP