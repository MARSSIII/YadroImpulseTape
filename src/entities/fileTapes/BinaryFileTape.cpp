#include "../../../include/entities/fileTapes/BinaryFileTape.h"

#include <chrono>
#include <stdexcept>
#include <thread>

BinaryFileTape::BinaryFileTape(const std::string &filename,
                               const size_t sizeTape, const TapeConfig &config)
    : m_currentPosition(0), m_size(0), m_maxSize(sizeTape / sizeof(int)),
      m_filename(filename), m_config(config) {

  m_file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

  if (!m_file.is_open()) {
    m_file.clear();
    m_file.open(filename, std::ios::out);
    m_file.close();
    m_file.open(filename, std::ios::in | std::ios::out | std::ios::ate |
                              std::ios::binary);
  }

  updateSize();

  if (m_size > m_maxSize) {
    throw std::runtime_error("File size exceeds maximum allowed size");
  }
}

BinaryFileTape::~BinaryFileTape() noexcept {
  if (m_file.is_open())
    m_file.close();
}

BinaryFileTape::BinaryFileTape(BinaryFileTape &&other) noexcept
    : m_currentPosition(other.m_currentPosition), m_size(other.m_size),
      m_maxSize(other.m_maxSize), m_file(std::move(other.m_file)),
      m_filename(std::move(other.m_filename)),
      m_config(std::move(other.m_config)) {
  other.m_currentPosition = 0;
  other.m_size = 0;
  other.m_maxSize = 0;
}

BinaryFileTape &BinaryFileTape::operator=(BinaryFileTape &&other) noexcept {
  if (this != &other) {
    if (m_file.is_open())
      m_file.close();
    m_currentPosition = other.m_currentPosition;
    m_size = other.m_size;
    m_maxSize = other.m_maxSize;
    m_file = std::move(other.m_file);
    m_filename = std::move(other.m_filename);
    m_config = std::move(other.m_config);

    other.m_currentPosition = 0;
    other.m_size = 0;
    other.m_maxSize = 0;
  }
  return *this;
}

void BinaryFileTape::updateSize() {
  m_file.seekg(0, std::ios::end);

  int bytes = m_file.tellg();
  m_size = bytes / sizeof(int);

  m_file.seekg(m_currentPosition * sizeof(int));
}

int BinaryFileTape::read() {
  if (m_currentPosition >= m_size)
    throw std::out_of_range("Read position out of range");

  applyDelay(m_config.readDelay);

  m_file.seekg(m_currentPosition * sizeof(int));

  int value;
  m_file.read(reinterpret_cast<char *>(&value), sizeof(int));

  return value;
}

void BinaryFileTape::write(int data) {
  applyDelay(m_config.writeDelay);

  if (m_currentPosition >= m_maxSize) {
    throw std::out_of_range("Write position exceeds maximum size");
  }

  if (m_currentPosition > m_size) {
    throw std::out_of_range("Write position out of range");
  }

  m_file.seekp(m_currentPosition * sizeof(int));
  m_file.write(reinterpret_cast<char *>(&data), sizeof(int));
  m_file.flush();

  if (m_currentPosition == m_size) {
    m_size++;
    updateSize();
  }
}

void BinaryFileTape::moveLeft() {
  if (m_currentPosition > 0) {

    applyDelay(m_config.shiftDelay);

    m_currentPosition--;
    m_file.seekg(m_currentPosition * sizeof(int));
  }
}

void BinaryFileTape::moveRight() {
  if (m_currentPosition < m_maxSize || m_currentPosition > m_size) {
    applyDelay(m_config.shiftDelay);

    m_currentPosition++;
    m_file.seekg(m_currentPosition * sizeof(int));
  }
}

void BinaryFileTape::rewind() {
  applyDelay(m_config.rewindDelay);

  m_currentPosition = 0;
  m_file.seekg(0);
}

bool BinaryFileTape::isAtEnd() const { return m_currentPosition >= m_size; }

size_t BinaryFileTape::getSize() const { return m_size; }

size_t BinaryFileTape::getMaxSize() const { return m_maxSize; }

std::string BinaryFileTape::getFilename() const { return m_filename; }

void BinaryFileTape::applyDelay(int delay) const {
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}
