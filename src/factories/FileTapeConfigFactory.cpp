#include "../../include/factories/FileTapeConfigFactory.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string_view>

using namespace std::string_view_literals;

void trimWhitespace(std::string &s) {
  constexpr auto whitespace = " \t\n\r\f\v"sv;
  s.erase(s.find_last_not_of(whitespace) + 1);
  s.erase(0, s.find_first_not_of(whitespace));
}

FileTapeConfigFactory::FileTapeConfigFactory(std::string filename)
    : configFile_(std::move(filename)) {}

TapeConfig FileTapeConfigFactory::create() const {
  TapeConfig config;

  if (configFile_.empty()) {
    return config;
  }

  std::ifstream file(configFile_);

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open config file: " + configFile_);
  }

  std::string line;
  int lineNumber = 0;
  while (std::getline(file, line)) {
    ++lineNumber;

    try {
      parseLine(line, config);
    } catch (const std::exception &e) {
      throw std::runtime_error("Error in line " + std::to_string(lineNumber) +
                               ": " + e.what());
    }
  }

  validateConfig(config);
  return config;
}

void FileTapeConfigFactory::parseLine(const std::string &line,
                                      TapeConfig &config) const {
  std::string trimmed = line;
  trimWhitespace(trimmed);

  if (trimmed.empty() || trimmed[0] == '#') {
    return;
  }

  std::istringstream iss(trimmed);
  std::string key;
  if (!std::getline(iss, key, '=')) {
    throw std::runtime_error("Invalid format");
  }

  trimWhitespace(key);
  std::string valueStr;
  if (!std::getline(iss, valueStr)) {
    throw std::runtime_error("Missing value");
  }
  trimWhitespace(valueStr);

  int value;
  try {
    value = std::stoi(valueStr);
  } catch (const std::exception &) {
    throw std::runtime_error("Invalid integer value: " + valueStr);
  }

  if (key == "read_delay") {
    config.readDelay = value;
  } else if (key == "write_delay") {
    config.writeDelay = value;
  } else if (key == "rewind_delay") {
    config.rewindDelay = value;
  } else if (key == "shift_delay") {
    config.shiftDelay = value;
  } else {
    throw std::runtime_error("Unknown config key: " + key);
  }
}

void FileTapeConfigFactory::validateConfig(const TapeConfig &config) const {
  const auto checkNegative = [](int value, const std::string &name) -> void {
    if (value < 0) {
      throw std::runtime_error(name + " cannot be negative");
    }
  };

  checkNegative(config.readDelay, "Read delay");
  checkNegative(config.writeDelay, "Write delay");
  checkNegative(config.rewindDelay, "Rewind delay");
  checkNegative(config.shiftDelay, "Shift delay");
}
