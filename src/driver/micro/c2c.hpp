
#ifndef MICRO_C2C_HPP
#define MICRO_C2C_HPP

#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

#include "../data.hpp"
#include "../driver.hpp"
#include "../params.hpp"

struct init_t {
  data_t** local_data;
  uint64_t threadId;
};

namespace ietubench::driver {

class DriverC2C : public Driver<DriverC2C> {
public:
  int run()
  {
    int err = 0;
    /* Kernel Benchmark */
    for (uint64_t k = 0; k < nreps; k++) {
#ifdef USEM5OPS
      m5_reset_stats(0, 0);
#endif
#ifdef LIKWID_PERFMON
      LIKWID_MARKER_START("Compute");
#endif
      for (uint64_t n = 0; n < nthreads; n++) {
        err = pthread_create(&thrd[n], &attr[n], KERNEL, data[n]);
      }

      for (uint64_t n = 0; n < nthreads; n++) {
        err = pthread_join(thrd[n], (void**)&time[n * nreps + k]);
      }
#ifdef LIKWID_PERFMON
      LIKWID_MARKER_STOP("Compute");
#endif
#ifdef USEM5OPS
      m5_dump_stats(0, 0);
#endif
    }
    return err;
  }

  int init_data(const ietubench::params::Params& p)
  {
    int err = 0;
    int tr;
    cpu_set_t cpu_set;
    std::unique_ptr<init_t[]> init;

    init = std::make_unique<init_t[]>(nthreads);
    data = std::make_unique<data_t*[]>(nthreads);
    thrd = std::make_unique<pthread_t[]>(nthreads);
    attr = std::make_unique<pthread_attr_t[]>(nthreads);

    for (uint64_t n = 0; n < nthreads; n++) {
      init[n].threadId = n;
      init[n].local_data = &data[n];

      CPU_ZERO(&cpu_set);
      CPU_SET(p.threads[n], &cpu_set);
      err = pthread_attr_init(&attr[n]);
      err = pthread_attr_setaffinity_np(&attr[n], sizeof(cpu_set_t), &cpu_set);
      err = pthread_create(&thrd[n], &attr[n], initialize_data_thread, &init[n]);
    }
    for (uint64_t n = 0; n < nthreads; n++) {
      err = pthread_join(thrd[n], (void**)&tr);
    }

    for (uint64_t n = 0; n < nthreads; n++) {
      size_t next = (n + 1) % nthreads;
      data[n]->main_buf = data[0]->main_buf;
      data[n]->mail_box = data[0]->mail_box;
      data[n]->nloop = nloop;

      // @TODO
      data[n]->next_buf = data[next]->curr_buf;
    }

    return err;
  }

  int free_data()
  {
    int err = 0;

    for (size_t n = 0; n < nthreads; n++) {
      if (data[n]->threadId == 0) {
        free(data[n]->main_buf);
        free(data[n]->mail_box);
      }
      free(data[n]->curr_buf);
      free(data[n]);
    }

    for (size_t n = 0; n < nthreads; n++) {
      err = pthread_attr_destroy(&attr[n]);
    }

    return 0;
  }

  static void* initialize_data_thread(void* p)
  {
    uint64_t* cache_line;
    init_t* _init = (init_t*)p;
    data_t** _data = (data_t**)_init->local_data;
    const auto page_size = PAGE_SIZE;

    uint64_t threadId = _init->threadId;

    posix_memalign((void**)_data, page_size, sizeof(data_t));
    memset((void*)*_data, 0, sizeof(data_t));

    (*_data)->threadId = threadId;
    (*_data)->curr_val = threadId == 0 ? 1 : 0;

    posix_memalign((void**)&cache_line, page_size, 8 * sizeof(uint64_t));
    memset((void*)cache_line, 0, 8 * sizeof(uint64_t));
    (*_data)->curr_buf = cache_line;

    if (threadId == 0) {
      posix_memalign((void**)&cache_line, page_size, 8 * sizeof(uint64_t));
      memset((void*)cache_line, 0, 8 * sizeof(uint64_t));
      (*_data)->main_buf = cache_line;

      posix_memalign((void**)&cache_line, page_size, 8 * sizeof(uint64_t));
      memset((void*)cache_line, 0, 8 * sizeof(uint64_t));
      (*_data)->mail_box = cache_line;
    }

    return (void**)threadId;
  }

  std::unique_ptr<data_t*[]> data;
  std::unique_ptr<pthread_t[]> thrd;
  std::unique_ptr<pthread_attr_t[]> attr;
};
}

#endif // MICRO_C2C_HPP
