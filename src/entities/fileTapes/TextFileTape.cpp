#include "../../../include/entities/fileTapes/TextFileTape.h"
#include <cctype>
#include <chrono>
#include <stdexcept>
#include <thread>

TextFileTape::TextFileTape(const std::string &filename,
                           const TapeConfig &config)
    : m_currentIndex(0), m_totalNumbers(0), m_filename(filename),
      m_config(config), m_fileModified(false) {

  m_file.open(m_filename, std::ios::in | std::ios::out | std::ios::ate);

  if (!m_file) {
    m_file.clear();
    m_file.open(m_filename, std::ios::out);
    m_file.close();
    m_file.open(m_filename, std::ios::in | std::ios::out | std::ios::ate);
  }

  buildPositionIndex();
  rewind();
}

TextFileTape::~TextFileTape() noexcept {
  if (m_file.is_open()) {
    m_file.close();
  }
}

void TextFileTape::buildPositionIndex() {
  m_positions.clear();
  m_file.seekg(0);
  m_file.clear();
  char ch;
  bool numberStart = true;
  std::streampos startPos;
  while (m_file.get(ch)) {
    if (std::isdigit(ch) || (numberStart && (ch == '-' || ch == '+'))) {
      if (numberStart) {
        startPos = m_file.tellg() - std::streampos(1);
        numberStart = false;
      }
    } else if (!std::isspace(ch)) {
      numberStart = true;
    } else {
      if (!numberStart) {
        m_positions.push_back(startPos);
        numberStart = true;
      }
    }
  }
  if (!numberStart) {
    m_positions.push_back(startPos);
  }
  m_totalNumbers = m_positions.size();
  m_file.clear();
}

void TextFileTape::updateFileSize() {
  if (m_fileModified) {
    buildPositionIndex();
    m_fileModified = false;
  }
}

int TextFileTape::read() {
  if (isAtEnd()) {
    throw std::out_of_range("Read position out of range");
  }
  applyDelay(m_config.readDelay);
  m_file.seekg(m_positions[m_currentIndex]);
  int value;
  m_file >> value;
  return value;
}

void TextFileTape::write(int data) {
  applyDelay(m_config.writeDelay);
  m_file.seekp(0, std::ios::end);
  if (m_totalNumbers > 0) {
    m_file << " ";
  }
  const std::streampos writePos = m_file.tellp();
  m_file << data;
  m_file.flush();
  m_positions.push_back(writePos);
  m_totalNumbers++;
  m_currentIndex = m_totalNumbers - 1;
}

void TextFileTape::moveLeft() {
  if (m_currentIndex > 0) {
    applyDelay(m_config.shiftDelay);
    m_currentIndex--;
  }
}

void TextFileTape::moveRight() {
  if (m_currentIndex < m_totalNumbers) {
    applyDelay(m_config.shiftDelay);
    m_currentIndex++;
  }
}

void TextFileTape::rewind() {
  applyDelay(m_config.rewindDelay);
  m_currentIndex = 0;
}

bool TextFileTape::isAtEnd() const { return m_currentIndex >= m_totalNumbers; }

size_t TextFileTape::getSize() const { return m_totalNumbers; }

std::string TextFileTape::getFileName() const { return m_filename; }

void TextFileTape::applyDelay(int delay) const {
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}
