#include "generator.hpp"
#include "params.hpp"
#include "parser.hpp"

using namespace j2gen;

int
main(int argc, char** argv)
{
  Params params(argc, argv);
  parser::Jinja2 parser;

  if (!params.conf_file.empty())
    parser.set_config(params.conf_file).set_loop_span(params.loopl);
  else
    return 0;

  for (const auto& g : generator::iet_bench) {
    parser.add_callback(g.name, g.f);
  }

  for (const auto& [name, text, span] : parser) {
    std::string output_file = params.output_dir / std::format("{}_{}.{}", name, span, params.ext);
    if (params.verbose > 0)
      std::cout << "  generating " << output_file << std::endl;
    std::ofstream ofs(output_file, std::ofstream::out);
    ofs << text << std::endl;
    ofs.close();
  }

  return 0;
}