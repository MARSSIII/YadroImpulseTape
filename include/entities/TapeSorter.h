#pragma once

#include "../interfaces/TapeInterface.h"
#include "TapeConfig.h"

#include <memory>
#include <string>
#include <vector>

class TapeSorter {
public:
  TapeSorter(size_t memoryLimit, TapeConfig config);

  void sort(TapeInterface &input, TapeInterface &output);

private:
  size_t m_memoryLimit;
  size_t m_maxElements;

  const TapeConfig m_config;

  const std::string m_tmpDir;

  void splitAndSort(TapeInterface &input,
                    std::vector<std::unique_ptr<TapeInterface>> &temps);
  void merge(TapeInterface &output,
             std::vector<std::unique_ptr<TapeInterface>> &temps);
  void mergeTwo(TapeInterface &in1, TapeInterface &in2, TapeInterface &out);
};
