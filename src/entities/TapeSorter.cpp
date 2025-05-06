#include "../../include/entities/TapeSorter.h"
#include "../../include/entities/fileTapes/BinaryFileTape.h"

#include <algorithm>
#include <exception>
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

TapeSorter::TapeSorter(size_t memoryLimit, TapeConfig config)
    : m_memoryLimit(memoryLimit), m_maxElements(memoryLimit / sizeof(int)),
      m_config(config), m_tmpDir("tmp") {}

void TapeSorter::sort(TapeInterface &input, TapeInterface &output) {
  std::vector<std::unique_ptr<TapeInterface>> temps;

  try {

    if (!fs::create_directories(m_tmpDir) && !fs::exists(m_tmpDir)) {
      throw std::runtime_error("Failed to create directory: " + m_tmpDir);
    }

    splitAndSort(input, temps);
    merge(output, temps);

    fs::remove_all(m_tmpDir);

  } catch (const std::exception &e) {
    fs::remove_all(m_tmpDir);
    throw std::runtime_error("[SORT]" + std::string(e.what()));
  }
}

void TapeSorter::splitAndSort(
    TapeInterface &input, std::vector<std::unique_ptr<TapeInterface>> &temps) {
  std::vector<int> buffer;
  input.rewind();

  while (!input.isAtEnd()) {
    buffer.clear();

    while (buffer.size() < m_maxElements && !input.isAtEnd()) {
      int x = input.read();
      buffer.push_back(x);
      input.moveRight();
    }

    std::sort(buffer.begin(), buffer.end());

    const std::string filename =
        m_tmpDir + "/temp_" + std::to_string(temps.size()) + ".bin";

    auto temp =
        std::make_unique<BinaryFileTape>(filename, m_memoryLimit, m_config);

    for (int num : buffer) {
      temp->write(num);
      temp->moveRight();
    }

    temp->rewind();
    temps.push_back(std::move(temp));
  }
}
void TapeSorter::merge(TapeInterface &output,
                       std::vector<std::unique_ptr<TapeInterface>> &temps) {

  while (temps.size() > 1) {
    std::vector<std::unique_ptr<TapeInterface>> newTemps;

    for (size_t i = 0; i < temps.size(); i += 2) {
      if (i + 1 >= temps.size()) {
        newTemps.push_back(std::move(temps[i]));
        continue;
      }

      const std::string filename = m_tmpDir + "/merge_" +
                                   std::to_string(temps.size()) + "_" +
                                   std::to_string(i / 2) + ".bin";

      size_t memoryLimitForMergeFile =
          ((temps[i]->getSize()) * sizeof(int)) +
          ((temps[i + 1]->getSize()) * sizeof(int));

      auto merged = std::make_unique<BinaryFileTape>(
          filename, memoryLimitForMergeFile, m_config);

      mergeTwo(*temps[i], *temps[i + 1], *merged);
      newTemps.push_back(std::move(merged));
    }
    temps = std::move(newTemps);
  }

  if (!temps.empty()) {
    temps.front()->rewind();
    output.rewind();

    size_t size = temps.front()->getSize();

    for (size_t i = 0; i < size; ++i) {
      output.write(temps.front()->read());
      output.moveRight();
      temps.front()->moveRight();
    }
  }
}

void TapeSorter::mergeTwo(TapeInterface &in1, TapeInterface &in2,
                          TapeInterface &out) {
  in1.rewind();
  in2.rewind();
  out.rewind();

  bool has1 = !in1.isAtEnd();
  bool has2 = !in2.isAtEnd();
  int val1 = has1 ? in1.read() : 0;
  int val2 = has2 ? in2.read() : 0;

  while (has1 && has2) {

    if (val1 <= val2) {
      out.write(val1);
      out.moveRight();
      in1.moveRight();
      has1 = !in1.isAtEnd();

      if (has1)
        val1 = in1.read();

    } else {
      out.write(val2);
      out.moveRight();
      in2.moveRight();
      has2 = !in2.isAtEnd();

      if (has2)
        val2 = in2.read();
    }
  }

  while (has1) {
    out.write(val1);
    out.moveRight();
    in1.moveRight();

    has1 = !in1.isAtEnd();

    if (has1)
      val1 = in1.read();
  }

  while (has2) {
    out.write(val2);
    out.moveRight();
    in2.moveRight();

    has2 = !in2.isAtEnd();

    if (has2)
      val2 = in2.read();
  }
}
