#ifndef SPEC_HPP
#define SPEC_HPP

#include <map>
#include <string>
#include <vector>

namespace j2gen::spec {
struct InstSpec {
  std::string inst;
  std::string rg_t;
  std::string rg_s;
  std::string rg_p;
  int operands;
};
struct KernelSpec {
  std::vector<InstSpec> ops;
  std::map<std::string, std::vector<int>> regs;
  std::map<std::string, std::vector<int>> srcs;
};
}

#endif /* SPEC_HPP */
