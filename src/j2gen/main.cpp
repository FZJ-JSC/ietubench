#include "callback.hpp"
#include "params.hpp"
#include "parser.hpp"

using namespace j2gen;

int
main(int argc, char** argv)
{
  Params params(argc, argv);
  parser::Jinja2 parser;

  if (!params.conf_file.empty())
    parser
      .set_config(params.conf_file)
      .set_loop_span(params.k_loop_len)
      .set_parallelism(params.k_parallel);
  else
    return 0;

  callback::Registry::add_callbacks(params.group, parser);

  for (const auto& [name, text] : parser) {
    std::string output_file = params.output_dir / std::format("{}{}", name, params.output_suffix);
    if (params.verbose > 0)
      std::cout << "  generating " << output_file << std::endl;
    std::ofstream ofs(output_file, std::ofstream::out);
    ofs << text << std::endl;
    ofs.close();
  }

  return 0;
}