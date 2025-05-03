#pragma once

#include <string>

struct TapeConfig {
  int readDelay = 0;
  int writeDelay = 0;
  int rewindDelay = 0;
  int shiftDelay = 0;

  TapeConfig() = default;
  explicit TapeConfig(const std::string &filename);
};
