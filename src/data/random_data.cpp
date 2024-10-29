#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <fstream>
#include <string>
#include <vector>

/*
  https://stackoverflow.com/a/6852396
  Return random number in range [0,range) exclusive
*/
uint64_t
random_max(uint64_t range)
{
  uint64_t bin_size = ((RAND_MAX - (range - 1)) / range) + 1;
  uint64_t bin_rmnd = ((RAND_MAX - (range - 1)) % range);
  uint64_t x;
  do {
    x = random();
  } while (RAND_MAX - bin_rmnd < x);

  return x / bin_size;
}

template<typename T>
int
write_random_data(std::string path, int size, int rmax)
{
  std::vector<T> data(size);
  std::generate(data.begin(), data.end(), [rmax]() { return random_max(rmax); });

  std::ofstream outfile(path, std::ios::out | std::ios::binary);
  outfile.write((const char*)&data[0], data.size());

  outfile.close();

  return size;
}

int
main(int argc, char** argv)
{
  using data_t = uint8_t;

  const int size = std::atoi(argv[1]);
  const int rmax = std::atoi(argv[2]);

  srandom(size);

  write_random_data<data_t>(std::format("data_s{}_r{}.dat", size, rmax), size, rmax);

  exit(0);
}
