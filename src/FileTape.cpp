#include "../include/entities/FileTape.h"

#include <chrono>
#include <stdexcept>
#include <thread>

FileTape::FileTape(const std::string &filename, const TapeConfig &config)
    : filename(filename), config(config), currentPosition(0), size(0) {
  file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
  if (!file.is_open()) {
    file.clear();
    file.open(filename, std::ios::out | std::ios::binary);
    file.close();
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
  }
  updateSize();
}

FileTape::~FileTape() {
  if (file.is_open())
    file.close();
}

void FileTape::updateSize() {
  file.seekg(0, std::ios::end);
  int bytes = file.tellg();
  size = bytes / sizeof(int);
  file.seekg(currentPosition * sizeof(int));
}

int FileTape::read() {
  if (currentPosition >= size)
    throw std::out_of_range("Read position out of range");

  applyDelay(config.readDelay);
  int value;
  file.read(reinterpret_cast<char *>(&value), sizeof(int));
  return value;
}

void FileTape::write(int data) {
  applyDelay(config.writeDelay);
  file.write(reinterpret_cast<const char *>(&data), sizeof(int));
  file.flush();
  updateSize();
}

void FileTape::moveLeft() {
  if (currentPosition > 0) {
    applyDelay(config.shiftDelay);
    currentPosition--;
    file.seekg(currentPosition * sizeof(int));
  }
}

void FileTape::moveRight() {
  applyDelay(config.shiftDelay);
  currentPosition++;
  if (currentPosition > size)
    size = currentPosition;
  file.seekg(currentPosition * sizeof(int));
}

void FileTape::rewind() {
  applyDelay(config.rewindDelay);
  currentPosition = 0;
  file.seekg(0);
}

bool FileTape::isAtEnd() const { return currentPosition >= size; }

int FileTape::getSize() const { return size; }

void FileTape::applyDelay(int delay) const {
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}
