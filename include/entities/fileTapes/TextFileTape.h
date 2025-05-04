#pragma once

#include "../../interfaces/TapeInterface.h"
#include "../TapeConfig.h"

#include <fstream>
#include <string>
#include <vector>

class TextFileTape : public TapeInterface {
public:
  TextFileTape(const std::string &filename, const TapeConfig &config);
  ~TextFileTape() noexcept;

  int read() final;
  void write(int data) final;
  void moveLeft() final;
  void moveRight() final;
  void rewind() final;
  bool isAtEnd() const final;
  int getSize() const final;

private:
  std::fstream file;
  std::string filename;
  TapeConfig config;

  std::vector<std::streampos> positions;
  size_t currentIndex;
  size_t totalNumbers;
  bool fileModified;

  void buildPositionIndex();
  void updateFileSize();
  void applyDelay(int delay) const;
};
