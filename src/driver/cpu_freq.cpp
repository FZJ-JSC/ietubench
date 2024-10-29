#include <cstdint>
#include <cstdio>
#include <pthread.h>

#include "cpu_freq.hpp"
#include "data.hpp"

uint64_t
cycle_add(uint64_t nloop)
{

  uint64_t t0;
  uint64_t t1;

  __asm__ volatile("\
    isb \n\
    mrs %[t0], cntvct_el0       \n\
loop:                           \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    add x6, x6, x7              \n\
    sub  %[nloop], %[nloop], #1 \n\
    cbnz %[nloop], loop         \n\
    isb                         \n\
    mrs %[t1], cntvct_el0       \n\
"
                   : [t0] "=&r"(t0), [t1] "=&r"(t1)
                   : [nloop] "r"(nloop));

  return t1 - t0;
}

namespace ietubench::util {

std::tuple<double, double>
cpu_freq(unsigned int mghz)
{
  double clock_ns;
  double clock_cy;
  uint64_t frq = 0;
  uint64_t cyc = 0;
  __asm__ volatile("mrs \t%0, cntfrq_el0" : "=r"(frq));

  clock_ns = (double)frq / 1e9;

  /* Cycle Benchmark */
  if (mghz == 0) {
    const int nloop = 100000;
    const int nreps = 100;
    const int loopl = 100;
    for (int k = 0; k < nreps; ++k) {
      cyc += cycle_add(nloop);
    }
    clock_cy = (double)cyc / (nloop * loopl * nreps);
  } else {
    clock_cy = clock_ns * 1000.0 / mghz;
  }
  return { clock_cy, clock_ns };
}

}