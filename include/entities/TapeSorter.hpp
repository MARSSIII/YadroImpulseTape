#pragma once

#include "../entities/TapeConfig.h"
#include "../interfaces/TapeInterface.h"

#include <concepts>
#include <filesystem>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

template <typename T>
concept TapeInterfaceDerived = std::is_base_of_v<TapeInterface, T>;

template <TapeInterfaceDerived TapeT> class TapeSorter {
public:
  TapeSorter(size_t memoryLimit, const TapeConfig &configT);
  ~TapeSorter();

  void sort(TapeT &input, TapeT &output);

private:
  size_t m_memoryLimit;
  size_t m_maxElements;
  const std::string m_tmpDir;

  TapeConfig m_config;

  void cleanupTempFiles();
  void splitAndSort(TapeT &input, std::vector<std::unique_ptr<TapeT>> &temps);
  void merge(TapeT &output, std::vector<std::unique_ptr<TapeT>> &temps);
  void mergeTwo(TapeT &in1, TapeT &in2, TapeT &out);
};

#include "TapeSorter.tpp"
