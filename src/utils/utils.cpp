#include "utils.hpp"
#include "../../include/entities/TapeConfig.h"
#include "../../include/entities/fileTapes/BinaryFileTape.h"
#include "../../include/entities/fileTapes/TextFileTape.h"
#include "../../include/interfaces/TapeInterface.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace fs = std::filesystem;

void utils::validateExtensions(const std::string &inputExt,
                               const std::string &outputExt) {
  const std::vector<std::string> allowedExts = {".txt", ".bin"};

  if (std::find(allowedExts.begin(), allowedExts.end(), inputExt) ==
      allowedExts.end()) {
    throw std::runtime_error("Unsupported input format: " + inputExt +
                             ". Allowed formats: .txt, .bin");
  }

  if (inputExt != outputExt) {
    throw std::runtime_error("Extension mismatch. Input: " + inputExt +
                             ", Output: " + outputExt);
  }
}

std::string utils::getFileExtension(const std::string &filename) {
  fs::path path(filename);
  std::string ext = path.extension().string();
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
  return ext;
}

std::unique_ptr<TapeInterface> utils::createTape(const TapeConfig &config,
                                                 const std::string &filename,
                                                 const std::string &ext) {
  if (ext == ".txt") {
    return std::make_unique<TextFileTape>(filename, config);
  }
  return std::make_unique<BinaryFileTape>(filename, config);
}

void utils::clearFile(const std::string &filename) {
  std::ofstream file(filename, std::ios::trunc);
  if (!file) {
    throw std::runtime_error("Failed to clear file: " + filename);
  }
}
