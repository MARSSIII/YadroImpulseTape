#include <iostream>

#include "../include/FileTape.h"
#include "../include/TapeConfig.h"
#include "../include/TapeSorter.h"

int main(int argc, char *argv[]) {

  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <input> <output> [config]\n";
    return 1;
  }

  try {
    TapeConfig config(argc > 3 ? argv[3] : "config.txt");
    FileTape input(argv[1], config);
    FileTape output(argv[2], config);

    TapeSorter sorter;
    sorter.sort(input, output);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
