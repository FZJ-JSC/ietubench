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
  std::string output_suffix;
  unsigned int k_loop_len;
  unsigned int k_parallel;
  unsigned int verbose;

  Params(int argc, char* argv[]) : output_suffix{ ".sx" }, k_loop_len{ 16 }, k_parallel{ 0 }, verbose{ 0 }
  {
    int c = 0;
    while ((c = getopt(argc, argv, "hc:l:o:p:s:v:")) != -1) {
      switch (c) {
        case 'c':
          conf_file = std::string(optarg);
          break;
        case 'l':
          k_loop_len = atoi(optarg);
          break;
        case 'o':
          output_dir = std::string(optarg);
          break;
        case 'p':
          k_parallel = atoi(optarg);
          break;
        case 's':
          output_suffix = std::string(optarg);
          break;
        case 'v':
          verbose = atoi(optarg);
          break;
        case 'h':
          printf("Usage: %s -l <loop_len> -p <parallel> -c <config_file> -o <output_dir> -s <suffix>\n", argv[0]);
          exit(0);
        default:
          break;
      }
    }
  }
};
} // namespace j2gen

#endif /* PARAMS_HPP */