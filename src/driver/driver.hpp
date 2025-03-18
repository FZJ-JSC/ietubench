#ifndef DRIVER_HPP
#define DRIVER_HPP

#include <cstdint>
#include <memory>

#include "params.hpp"

#ifdef USEM5OPS
#include <gem5/m5ops.h>
#endif

#ifdef LIKWID_PERFMON
#include <likwid-marker.h>
#endif

#ifndef KERNEL_LOOP_LEN
#define KERNEL_LOOP_LEN 1
#endif

#ifndef KERNEL_PARALLEL
#define KERNEL_PARALLEL 0
#endif

extern "C"
{
  void* KERNEL(void*);
}

namespace ietubench::driver {

constexpr unsigned int k_loop_len = KERNEL_LOOP_LEN;
constexpr unsigned int k_parallel = KERNEL_PARALLEL;

template<typename D>
class Driver {
public:
  Driver() {}

  int set_affinity(const int& cpu_id)
  {
    int err = 0;
    cpu_set_t cpu_set;

    if (cpu_id > -1) {
      pthread_t t = pthread_self();
      CPU_ZERO(&cpu_set);
      CPU_SET(cpu_id, &cpu_set);
      err = pthread_setaffinity_np(t, sizeof(cpu_set_t), &cpu_set);
    }

    return err;
  }

  int set_params(const ietubench::params::Params& p)
  {
    nthreads = p.nthreads;
    nreps = p.nreps;
    nloop = p.nloop;
    prlsm = k_parallel;
    iter = nloop * k_loop_len;

    set_affinity(p.threads[0]);

    time = std::make_unique<uint64_t[]>(nreps * nthreads);
    static_cast<D*>(this)->init_data(p);

#ifdef LIKWID_PERFMON
    LIKWID_MARKER_INIT;
    {
      LIKWID_MARKER_REGISTER("Compute");
    }
#endif
    return 0;
  }

  int clean()
  {

#ifdef LIKWID_PERFMON
    LIKWID_MARKER_CLOSE;
#endif

    return static_cast<D*>(this)->free_data();
  }

  int operator()() { return static_cast<D*>(this)->run(); }

  uint64_t nthreads;
  uint64_t nreps;
  uint64_t nloop;
  uint64_t iter;
  unsigned int prlsm;
  std::unique_ptr<uint64_t[]> time;
};
}

#endif // DRIVER_HPP