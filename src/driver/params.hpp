#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <cstring>
#include <format>
#include <unistd.h>
#include <vector>

#define PAGE_SIZE sysconf(_SC_PAGESIZE)

#ifndef MAX_FILENAME_LEN
#define MAX_FILENAME_LEN 128
#endif

#ifndef MAX_NTHREADS_LEN
#define MAX_NTHREADS_LEN 512
#endif

std::vector<int>
parse_int_csv(const char* _str)
{
  std::vector<int> v;
  char _t[MAX_NTHREADS_LEN];
  char* _s;
  int _n = 0;

  std::strncpy(_t, _str, MAX_NTHREADS_LEN);

  _s = std::strtok(_t, ",");
  for (_n = 0; _s != NULL; _n++) {
    v.push_back(atoi(_s));
    _s = std::strtok(NULL, ",");
  }
  return v;
}

namespace ietubench::params {
struct Params {
  std::vector<int> params;
  std::vector<int> threads;
  std::vector<std::string> files;
  unsigned int nthreads;
  unsigned int nreps;
  unsigned int nloop;
  unsigned int verbose;
  unsigned int mghz;
  unsigned int page_size;
  bool ocsv;

  Params(int argc, char* argv[])
    : params{}, threads{ 0 }, files{}, nthreads{ 1 }, nreps{ 100 }, nloop{ 100 }, verbose{ 1 }, mghz{ 0 }, ocsv{ false }
  {

    page_size = PAGE_SIZE;
    int c;

    while ((c = getopt(argc, argv, "hof:l:n:p:q:r:t:v:")) != -1) {
      switch (c) {
        case 'f':
          files.push_back(std::string(optarg));
          break;
        case 'h':
          std::printf("Usage: %s -r <reps> -l <nloop>\n", argv[0]);
          std::exit(0);
        case 'l':
          nloop = atoi(optarg);
          break;
        case 'n':
          nthreads = atoi(optarg);
          threads.resize(nthreads);
          for (int i = 0; i < nthreads; i++)
            threads[i] = i;
          break;
        case 'o':
          ocsv = true;
          break;
        case 'p':
          params = parse_int_csv(optarg);
          break;
        case 'q':
          mghz = atoi(optarg);
          break;
        case 'r':
          nreps = atoi(optarg);
          break;
        case 't':
          threads = parse_int_csv(optarg);
          nthreads = threads.size();
          break;
        case 'v':
          verbose = atoi(optarg);
          break;
        default:
          break;
      }
    }
  }
};

std::ostream&
operator<<(std::ostream& os, const Params& p)
{
  os << std::format(" nthreads={} nreps={} nloop={}", p.nthreads, p.nreps, p.nloop);
  return os;
}
} // namespace ietubench

#endif /* PARAMS_HPP */