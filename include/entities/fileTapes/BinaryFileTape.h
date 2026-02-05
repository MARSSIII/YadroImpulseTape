#pragma once

#include "../../interfaces/TapeInterface.h"
#include "../TapeConfig.h"

#include <fstream>
#include <string>

class BinaryFileTape : public TapeInterface {
public:
  BinaryFileTape(const std::string &filename, const size_t sizeTape,
                 const TapeConfig &config);

  ~BinaryFileTape() noexcept;

  BinaryFileTape(const BinaryFileTape &) = delete;
  BinaryFileTape &operator=(const BinaryFileTape &) = delete;

  BinaryFileTape(BinaryFileTape &&other) noexcept;

  BinaryFileTape &operator=(BinaryFileTape &&other) noexcept;

  int read() final;

  void write(int data) final;

  void moveLeft() final;

  void moveRight() final;

  void rewind() final;

  bool isAtEnd() const final;

  size_t getSize() const final;

  size_t getMaxSize() const;

  std::string getFilename() const;

private:
  size_t m_currentPosition;
  size_t m_size;

  size_t m_maxSize;

  std::fstream m_file;
  std::string m_filename;

  TapeConfig m_config;

  void updateSize();

  void applyDelay(int delay) const;
};
