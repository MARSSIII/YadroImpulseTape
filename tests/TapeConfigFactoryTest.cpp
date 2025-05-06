#include "../include/factories/TapeConfigFactory.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

namespace fs = std::filesystem;

class TapeConfigFactoryTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Создаем временные файлы для тестов
    fs::create_directory("testTempConfigFactory");
  }

  void TearDown() override {
    // Удаляем временную директорию
    fs::remove_all("testTempConfigFactory");
  }
};

TEST_F(TapeConfigFactoryTest, CorrectConfig) {
  // Создание корректного конфигурационного файла
  const std::string filename = "testTempConfigFactory/testConfig.cfg";

  {
    std::ofstream file(filename);
    file << "read_delay = 10\n";
    file << "write_delay = 20\n";
    file << "rewind_delay = 30\n";
    file << "shift_delay = 40\n";
  }

  TapeConfigFactory factory(filename);
  TapeConfig config = factory.create();

  EXPECT_EQ(config.readDelay, 10);
  EXPECT_EQ(config.writeDelay, 20);
  EXPECT_EQ(config.rewindDelay, 30);
  EXPECT_EQ(config.shiftDelay, 40);
}

TEST_F(TapeConfigFactoryTest, EmptyFile) {
  // Создание пустого файла
  const std::string filename = "testTempConfigFactory/emptyConfig.cfg";

  {
    std::ofstream file(filename);
  }

  TapeConfigFactory factory(filename);
  TapeConfig config = factory.create();

  EXPECT_EQ(config.readDelay, 0);
  EXPECT_EQ(config.writeDelay, 0);
  EXPECT_EQ(config.rewindDelay, 0);
  EXPECT_EQ(config.shiftDelay, 0);
}

TEST_F(TapeConfigFactoryTest, CommentsAndWhitespace) {
  // Файл с комментариями и пробелами
  const std::string filename = "testTempConfigFactory/configWithComments";
  {
    std::ofstream file(filename);
    file << "# Comment line\n";
    file << "  read_delay   =   100  \n";
    file << "\twrite_delay=200\t\n";
    file << "   # Another comment\n";
    file << "rewind_delay=300\n";
    file << "shift_delay = 400\n";
    file << "memory_limit = 2000";
  }

  TapeConfigFactory factory(filename);
  TapeConfig config = factory.create();

  EXPECT_EQ(config.readDelay, 100);
  EXPECT_EQ(config.writeDelay, 200);
  EXPECT_EQ(config.rewindDelay, 300);
  EXPECT_EQ(config.shiftDelay, 400);
  EXPECT_EQ(config.memoryLimit, 2000);
}

TEST_F(TapeConfigFactoryTest, InvalidFormatNotAssignment) {
  // Отсутствие '='
  const std::string filename = "testTempConfigFactory/InvalidFormat1.cfg";
  {
    std::ofstream file(filename);
    file << "read_delay 10\n";
    file.close();
  }
  TapeConfigFactory factory(filename);
  EXPECT_THROW(factory.create(), std::runtime_error);
}

TEST_F(TapeConfigFactoryTest, InvalidFormatWithTwiceAssignment) {
  // Несколько '='
  const std::string filename = "testTempConfigFactory/InvalidFormat2";
  {
    std::ofstream file(filename);
    file << "read_delay=10=20\n";
    file.close();
  }

  TapeConfigFactory factory(filename);
  EXPECT_THROW(factory.create(), std::runtime_error);
}

TEST_F(TapeConfigFactoryTest, InvalidKey) {
  // Неизвестный ключ
  const std::string filename = "testTempConfigFactory/invalidKey.cfg";
  {
    std::ofstream file(filename);
    file << "unknown_key=123\n";
    file.close();
  }

  TapeConfigFactory factory(filename);
  EXPECT_THROW(factory.create(), std::runtime_error);
}

TEST_F(TapeConfigFactoryTest, InvalidValue) {
  // Нечисловое значение
  const std::string filename = "testTempConfigFactory/invalidValue";
  {
    std::ofstream file(filename);
    file << "read_delay=abc\n";
    file.close();
  }

  TapeConfigFactory factory(filename);
  EXPECT_THROW(factory.create(), std::runtime_error);
}

TEST_F(TapeConfigFactoryTest, NegativeValues) {
  const std::string filename = "testTempConfigFactory/negativeValues";

  {
    std::ofstream file(filename);
    file << "read_delay=-5\n";
  }

  TapeConfigFactory factory(filename);
  EXPECT_THROW(factory.create(), std::runtime_error);
}

TEST_F(TapeConfigFactoryTest, NonExistentFile) {
  const std::string filename = "testTempConfigFactory/nonExistentFile.cfg";

  TapeConfigFactory factory(filename);
  EXPECT_THROW(factory.create(), std::runtime_error);
}

TEST_F(TapeConfigFactoryTest, PartialConfig) {
  const std::string filename = "testTempConfigFactory/PartialConfig.cfg";

  {
    std::ofstream file(filename);
    file << "read_delay=50\n";
    file << "shift_delay=60\n";
  }

  TapeConfigFactory factory(filename);
  TapeConfig config = factory.create();

  EXPECT_EQ(config.readDelay, 50);
  EXPECT_EQ(config.shiftDelay, 60);
  EXPECT_EQ(config.writeDelay, 0);
  EXPECT_EQ(config.rewindDelay, 0);
  EXPECT_EQ(config.memoryLimit, 1024);
}

TEST_F(TapeConfigFactoryTest, DuplicateKeys) {
  const std::string filename = "testTempConfigFactory/duplicateKeys.cfg";

  {
    std::ofstream file(filename);
    file << "read_delay=10\n";
    file << "read_delay=20\n";
  }

  TapeConfigFactory factory(filename);
  TapeConfig config = factory.create();

  EXPECT_EQ(config.readDelay, 20);
}

TEST_F(TapeConfigFactoryTest, KeyWithSpaces) {
  const std::string filename = "testTempConfigFactory/KeyWithSpaces.cfg";

  {
    std::ofstream file(filename);
    file << "read delay=10\n";
  }

  TapeConfigFactory factory(filename);
  EXPECT_THROW(factory.create(), std::runtime_error);
}
