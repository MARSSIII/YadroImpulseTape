#include "../../../include/entities/fileTapes/BinaryFileTape.h"

#include <chrono>
#include <stdexcept>
#include <thread>

BinaryFileTape::BinaryFileTape(const std::string &filename,
                               const TapeConfig &config)
    : currentPosition(0), size(0), filename(filename), config(config) {

  file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

  if (!file.is_open()) {
    file.clear();
    file.open(filename, std::ios::out);
    file.close();
    file.open(filename, std::ios::in | std::ios::out | std::ios::ate);
  }

  updateSize();
}

BinaryFileTape::~BinaryFileTape() noexcept {
  if (file.is_open())
    file.close();
}

void BinaryFileTape::updateSize() {
  file.seekg(0, std::ios::end);

  int bytes = file.tellg();
  size = bytes / sizeof(int);

  file.seekg(currentPosition * sizeof(int));
}

int BinaryFileTape::read() {
  if (currentPosition >= size)
    throw std::out_of_range("Read position out of range");

  applyDelay(config.readDelay);

  int value;
  file.read(reinterpret_cast<char *>(&value), sizeof(int));

  return value;
}

void BinaryFileTape::write(int data) {
  applyDelay(config.writeDelay);

  if (currentPosition > size) {
    throw std::out_of_range("Write position out of range");
  }

  file.seekp(currentPosition * sizeof(int));
  file.write(reinterpret_cast<const char *>(&data), sizeof(int));
  file.flush();

  if (currentPosition == size) {
    size++;
  }

  currentPosition++;
}

void BinaryFileTape::moveLeft() {
  if (currentPosition > 0) {

    applyDelay(config.shiftDelay);

    currentPosition--;
    file.seekg(currentPosition * sizeof(int));
  }
}

void BinaryFileTape::moveRight() {
  if (currentPosition < size) {
    applyDelay(config.shiftDelay);
    currentPosition++;
    file.seekg(currentPosition * sizeof(int));
  }
}

void BinaryFileTape::rewind() {
  applyDelay(config.rewindDelay);

  currentPosition = 0;
  file.seekg(0);
}

bool BinaryFileTape::isAtEnd() const { return currentPosition >= size; }

int BinaryFileTape::getSize() const { return size; }

void BinaryFileTape::applyDelay(int delay) const {
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}
