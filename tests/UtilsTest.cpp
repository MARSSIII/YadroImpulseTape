#include "../include/utils/utils.hpp"
#include "../include/entities/TapeConfig.h"
#include "../include/entities/fileTapes/BinaryFileTape.h"
#include "../include/entities/fileTapes/TextFileTape.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

class UtilsTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Создаем временные файлы для тестов
    fs::create_directory("test_temp_utils");

    std::ofstream("test_temp_utils/empty.txt");
    std::ofstream("test_temp_utils/test.bin");
    std::ofstream("test_temp_utils/no_extension");
    std::ofstream("test_temp_utils/multiple.dots.txt");
    std::ofstream("test_temp_utils/to_clear.txt");
  }

  void TearDown() override {
    // Удаляем временную директорию
    fs::remove_all("test_temp_utils");
  }

  TapeConfig config{10, 20, 30, 40};
};

// Тесты для validateExtensions
TEST_F(UtilsTest, ValidateExtensionsValidCases) {
  // Проверка допустимых комбинаций расширений
  EXPECT_NO_THROW(utils::validateExtensions(".txt", ".txt"));
  EXPECT_NO_THROW(utils::validateExtensions(".bin", ".bin"));
}

TEST_F(UtilsTest, ValidateExtensionsInvalidFormat) {
  // Проверка обработки неподдерживаемых форматов
  EXPECT_THROW(utils::validateExtensions(".csv", ".csv"), std::runtime_error);
  EXPECT_THROW(utils::validateExtensions(".dat", ".dat"), std::runtime_error);
}

TEST_F(UtilsTest, ValidateExtensionsMismatch) {
  // Проверка обнаружения несоответствия расширений
  EXPECT_THROW(utils::validateExtensions(".txt", ".bin"), std::runtime_error);
  EXPECT_THROW(utils::validateExtensions(".bin", ".txt"), std::runtime_error);
}

// Тесты для getFileExtension
TEST_F(UtilsTest, GetFileExtensionBasicCases) {
  // Проверка базовых случаев с обычными расширениями
  EXPECT_EQ(utils::getFileExtension("test_temp_utils/empty.txt"), ".txt");
  EXPECT_EQ(utils::getFileExtension("test_temp_utils/test.bin"), ".bin");
}

TEST_F(UtilsTest, GetFileExtensionCaseInsensitive) {
  // Проверка регистронезависимого определения расширения
  EXPECT_EQ(utils::getFileExtension("test_temp_utils/EMPTY.TXT"), ".txt");
  EXPECT_EQ(utils::getFileExtension("test_temp_utils/TEST.BIN"), ".bin");
}

TEST_F(UtilsTest, GetFileExtensionSpecialCases) {
  // Проверка особых случаев: файл без расширения и с несколькими точками
  EXPECT_EQ(utils::getFileExtension("test_temp_utils/no_extension"), "");
  EXPECT_EQ(utils::getFileExtension("test_temp_utils/multiple.dots.txt"),
            ".txt");
}

// Тесты для createTape
TEST_F(UtilsTest, CreateTapeTextFile) {
  // Проверка создания текстовой ленты
  auto tape =
      utils::createTape(40, config, "test_temp_utils/empty.txt", ".txt");
  EXPECT_NE(dynamic_cast<TextFileTape *>(tape.get()), nullptr);
}

TEST_F(UtilsTest, CreateTapeBinaryFile) {
  // Проверка создания бинарной ленты
  auto tape = utils::createTape(40, config, "test_temp_utils/test.bin", ".bin");
  EXPECT_NE(dynamic_cast<BinaryFileTape *>(tape.get()), nullptr);
}

TEST_F(UtilsTest, CreateTapeInvalidExtension) {
  // Проверка обработки неизвестного расширения
  EXPECT_THROW(utils::createTape(40, config, "test.txt", ".csv"),
               std::invalid_argument);
}

// Тесты для clearFile, использующие файлы из SetUp класса UtilsTest
TEST_F(UtilsTest, ClearExistingFile) {
  const std::string filename = "test_temp_utils/empty.txt";

  // Проверяем, что файл изначально пуст (соответствует SetUp)
  ASSERT_EQ(fs::file_size(filename), 0);

  // Для демонстрации добавим данные в файл
  {
    std::ofstream file(filename, std::ios::app);
    file << "test data";
    file.close();
    ASSERT_GT(fs::file_size(filename), 0); // Убедимся, что данные записаны
  }

  // Вызываем тестируемую функцию
  EXPECT_NO_THROW(utils::clearFile(filename));

  // Проверяем, что файл снова пуст
  EXPECT_EQ(fs::file_size(filename), 0);
}

TEST_F(UtilsTest, ClearBinaryFile) {
  const std::string filename = "test_temp_utils/test.bin";

  // Записываем данные в бинарный файл
  {
    std::ofstream file(filename, std::ios::binary | std::ios::app);
    file.write("binary\0data", 11);
    file.close();
    ASSERT_GT(fs::file_size(filename), 0);
  }

  // Очищаем файл
  EXPECT_NO_THROW(utils::clearFile(filename));

  // Проверяем результат
  EXPECT_EQ(fs::file_size(filename), 0);
}

TEST_F(UtilsTest, ClearFileWithMultipleDots) {
  const std::string filename = "test_temp_utils/multiple.dots.txt";

  // Добавляем данные
  {
    std::ofstream file(filename);
    file << "content";
    file.close();
    ASSERT_GT(fs::file_size(filename), 0);
  }

  // Очищаем
  EXPECT_NO_THROW(utils::clearFile(filename));

  // Проверяем
  EXPECT_EQ(fs::file_size(filename), 0);
}

TEST_F(UtilsTest, ClearNonExistentFileCreatesNew) {
  const std::string filename = "test_temp_utils/non_existent.file";

  // Убедимся, что файла нет
  ASSERT_FALSE(fs::exists(filename));

  // Вызываем функцию
  EXPECT_NO_THROW(utils::clearFile(filename));

  // Проверяем создание нового пустого файла
  EXPECT_TRUE(fs::exists(filename));
  EXPECT_EQ(fs::file_size(filename), 0);
}

TEST_F(UtilsTest, ClearFileThrowsWhenDirectory) {
  const std::string dirname = "test_temp_utils/empty_dir";

  // Создаем директорию в SetUp
  fs::create_directory(dirname);

  // Пытаемся очистить директорию
  EXPECT_THROW(
      {
        try {
          utils::clearFile(dirname);
        } catch (const std::runtime_error &e) {
          // Проверяем сообщение об ошибке
          EXPECT_STREQ(e.what(), ("Failed to clear file: " + dirname).c_str());
          throw;
        }
      },
      std::runtime_error);
}
