#include "../include/entities/fileTapes/BinaryFileTape.h"

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

namespace fs = std::filesystem;

class BinaryFileTapeTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Создаем временную директорию
    fs::create_directory(tmpDir);
  }

  void TearDown() override {
    // Удаляем временную директорию
    fs::remove_all(tmpDir);
  }

  const std::string tmpDir = "testTempBinaryFileTapeTest";

  TapeConfig config{10, 20, 30, 40};
};

TEST_F(BinaryFileTapeTest, CreateEmptyFile) {
  const std::string filename = tmpDir + "/testEmpty.bin";

  BinaryFileTape tape(filename, 40, config);
  ASSERT_EQ(tape.getSize(), 0);
  ASSERT_TRUE(tape.isAtEnd());
}

TEST_F(BinaryFileTapeTest, WriteAndRead) {
  const std::string filename = tmpDir + "/testWriteAndRead.bin";

  BinaryFileTape tape(filename, 40, config);

  tape.write(123);

  ASSERT_EQ(tape.read(), 123);
  ASSERT_EQ(tape.getSize(), 1);
}

TEST_F(BinaryFileTapeTest, ReadFromExistingFile) {
  // Создаем файл с данными

  const std::string filename = tmpDir + "/testExisting.bin";

  {
    std::ofstream file(filename, std::ios::binary);
    int data[] = {10, 20, 30};
    file.write(reinterpret_cast<const char *>(data), sizeof(data));
  }

  BinaryFileTape tape(filename, 40, config);
  ASSERT_EQ(tape.getSize(), 3);

  ASSERT_EQ(tape.read(), 10);

  tape.moveRight();
  ASSERT_EQ(tape.read(), 20);

  tape.moveRight();
  ASSERT_EQ(tape.read(), 30);

  tape.moveRight();
  ASSERT_TRUE(tape.isAtEnd());
}

TEST_F(BinaryFileTapeTest, OverwriteData) {
  const std::string filename = tmpDir + "/testOverwrite.bin";

  {
    BinaryFileTape tape(filename, 40, config);

    tape.write(1);
    tape.moveRight();

    tape.write(2);
    tape.moveRight();

    tape.write(3);
    tape.moveRight();
  }

  BinaryFileTape tape(filename, 40, config);

  tape.moveRight(); // Перемещаемся к позиции 1 (значение 2)
  tape.write(99);   // Перезаписываем
  tape.rewind();

  ASSERT_EQ(tape.read(), 1);

  tape.moveRight();
  ASSERT_EQ(tape.read(), 99);

  tape.moveRight();
  ASSERT_EQ(tape.read(), 3);
}

TEST_F(BinaryFileTapeTest, MoveLeftAndRight) {
  const std::string filename = tmpDir + "/testMove.bin";

  BinaryFileTape tape(filename, 40, config);

  tape.write(100);
  tape.moveRight();

  tape.write(200);
  tape.rewind();

  ASSERT_EQ(tape.read(), 100);

  tape.moveRight();
  ASSERT_EQ(tape.read(), 200);

  tape.moveLeft();
  ASSERT_EQ(tape.read(), 100);
}

TEST_F(BinaryFileTapeTest, ReadOutOfRange) {
  const std::string filename = tmpDir + "/testEmpty.bin";

  BinaryFileTape tape(filename, 40, config);

  tape.write(42);
  tape.moveRight(); // Позиция 1 (за пределами размера)
  ASSERT_THROW(tape.read(), std::out_of_range);
}

TEST_F(BinaryFileTapeTest, WriteAtEndIncrementsSize) {
  const std::string filename = tmpDir + "/testEmpty.bin";

  BinaryFileTape tape(filename, 40, config);
  ASSERT_EQ(tape.getSize(), 0);

  tape.write(1); // size=1
  ASSERT_EQ(tape.getSize(), 1);

  tape.moveRight();

  tape.write(2); // size=2
  ASSERT_EQ(tape.getSize(), 2);
}

TEST_F(BinaryFileTapeTest, RewindResetsPosition) {
  const std::string filename = tmpDir + "/testEmpty.bin";

  BinaryFileTape tape(filename, 40, config);

  tape.write(1);
  tape.moveRight();

  tape.write(2);
  tape.rewind();
  ASSERT_EQ(tape.read(), 1);
}

TEST_F(BinaryFileTapeTest, MoveBeyondBoundsDoesNothing) {
  const std::string filename = tmpDir + "/testEmpty.bin";

  BinaryFileTape tape(filename, 40, config);

  tape.write(1);
  tape.rewind();

  // Попытка сдвинуться влево от позиции 0
  tape.moveLeft();
  ASSERT_EQ(tape.read(), 1);

  // Переместиться в конец и попытаться сдвинуться вправо
  tape.moveRight();
  ASSERT_TRUE(tape.isAtEnd());

  tape.moveRight(); // Ничего не происходит
  ASSERT_TRUE(tape.isAtEnd());
}
