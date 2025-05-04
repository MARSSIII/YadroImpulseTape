#include "../../include/factories/FileTapeConfigFactory.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

FileTapeConfigFactory::FileTapeConfigFactory(const std::string &filename)
    : configFile(filename) {}

TapeConfig FileTapeConfigFactory::createConfig() const {
  TapeConfig config;
  std::ifstream file(configFile);

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
        config.readDelay = value;

      else if (key == "write_delay")
        config.writeDelay = value;

      else if (key == "rewind_delay")
        config.rewindDelay = value;

      else if (key == "shift_delay")
        config.shiftDelay = value;
    }
  }
  return config;
}
