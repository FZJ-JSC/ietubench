#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <filesystem>
#include <format>
#include <unistd.h>

namespace j2gen {
struct Params {
public:
  std::filesystem::path output_dir;
  std::filesystem::path conf_file;
  std::string conf_string;
  std::string ext;
  unsigned int loopl;
  unsigned int verbose;

  Params(int argc, char* argv[]) : ext{ "sx" }, loopl{ 16 }, verbose{ 0 }
  {
    int c = 0;
    while ((c = getopt(argc, argv, "hc:e:l:o:s:t:v:")) != -1) {
      switch (c) {
        case 'c':
          conf_file = std::string(optarg);
          break;
        case 'e':
          ext = std::string(optarg);
          break;
        case 'l':
          loopl = atoi(optarg);
          break;
        case 'o':
          output_dir = std::string(optarg);
          break;
        case 's':
          conf_string = std::string(optarg);
          break;
        case 'v':
          verbose = atoi(optarg);
          break;
        case 'h':
          printf("Usage: %s -l <loopl> -c <config_file> -o <output_dir> -e <ext>\n", argv[0]);
          exit(0);
        default:
          break;
      }
    }
  }
};
} // namespace j2gen

#endif /* PARAMS_HPP */