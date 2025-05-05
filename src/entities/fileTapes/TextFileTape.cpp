#include "../../../include/entities/fileTapes/TextFileTape.h"

#include <cctype>
#include <chrono>
#include <stdexcept>
#include <thread>

TextFileTape::TextFileTape(const std::string &filename,
                           const TapeConfig &config)
    : filename(filename), config(config), currentIndex(0), totalNumbers(0),
      fileModified(false) {

  file.open(filename, std::ios::in | std::ios::out | std::ios::ate);
  if (!file) {
    file.clear();
    file.open(filename, std::ios::out);
    file.close();
    file.open(filename, std::ios::in | std::ios::out | std::ios::ate);
  }

  buildPositionIndex();
  rewind();
}

TextFileTape::~TextFileTape() noexcept {
  if (file.is_open()) {
    file.close();
  }
}

void TextFileTape::buildPositionIndex() {
  positions.clear();
  file.seekg(0);
  file.clear();

  char ch;
  bool numberStart = true;
  std::streampos startPos;

  while (file.get(ch)) {
    if (std::isdigit(ch) || (numberStart && (ch == '-' || ch == '+'))) {

      if (numberStart) {
        startPos = file.tellg() - std::streampos(1);
        numberStart = false;
      }

    } else if (!std::isspace(ch)) {
      numberStart = true;

    } else {

      if (!numberStart) {
        positions.push_back(startPos);
        numberStart = true;
      }
    }
  }

  if (!numberStart) {
    positions.push_back(startPos);
  }

  totalNumbers = positions.size();
  file.clear();
}

void TextFileTape::updateFileSize() {
  if (fileModified) {
    buildPositionIndex();
    fileModified = false;
  }
}

int TextFileTape::read() {
  if (isAtEnd()) {
    throw std::out_of_range("Read position out of range");
  }

  applyDelay(config.readDelay);

  file.seekg(positions[currentIndex]);
  int value;
  file >> value;

  return value;
}

void TextFileTape::write(int data) {
  applyDelay(config.writeDelay);

  file.seekp(0, std::ios::end);

  if (totalNumbers > 0) {
    file << " ";
  }

  const std::streampos writePos = file.tellp();
  file << data;
  file.flush();

  positions.push_back(writePos);
  totalNumbers++;

  currentIndex = totalNumbers - 1;
}

void TextFileTape::moveLeft() {
  if (currentIndex > 0) {
    applyDelay(config.shiftDelay);
    currentIndex--;
  }
}

void TextFileTape::moveRight() {
  if (currentIndex < totalNumbers) {
    applyDelay(config.shiftDelay);
    currentIndex++;
  }
}

void TextFileTape::rewind() {
  applyDelay(config.rewindDelay);
  currentIndex = 0;
}

bool TextFileTape::isAtEnd() const { return currentIndex >= totalNumbers; }

int TextFileTape::getSize() const { return totalNumbers; }

std::string TextFileTape::getFileName() const { return filename; }

void TextFileTape::applyDelay(int delay) const {
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}
