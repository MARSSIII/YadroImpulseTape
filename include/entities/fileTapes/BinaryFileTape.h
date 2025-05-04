#pragma once

#include "../../interfaces/TapeInterface.h"
#include "../TapeConfig.h"

#include <fstream>
#include <string>

class BinaryFileTape : public TapeInterface {
public:
  BinaryFileTape(const std::string &filename, const TapeConfig &config);
  ~BinaryFileTape() noexcept;

  int read() final;
  void write(int data) final;
  void moveLeft() final;
  void moveRight() final;
  void rewind() final;
  bool isAtEnd() const final;
  int getSize() const final;

private:
  int currentPosition;
  int size;

  std::fstream file;
  std::string filename;

  TapeConfig config;

  void updateSize();
  void applyDelay(int delay) const;
};
