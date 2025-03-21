#ifndef MICRO_LAT_HPP
#define MICRO_LAT_HPP

#include <numeric>
#include <random>
#include <vector>

#include "../data.hpp"
#include "../driver.hpp"
#include "../params.hpp"

template<typename T>
struct random_range {
  T min;
  T max;
  std::minstd_rand gen;
  std::uniform_int_distribution<T> dist;
  random_range(T min = 0, T max = std::numeric_limits<T>::max())
    : min{ min }, max{ max }, gen{}, dist{ min, max }
  {
  }

  /*
    https://stackoverflow.com/a/6852396
    Return random number in range [0,range) exclusive
  */
  inline T operator()(T range)
  {
    T bin_size = ((max - (range - 1)) / range) + 1;
    T bin_rmnd = ((max - (range - 1)) % range);
    T x;
    do {
      x = dist(gen);
    } while (max - bin_rmnd < x);

    return x / bin_size;
  }
};

/*
  Sandra Sattolo, An algorithm to generate a random cyclic permutation,
  Information Processing Letters, Volume 22, Issue 6, 1986,
  https://doi.org/10.1016/0020-0190(86)90073-6
*/
std::vector<unsigned int>
random_cycle(unsigned int nelems, unsigned int parallelism = 1)
{
  auto rng = random_range<uint64_t>();

  std::vector<unsigned int> v(nelems);
  std::iota(std::begin(v), std::end(v), 0);
  const auto cycle_len = nelems / parallelism;
  for (unsigned long p = 0; p < parallelism; p++) {
    for (unsigned long k = cycle_len - 1; k > 0; k--) {
      std::swap(v[parallelism * k + p], v[parallelism * rng(k) + p]);
    }
  }

  return v;
}

std::vector<unsigned int>
simple_cycle(unsigned int nelems, unsigned int parallelism = 1)
{
  std::vector<unsigned int> v(nelems);
  std::iota(std::begin(v), std::end(v), 1);
  v.back() = 0;

  return v;
}

namespace ietubench::driver {
class DriverLat : public Driver<DriverLat> {
public:
  int run()
  {
    /* Kernel Benchmark */
    for (size_t k = 0; k < nreps; k++) {
#ifdef USEM5OPS
      m5_reset_stats(0, 0);
#endif
#ifdef LIKWID_PERFMON
      LIKWID_MARKER_START("Compute");
#endif

      KERNEL((void*)&data);

#ifdef LIKWID_PERFMON
      LIKWID_MARKER_STOP("Compute");
#endif
#ifdef USEM5OPS
      m5_dump_stats(0, 0);
#endif

      time[k] = data.time;
    }
    return 0;
  }

  int init_data(const ietubench::params::Params& p)
  {
    uint64_t* r;
    uint64_t* q;
    size_t nlines = p.nbytes / p.line_size;
    size_t c_line = p.line_size / sizeof(uint64_t);
    size_t b_size = p.blocksize * 1024 * 1024;

    posix_memalign((void**)&r, p.page_size, b_size + 2 * p.page_size);
    posix_memalign((void**)&q, p.page_size, prlsm * sizeof(uint64_t*));

    auto v = random_cycle(nlines, prlsm);
    for (int e = 0; e < nlines; e++) {
      *(uint64_t**)&r[e * c_line] = (uint64_t*)&r[v[e] * c_line];
    }
    for (int e = 0; e < prlsm; e++) {
      q[e] = (uint64_t)&r[e * c_line];
    }

    data.nloop = nloop;
    data.threadId = 0;
    data.curr_buf = q;
    data.main_buf = r;

    return 0;
  }

  int free_data()
  {
    free(data.curr_buf);
    free(data.main_buf);
    return 0;
  }

  data_t data;
};
}

#endif // MICRO_LAT_HPP
