#include <algorithm>
#include <filesystem>
#include <gtest/gtest.h>
#include <random>
#include <vector>

#include "../include/entities/TapeSorter.h"
#include "../include/entities/fileTapes/BinaryFileTape.h"

namespace fs = std::filesystem;

class TapeSorterTest : public ::testing::Test {
protected:
  void SetUp() override { fs::create_directories(tempDir); }

  void TearDown() override { fs::remove_all(tempDir); }

  const std::string tempDir = "tape_sorter_test_tmp";
  ;
};

/// @brief Создаёт заполненный ленту с числами из data
std::unique_ptr<BinaryFileTape> makeTape(const std::string &file,
                                         const std::vector<int> &data,
                                         const TapeConfig &cfg) {
  // max размер - на весь data * sizeof(int)
  auto tape =
      std::make_unique<BinaryFileTape>(file, data.size() * sizeof(int), cfg);

  for (size_t i = 0; i < data.size(); ++i) {

    tape->write(data[i]);
    if (i + 1 < data.size())
      tape->moveRight();
  }

  tape->rewind();
  return tape;
}

/// @brief Считывает все значения из ленты, от позиции 0
std::vector<int> readTape(BinaryFileTape &tape) {
  std::vector<int> out;

  tape.rewind();

  size_t n = tape.getSize();
  for (size_t i = 0; i < n; ++i) {
    out.push_back(tape.read());
    tape.moveRight();
  }

  return out;
}

TEST_F(TapeSorterTest, SimpleSortWorks) {
  // Сортировка небольшого массива
  TapeConfig cfg{0, 0, 0, 0};

  std::vector<int> vec{5, 1, 3, 8, -2, 0, 5};
  std::string inputFile = tempDir + "/input.bin";
  std::string outputFile = tempDir + "/output.bin";

  auto inputTape = makeTape(inputFile, vec, cfg);
  BinaryFileTape outputTape(outputFile, vec.size() * sizeof(int), cfg);

  TapeSorter sorter(vec.size() * sizeof(int), cfg);
  sorter.sort(*inputTape, outputTape);

  auto result = readTape(outputTape);

  std::vector<int> sortedExpected = vec;
  std::sort(sortedExpected.begin(), sortedExpected.end());

  ASSERT_EQ(result, sortedExpected);
}

TEST_F(TapeSorterTest, AlreadySorted) {
  TapeConfig cfg{0, 0, 0, 0};
  std::vector<int> vec{-3, 0, 4, 8, 11, 22};
  std::string inputFile = tempDir + "/input2.bin";
  std::string outputFile = tempDir + "/output2.bin";

  auto inputTape = makeTape(inputFile, vec, cfg);
  BinaryFileTape outputTape(outputFile, vec.size() * sizeof(int), cfg);

  TapeSorter sorter(vec.size() * sizeof(int), cfg);
  sorter.sort(*inputTape, outputTape);

  auto result = readTape(outputTape);
  ASSERT_EQ(result, vec);
}

TEST_F(TapeSorterTest, ReverseSorted) {
  TapeConfig cfg{0, 0, 0, 0};
  std::vector<int> vec{9, 5, 4, 2, 0, -2, -10};
  std::string inputFile = tempDir + "/input3.bin";
  std::string outputFile = tempDir + "/output3.bin";

  auto inputTape = makeTape(inputFile, vec, cfg);
  BinaryFileTape outputTape(outputFile, vec.size() * sizeof(int), cfg);

  TapeSorter sorter(vec.size() * sizeof(int), cfg);
  sorter.sort(*inputTape, outputTape);

  std::vector<int> expected = vec;
  std::sort(expected.begin(), expected.end());
  auto result = readTape(outputTape);

  ASSERT_EQ(result, expected);
}

TEST_F(TapeSorterTest, LargeRandom) {
  // Тестируем сортировку большого файла, с ограничением памяти
  TapeConfig cfg{0, 0, 0, 0};
  size_t N = 500; // Можно увеличить для проверок
  std::vector<int> vec(N);

  // Инициализируем случайными значениями
  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(-10000, 10000);
  for (auto &v : vec)
    v = dist(rng);

  std::string inputFile = tempDir + "/input_large.bin";
  std::string outputFile = tempDir + "/output_large.bin";

  auto inputTape = makeTape(inputFile, vec, cfg);
  BinaryFileTape outputTape(outputFile, N * sizeof(int), cfg);

  // memoryLimit меньше полного размера файла -> будет несколько фрагментов по
  // ходу сортировки
  TapeSorter sorter(N * sizeof(int) / 10, cfg);
  sorter.sort(*inputTape, outputTape);

  std::vector<int> expected = vec;
  std::sort(expected.begin(), expected.end());
  auto result = readTape(outputTape);

  ASSERT_EQ(result, expected);
}
