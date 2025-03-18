#include <format>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu_freq.hpp"
#include "data.hpp"
#include "micro.hpp"
#include "params.hpp"
#include "stats.hpp"

using namespace ietubench;

int
main(int argc, char* argv[])
{
  params::Params p(argc, argv);

  if (!p.ocsv) {
    std::cout << std::format("kernel={} k_loop={} k_para={}", TO_STR(KERNEL), driver::k_loop_len, driver::k_parallel);
    std::cout << p << std::endl;
  }

  driver::KERNEL_DRIVER drv;

  drv.set_params(p);

  drv();

  const auto [clock_cy, clock_ns] = util::cpu_freq(p.mghz);
  const auto clock_fr = p.mghz > 0 ? p.mghz : 1000.0 * clock_ns / clock_cy;
  util::Stats<uint64_t, double> stats;
  for (int n = 0; n < drv.nthreads; n++) {
    stats.update(drv.nreps, &drv.time[drv.nreps * n]);
    std::cout << (p.ocsv ? std::format("{},{:4.2f},{},", n, clock_fr, drv.iter)
                         : std::format("thread={} freq={:4.2f} iter={} ", n, clock_fr, drv.iter));
    std::cout << stats.set_output(p.ocsv).set_work(drv.iter).set_time(clock_cy) << (p.ocsv ? "," : " [cy/ops]");
    std::cout << std::endl;
  }

  drv.clean();

  return 0;
}
