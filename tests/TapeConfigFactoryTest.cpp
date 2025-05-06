#include "../include/factories/TapeConfigFactory.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

namespace fs = std::filesystem;

class TapeConfigFactoryTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Создаем временные файлы для тестов
    fs::create_directory("test_temp_ConfigFactory");

    std::ofstream("test_temp_ConfigFactory/empty.txt");
    std::ofstream("test_temp_ConfigFactory/test.bin");
    std::ofstream("test_temp_ConfigFactory/no_extension");
    std::ofstream("test_temp_ConfigFactory/multiple.dots.txt");
  }

  void TearDown() override {
    // Удаляем временную директорию
    fs::remove_all("test_temp_ConfigFactory");
  }
};

TEST_F(TapeConfigFactoryTest, CorrectConfig) {
  // Создание корректного конфигурационного файла
  {
    std::ofstream file("test_config.cfg");
    file << "read_delay = 10\n";
    file << "write_delay = 20\n";
    file << "rewind_delay = 30\n";
    file << "shift_delay = 40\n";
  }

  TapeConfigFactory factory("test_config.cfg");
  TapeConfig config = factory.create();

  EXPECT_EQ(config.readDelay, 10);
  EXPECT_EQ(config.writeDelay, 20);
  EXPECT_EQ(config.rewindDelay, 30);
  EXPECT_EQ(config.shiftDelay, 40);
}

TEST_F(TapeConfigFactoryTest, EmptyFile) {
  // Создание пустого файла
  {
    std::ofstream file("empty_config.cfg");
  }

  TapeConfigFactory factory("empty_config.cfg");
  TapeConfig config = factory.create();

  EXPECT_EQ(config.readDelay, 0);
  EXPECT_EQ(config.writeDelay, 0);
  EXPECT_EQ(config.rewindDelay, 0);
  EXPECT_EQ(config.shiftDelay, 0);
}

TEST_F(TapeConfigFactoryTest, CommentsAndWhitespace) {
  // Файл с комментариями и пробелами
  {
    std::ofstream file("comments_config.cfg");
    file << "# Comment line\n";
    file << "  read_delay   =   100  \n";
    file << "\twrite_delay=200\t\n";
    file << "   # Another comment\n";
    file << "rewind_delay=300\n";
    file << "shift_delay = 400\n";
  }

  TapeConfigFactory factory("comments_config.cfg");
  TapeConfig config = factory.create();

  EXPECT_EQ(config.readDelay, 100);
  EXPECT_EQ(config.writeDelay, 200);
  EXPECT_EQ(config.rewindDelay, 300);
  EXPECT_EQ(config.shiftDelay, 400);
}

TEST_F(TapeConfigFactoryTest, InvalidFormat) {
  // Отсутствие '='
  {
    std::ofstream file("invalid_format1.cfg");
    file << "read_delay 10\n";
    file.close();

    TapeConfigFactory factory("invalid_format1.cfg");
    EXPECT_THROW(factory.create(), std::runtime_error);
    std::filesystem::remove("invalid_format1.cfg");
  }

  // Несколько '='
  {
    std::ofstream file("invalid_format2.cfg");
    file << "read_delay=10=20\n";
    file.close();

    TapeConfigFactory factory("invalid_format2.cfg");
    EXPECT_THROW(factory.create(), std::runtime_error);
    std::filesystem::remove("invalid_format2.cfg");
  }

  // Неизвестный ключ
  {
    std::ofstream file("invalid_key.cfg");
    file << "unknown_key=123\n";
    file.close();

    TapeConfigFactory factory("invalid_key.cfg");
    EXPECT_THROW(factory.create(), std::runtime_error);
    std::filesystem::remove("invalid_key.cfg");
  }

  // Нечисловое значение
  {
    std::ofstream file("invalid_value.cfg");
    file << "read_delay=abc\n";
    file.close();

    TapeConfigFactory factory("invalid_value.cfg");
    EXPECT_THROW(factory.create(), std::runtime_error);
    std::filesystem::remove("invalid_value.cfg");
  }
}

TEST_F(TapeConfigFactoryTest, NegativeValues) {
  {
    std::ofstream file("negative_values.cfg");
    file << "read_delay=-5\n";
  }

  TapeConfigFactory factory("negative_values.cfg");
  EXPECT_THROW(factory.create(), std::runtime_error);
  std::filesystem::remove("negative_values.cfg");
}

TEST_F(TapeConfigFactoryTest, NonExistentFile) {
  TapeConfigFactory factory("non_existent.cfg");
  EXPECT_THROW(factory.create(), std::runtime_error);
}

TEST_F(TapeConfigFactoryTest, PartialConfig) {
  {
    std::ofstream file("partial_config.cfg");
    file << "read_delay=50\n";
    file << "shift_delay=60\n";
  }

  TapeConfigFactory factory("partial_config.cfg");
  TapeConfig config = factory.create();

  EXPECT_EQ(config.readDelay, 50);
  EXPECT_EQ(config.shiftDelay, 60);
  EXPECT_EQ(config.writeDelay, 0);
  EXPECT_EQ(config.rewindDelay, 0);
}

TEST_F(TapeConfigFactoryTest, DuplicateKeys) {
  {
    std::ofstream file("duplicate_keys.cfg");
    file << "read_delay=10\n";
    file << "read_delay=20\n";
  }

  TapeConfigFactory factory("duplicate_keys.cfg");
  TapeConfig config = factory.create();

  EXPECT_EQ(config.readDelay, 20);
}

TEST_F(TapeConfigFactoryTest, KeyWithSpaces) {
  {
    std::ofstream file("key_with_spaces.cfg");
    file << "read delay=10\n";
  }

  TapeConfigFactory factory("key_with_spaces.cfg");
  EXPECT_THROW(factory.create(), std::runtime_error);
}
