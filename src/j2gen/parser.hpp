#ifndef PARSER_HPP
#define PARSER_HPP
#include <filesystem>
#include <format>
#include <regex>
#include <string>

#include <inja.hpp>

#include "spec.hpp"

namespace j2gen::parser {
using json_type = inja::json;
using args_type = inja::Arguments;
using ctxt_type = inja::Environment;

class Jinja2 {
  class iterator {
  public:
    iterator(const Jinja2* f, const unsigned int idx = 0) : f{ f }, idx{ idx } {}
    iterator& operator++()
    {
      ++idx;
      return *this;
    }
    bool operator!=(const iterator& b) const { return idx != b.idx; }
    std::tuple<std::string, std::string> operator*() { return f->get_k(idx); }

  private:
    unsigned int idx;
    const Jinja2* f;
  };

public:
  Jinja2() : ctxt{}, config{}, config_path{}, _size{ 0 }, _span{ 16 }
  {
    ctxt.set_trim_blocks(true);
    ctxt.set_lstrip_blocks(true);
  };

  Jinja2& set_config(const std::filesystem::path& path)
  {
    config_path = path;
    config = nlohmann::json::parse(std::ifstream(path));
    if (config.contains("kernels") && config.at("kernels").is_array())
      _size = config["kernels"].size();
    return *this;
  }

  Jinja2& set_loop_span(const unsigned int& span)
  {
    _span = span;
    return *this;
  }

  Jinja2& set_parallelism(const unsigned int& para)
  {
    _para = para;
    return *this;
  }

  template<class O>
  Jinja2& add_callback(const std::string& name, const std::function<std::string(const unsigned int&, const O&)>& f)
  {
    ctxt.add_callback(name, 2, [f](args_type& args) { return f(args[0]->get<unsigned int>(), args[1]->get<O>()); });
    return *this;
  }

  iterator begin() const { return iterator(this); }
  iterator end() const { return iterator(this, _size); }

private:
  const std::tuple<std::string, std::string> get_k(const unsigned int& k) const
  {
    if (_size == 0)
      return {};
    auto data = config["kernels"][k];
    auto span = data.value("span", _span);
    auto para = data.value("para", _para);
    data["span"] = span;
    data["para"] = para;

    std::string tmpl_path;
    if (config.contains("template"))
      tmpl_path = config_path.parent_path() / config["template"];
    else
      tmpl_path = config_path.parent_path() / data["template"];

    return { data["name"], ctxt.render_file(tmpl_path, data) };
  }

  mutable ctxt_type ctxt;
  std::filesystem::path config_path;
  json_type config;
  unsigned int _size;
  unsigned int _span;
  unsigned int _para;
};

} // namespace j2gen::parser

namespace j2gen::spec {

void
from_json(const j2gen::parser::json_type& j, KernelSpec& s)
{
  std::regex re(R"(([a-z]+)_?(\d)?\.?(\w)?([mz])?(\.\w+)?)");
  std::smatch m;
  std::vector<std::string> inst;
  j.at("inst").get_to(inst);
  for (const auto& e : inst) {
    if (std::regex_match(e, m, re))
      s.ops.push_back({ m[1], m[3], m[5], m[4], m[2].matched ? std::stoi(m[2]) : 2 });
    else
      s.ops.push_back({ e, "", "", "", 2 });
  }
  for (auto& e : s.ops)
    e.rg_t = e.rg_t.empty() ? "x" : e.rg_t;

  const auto _reg_t = s.ops[0].rg_t;
  if (j.at("regs").is_object()) {
    j.at("regs").get_to(s.regs);
  } else {
    s.regs.insert({ _reg_t, j.at("regs").get<std::vector<int>>() });
  }

  if (j.contains("srcs")) {
    if (j.at("srcs").is_object()) {
      j.at("srcs").get_to(s.srcs);
    } else {
      s.srcs.insert({ _reg_t, j.at("srcs").get<std::vector<int>>() });
    }
  }

  for (auto& [rg_t, regs] : s.regs) {
    if (!s.srcs.contains(rg_t)) {
      s.srcs.emplace(rg_t, std::vector<int>{ -1, -1, -1 });
    };
  }
}
} // namespace j2gen::generator

#endif /* PARSER_HPP */