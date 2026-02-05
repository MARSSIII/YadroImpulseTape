#include "../include/utils/utils.hpp"
#include "../include/entities/TapeConfig.h"
#include "../include/entities/fileTapes/BinaryFileTape.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

class UtilsTest : public ::testing::Test {
protected:
  void SetUp() override {
    fs::create_directory("test_temp_utils");

    std::ofstream("test_temp_utils/empty.txt");
    std::ofstream("test_temp_utils/test.bin");
    std::ofstream("test_temp_utils/no_extension");
    std::ofstream("test_temp_utils/multiple.dots.txt");
    std::ofstream("test_temp_utils/to_clear.txt");
  }

  void TearDown() override { fs::remove_all("test_temp_utils"); }

  TapeConfig config{10, 20, 30, 40};
};

TEST_F(UtilsTest, ValidateExtensionsValidCases) {
  EXPECT_NO_THROW(utils::validateExtensions(".txt", ".txt"));
  EXPECT_NO_THROW(utils::validateExtensions(".bin", ".bin"));
}

TEST_F(UtilsTest, ValidateExtensionsInvalidFormat) {
  EXPECT_THROW(utils::validateExtensions(".csv", ".csv"), std::runtime_error);
  EXPECT_THROW(utils::validateExtensions(".dat", ".dat"), std::runtime_error);
}

TEST_F(UtilsTest, ValidateExtensionsMismatch) {
  EXPECT_THROW(utils::validateExtensions(".txt", ".bin"), std::runtime_error);
  EXPECT_THROW(utils::validateExtensions(".bin", ".txt"), std::runtime_error);
}

TEST_F(UtilsTest, GetFileExtensionBasicCases) {
  EXPECT_EQ(utils::getFileExtension("test_temp_utils/empty.txt"), ".txt");
  EXPECT_EQ(utils::getFileExtension("test_temp_utils/test.bin"), ".bin");
}

TEST_F(UtilsTest, GetFileExtensionCaseInsensitive) {
  EXPECT_EQ(utils::getFileExtension("test_temp_utils/EMPTY.TXT"), ".txt");
  EXPECT_EQ(utils::getFileExtension("test_temp_utils/TEST.BIN"), ".bin");
}

TEST_F(UtilsTest, GetFileExtensionSpecialCases) {
  EXPECT_EQ(utils::getFileExtension("test_temp_utils/no_extension"), "");
  EXPECT_EQ(utils::getFileExtension("test_temp_utils/multiple.dots.txt"),
            ".txt");
}

TEST_F(UtilsTest, CreateTapeBinaryFile) {
  auto tape = utils::createTape(40, config, "test_temp_utils/test.bin", ".bin");
  EXPECT_NE(dynamic_cast<BinaryFileTape *>(tape.get()), nullptr);
}

TEST_F(UtilsTest, CreateTapeInvalidExtension) {
  EXPECT_THROW(utils::createTape(40, config, "test.txt", ".csv"),
               std::invalid_argument);
}

TEST_F(UtilsTest, ClearExistingFile) {
  const std::string filename = "test_temp_utils/empty.txt";

  ASSERT_EQ(fs::file_size(filename), 0);

  {
    std::ofstream file(filename, std::ios::app);
    file << "test data";
    file.close();
    ASSERT_GT(fs::file_size(filename), 0); // Убедимся, что данные записаны
  }

  EXPECT_NO_THROW(utils::clearFile(filename));

  EXPECT_EQ(fs::file_size(filename), 0);
}

TEST_F(UtilsTest, ClearBinaryFile) {
  const std::string filename = "test_temp_utils/test.bin";

  {
    std::ofstream file(filename, std::ios::binary | std::ios::app);
    file.write("binary\0data", 11);
    file.close();
    ASSERT_GT(fs::file_size(filename), 0);
  }

  EXPECT_NO_THROW(utils::clearFile(filename));

  EXPECT_EQ(fs::file_size(filename), 0);
}

TEST_F(UtilsTest, ClearFileWithMultipleDots) {
  const std::string filename = "test_temp_utils/multiple.dots.txt";

  {
    std::ofstream file(filename);
    file << "content";
    file.close();
    ASSERT_GT(fs::file_size(filename), 0);
  }

  EXPECT_NO_THROW(utils::clearFile(filename));

  EXPECT_EQ(fs::file_size(filename), 0);
}

TEST_F(UtilsTest, ClearNonExistentFileCreatesNew) {
  const std::string filename = "test_temp_utils/non_existent.file";

  ASSERT_FALSE(fs::exists(filename));

  EXPECT_NO_THROW(utils::clearFile(filename));

  EXPECT_TRUE(fs::exists(filename));
  EXPECT_EQ(fs::file_size(filename), 0);
}

TEST_F(UtilsTest, ClearFileThrowsWhenDirectory) {
  const std::string dirname = "test_temp_utils/empty_dir";

  fs::create_directory(dirname);

  EXPECT_THROW(
      {
        try {
          utils::clearFile(dirname);
        } catch (const std::runtime_error &e) {
          EXPECT_STREQ(e.what(), ("Failed to clear file: " + dirname).c_str());
          throw;
        }
      },
      std::runtime_error);
}
