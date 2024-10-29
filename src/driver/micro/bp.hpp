#ifndef MICRO_BP_HPP
#define MICRO_BP_HPP

#include "../data.hpp"
#include "../driver.hpp"
#include "../params.hpp"

template<typename T>
unsigned int
read_data(const std::string& path, void** curr_buf)
{
  FILE* file;
  unsigned int nbytes = 0;

  file = fopen(path.c_str(), "rb");
  fseek(file, 0, SEEK_END);
  nbytes = ftell(file) / sizeof(T);
  rewind(file);

  posix_memalign(curr_buf, PAGE_SIZE, nbytes * sizeof(T));

  fread(*curr_buf, sizeof(T), nbytes, file);
  fclose(file);
  return nbytes;
}

namespace ietubench::driver {
class DriverBP : public Driver<DriverBP> {
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
    data.threadId = 0;
    data.curr_val = p.params.size() > 0 ? p.params[0] : 0;
    nbytes = read_data<uint8_t>(p.files[0], (void**)&(data.curr_buf));
    return 0;
  }

  int free_data()
  {
    free(data.curr_buf);
    return 0;
  }

  data_t data;
  unsigned int nbytes;
};
}

#endif // MICRO_BP_HPP