#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "spec.hpp"

#include <format>
#include <functional>

namespace j2gen::generator {

using namespace spec;

template<class S>
struct Generator {
  std::string name;
  std::function<std::string(const unsigned int&, const S&)> f;
};

struct cyclic_counter {
  cyclic_counter& operator++()
  {
    idx = idx + 1 < max ? idx + 1 : min;
    return *this;
  }
  bool operator!=(const cyclic_counter& b) const { return idx != b.idx; }
  int operator*() const { return idx; }

  int min;
  int max;
  int idx;
};

const std::vector<Generator<KernelSpec>> iet_bench{
  { "iet_bench",
    [](const unsigned int& loop_len, const auto& s) {
      std::string loop_text;
      std::map<std::string, cyclic_counter> rg_counter;
      for (const auto& [rg_t, regs] : s.regs) {
        rg_counter.emplace(rg_t, cyclic_counter{ regs[0], regs[1], regs[0] });
      }

      const auto l_len = loop_len;
      const auto i_len = s.ops.size();
      for (unsigned int l = 0; l < l_len; l += i_len) {
        for (auto const& op : s.ops) {
          const auto r_0 = *rg_counter[op.rg_t];
          ++rg_counter[op.rg_t];

          loop_text += std::format("   {} {}{}{}", op.inst, op.rg_t, r_0, op.rg_s);
          if (!op.rg_p.empty()) {
            loop_text += std::format(", p{}/{}", r_0, op.rg_p);
          }
          for (int i = 0; i < op.operands; i++) {
            const auto r_i = s.srcs.at(op.rg_t)[i];
            loop_text += std::format(", {}{}{}", op.rg_t, r_i > 0 ? r_i : r_0, op.rg_s);
          }
          loop_text += "\n";
        }
      }
      return loop_text;
    } }
};

} // namespace j2gen::generator

#endif /* GENERATOR_HPP */