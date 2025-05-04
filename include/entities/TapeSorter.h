#pragma once

#include "../interfaces/TapeInterface.h"

#include <memory>
#include <vector>

class TapeSorter {
public:
  explicit TapeSorter(size_t memoryLimit);

  void sort(TapeInterface &input, TapeInterface &output);

private:
  size_t memoryLimit;
  size_t maxElements;

  void splitAndSort(TapeInterface &input,
                    std::vector<std::unique_ptr<TapeInterface>> &temps);
  void merge(std::vector<std::unique_ptr<TapeInterface>> &temps,
             TapeInterface &output);
  void mergeTwo(TapeInterface &in1, TapeInterface &in2, TapeInterface &out);
};
