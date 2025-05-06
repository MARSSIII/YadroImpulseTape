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
  size_t getSize() const final;
  std::string getFileName() const;

private:
  size_t m_currentIndex;
  size_t m_totalNumbers;

  std::fstream m_file;
  std::string m_filename;

  TapeConfig m_config;

  std::vector<std::streampos> m_positions;
  bool m_fileModified;

  void buildPositionIndex();
  void updateFileSize();
  void applyDelay(int delay) const;
};
