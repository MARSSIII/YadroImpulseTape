#pragma once

#include "../include/TapeInterface.h"

#include <memory>
#include <vector>

class TapeSorter {
public:
  TapeSorter() = default;
  void sort(TapeInterface &input, TapeInterface &output);

private:
  size_t maxElements;
};
