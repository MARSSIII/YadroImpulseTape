#pragma once

// #include "../../include/entities/TapeSorter.hpp"

#include <memory>
#include <string>

class TapeConfig;
class TapeInterface;

namespace utils {
void validateExtensions(const std::string &inputExt,
                        const std::string &outputExt);
std::string getFileExtension(const std::string &filename);
std::unique_ptr<TapeInterface> createTape(const TapeConfig &config,
                                          const std::string &filename,
                                          const std::string &ext);
void clearFile(const std::string &filename);
/*
template <typename TapeT>
void processTapes(const TapeConfig &config, const std::string &inputPath,
                  const std::string &outputPath) {
  auto inputTape = std::make_unique<TapeT>(inputPath, config);
  auto outputTape = std::make_unique<TapeT>(outputPath, config);

  TapeSorter<TapeT> sorter(12, config);
  sorter.sort(*inputTape, *outputTape);
}
*/
} // namespace utils
