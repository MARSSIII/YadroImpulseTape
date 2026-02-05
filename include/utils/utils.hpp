#pragma once

#include <memory>
#include <string>

struct TapeConfig;
class TapeInterface;

namespace utils {
void validateExtensions(const std::string &inputExt,
                        const std::string &outputExt);

std::string getFileExtension(const std::string &filename);

std::unique_ptr<TapeInterface> createTape(const size_t maxSize,
                                          const TapeConfig &config,
                                          const std::string &filename,
                                          const std::string &ext);

void clearFile(const std::string &filename);

size_t getFileSize(const std::string &filename);

} // namespace utils
