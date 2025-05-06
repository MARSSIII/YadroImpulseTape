#include "../../include/entities/TapeSorter.hpp"
#include <algorithm>
#include <stdexcept>
namespace fs = std::filesystem;

template <TapeInterfaceDerived TapeT>
TapeSorter<TapeT>::TapeSorter(size_t memoryLimit, const TapeConfig& configT)
    : m_memoryLimit(memoryLimit),
      m_maxElements(memoryLimit / sizeof(int)),
      m_tmpDir("tmp"),
      m_config(configT) {}

template <TapeInterfaceDerived TapeT>
TapeSorter<TapeT>::~TapeSorter() {
  cleanupTempFiles();
}

template <TapeInterfaceDerived TapeT>
void TapeSorter<TapeT>::cleanupTempFiles() {
  if (fs::exists(m_tmpDir)) {
    fs::remove_all(m_tmpDir);
  }
}

template <TapeInterfaceDerived TapeT>
void TapeSorter<TapeT>::sort(TapeT &input, TapeT &output) {
  std::vector<std::unique_ptr<TapeT>> temps;
  if (!fs::create_directories(m_tmpDir) && !fs::exists(m_tmpDir)) {
    throw std::runtime_error("Failed to create directory: " + m_tmpDir);
  }
  splitAndSort(input, temps);
  merge(output, temps);
  cleanupTempFiles();
}

template <TapeInterfaceDerived TapeT>
void TapeSorter<TapeT>::splitAndSort(
    TapeT &input, std::vector<std::unique_ptr<TapeT>> &temps) {
  std::vector<int> buffer;
  input.rewind();
  while (!input.isAtEnd()) {
    buffer.clear();
    while (buffer.size() < m_maxElements && !input.isAtEnd()) {
      buffer.push_back(input.read());
      input.moveRight();
    }
    std::sort(buffer.begin(), buffer.end());
    const std::string filename =
        m_tmpDir + "/temp_" + std::to_string(temps.size());
    auto temp = std::make_unique<TapeT>(filename, m_config);
    for (int num : buffer) {
      temp->write(num);
      temp->moveRight();
    }
    temp->rewind();
    temps.push_back(std::move(temp));
  }
}

template <TapeInterfaceDerived TapeT>
void TapeSorter<TapeT>::merge(TapeT &output,
                              std::vector<std::unique_ptr<TapeT>> &temps) {
  while (temps.size() > 1) {
    std::vector<std::unique_ptr<TapeT>> newTemps;
    for (size_t i = 0; i < temps.size(); i += 2) {
      if (i + 1 >= temps.size()) {
        newTemps.push_back(std::move(temps[i]));
        continue;
      }
      const std::string filename =
          m_tmpDir + "/merge_" + std::to_string(temps.size());
      auto merged = std::make_unique<TapeT>(filename, m_config);
      mergeTwo(*temps[i], *temps[i + 1], *merged);
      merged->rewind();
      newTemps.push_back(std::move(merged));
    }
    temps = std::move(newTemps);
  }
  if (!temps.empty()) {
    temps.front()->rewind();
    output.rewind();
    while (!temps.front()->isAtEnd()) {
      output.write(temps.front()->read());
      output.moveRight();
      temps.front()->moveRight();
    }
  }
}

template <TapeInterfaceDerived TapeT>
void TapeSorter<TapeT>::mergeTwo(TapeT &in1, TapeT &in2, TapeT &out) {
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
