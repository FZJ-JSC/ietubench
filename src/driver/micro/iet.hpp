
#ifndef MICRO_IET_HPP
#define MICRO_IET_HPP

#include "../data.hpp"
#include "../driver.hpp"
#include "../params.hpp"

namespace ietubench::driver {
class DriverIET : public Driver<DriverIET> {
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
    data.nloop = nloop;
    return 0;
  }

  int free_data() { return 0; }

  data_t data;
};
}

#endif // MICRO_IET_HPP