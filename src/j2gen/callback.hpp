#ifndef CALLBACK_HPP
#define CALLBACK_HPP

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "spec.hpp"

namespace j2gen::callback {

class Registry {
  using Spec = spec::KernelSpec;

  struct NamedCB {
    std::string name;
    std::function<std::string(const unsigned int&, const Spec&)> f;
  };

  using CBList = std::vector<NamedCB>;

  static std::map<std::string, CBList> cb_registry;

public:
  static std::string add_arch(const std::string& group, const CBList& cb)
  {
    cb_registry.emplace(group, cb);
    return group;
  }
  template<typename T>
  static int add_callbacks(const std::string& group, T& parser)
  {
    if (cb_registry.count(group) == 0)
      return 0;
    for (const auto& g : cb_registry[group]) {
      parser.add_callback(g.name, g.f);
    }
    return cb_registry[group].size();
  }
};

std::map<std::string, Registry::CBList> Registry::cb_registry;
} // namespace j2gen::callback

#include "group/arm64.hpp"

#endif /* GENERATOR_HPP */
