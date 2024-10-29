#ifndef STATS_HPP
#define STATS_HPP

#include <cmath>
#include <cstdint>
#include <format>

namespace ietubench::util {

template<typename I, typename F>
struct Stats {
  int N;
  F amean;
  F arate;
  F stdev;
  I sum;
  I min;
  I max;
  F time_unit;
  I work_unit;
  bool ocsv;

  Stats<I, F>& update(const int _N, uint64_t* t)
  {
    N = _N;
    sum = t[0];
    min = t[0];
    max = t[0];
    arate = 1.0;
    amean = 0.0;
    stdev = 0.0;

    for (int k = 1; k < N; k++) {
      sum += t[k];
      min = t[k] < min ? t[k] : min;
      max = t[k] > max ? t[k] : max;
    }
    amean = (double)sum / N;
    arate = (double)N / sum;

    for (int k = 0; k < N; k++)
      stdev += (t[k] - amean) * (t[k] - amean);
    if (N > 1)
      stdev = std::sqrt(stdev / (N - 1));

    return *this;
  }

  Stats<I, F>& set_work(const I& work)
  {
    work_unit = work;
    return *this;
  }
  Stats<I, F>& set_time(const F& time)
  {
    time_unit = time;
    return *this;
  }
  Stats<I, F>& set_output(const bool& _csv)
  {
    ocsv = _csv;
    return *this;
  }
};

template<typename I, typename F>
std::ostream&
operator<<(std::ostream& os, Stats<I, F> const& s)
{
  const auto work_time = s.work_unit * s.time_unit;
  const auto out = s.ocsv ? std::format("{:4.2f},{:4.2f},{:4.2f},{:4.2f},{:4.2f},{:4.2f}",
                                        s.min / work_time,
                                        s.max / work_time,
                                        s.amean / work_time,
                                        s.arate * work_time,
                                        s.stdev / s.amean,
                                        s.amean / s.time_unit)
                          : std::format(" min={:4.2f} max={:4.2f} mean={:4.2f} arate={:4.2f} CV={:4.2f}",
                                        s.min / work_time,
                                        s.max / work_time,
                                        s.amean / work_time,
                                        s.arate * work_time,
                                        s.stdev / s.amean);
  return os << out;
}

}
#endif /* STATS_HPP */
