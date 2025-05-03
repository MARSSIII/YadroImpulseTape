#include "../include/TapeConfig.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

TapeConfig::TapeConfig(const std::string &filename) {
  std::ifstream file(filename);

  if (!file)
    throw std::runtime_error("Config file not found");

  std::string line;
  while (std::getline(file, line)) {

    std::istringstream iss(line);
    std::string key;

    if (std::getline(iss, key, '=')) {

      int value;
      iss >> value;

      if (key == "read_delay")
        readDelay = value;

      else if (key == "write_delay")
        writeDelay = value;

      else if (key == "rewind_delay")
        rewindDelay = value;

      else if (key == "shift_delay")
        shiftDelay = value;
    }
  }
}
