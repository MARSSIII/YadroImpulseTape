#include "../../include/factories/TapeConfigFactory.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>

using namespace std::string_view_literals;

// Вспомогательная функция для удаления пробельных символов с обеих сторон
// строки
void trimWhitespace(std::string &s) {
  constexpr auto whitespace = " \t\n\r\f\v"sv;
  s.erase(s.find_last_not_of(whitespace) + 1);
  s.erase(0, s.find_first_not_of(whitespace));
}

TapeConfigFactory::TapeConfigFactory(std::string filename)
    : m_configFile(std::move(filename)) {}

TapeConfig TapeConfigFactory::create() const {
  TapeConfig config;

  if (m_configFile.empty()) {
    return config;
  }

  std::ifstream file(m_configFile);

  if (!file.is_open()) {
    throw std::runtime_error("[CONFIG] Failed to open config file: " +
                             m_configFile);
  }

  std::string line;
  int lineNumber = 0;
  while (std::getline(file, line)) {
    ++lineNumber;

    try {
      parseLine(line, config);
    } catch (const std::exception &e) {
      throw std::runtime_error("[CONFIG] Error in line " +
                               std::to_string(lineNumber) + ": " + e.what());
    }
  }

  validateConfig(config);
  return config;
}

/// @brief Обрабатывает строку конфигурации формата "ключ=значение"
/// @details Поддерживаемые ключи (регистрозависимые):
///   read_delay    - задержка чтения (мс)
///   write_delay   - задержка записи (мс)
///   rewind_delay  - задержка перемотки (мс)
///   shift_delay   - задержка перемещения головки (мс)
/// Пробелы вокруг ключа и значения игнорируются
void TapeConfigFactory::parseLine(const std::string &line,
                                  TapeConfig &config) const {
  std::string trimmed = line;
  trimWhitespace(trimmed);

  if (trimmed.empty() || trimmed[0] == '#') {
    return;
  }

  size_t equals_count = std::count(trimmed.begin(), trimmed.end(), '=');
  if (equals_count != 1) {
    throw std::runtime_error("Invalid format: expected exactly one '='");
  }

  std::istringstream iss(trimmed);
  std::string key;

  if (!std::getline(iss, key, '=')) {
    throw std::runtime_error("Invalid format: missing key");
  }

  trimWhitespace(key);
  std::string valueStr;
  if (!std::getline(iss, valueStr)) {
    throw std::runtime_error("Missing value");
  }

  trimWhitespace(valueStr);

  int value;
  try {
    value = std::stoi(valueStr);
  } catch (const std::exception &) {
    throw std::runtime_error("Invalid integer value: " + valueStr);
  }

  if (key == "read_delay") {
    config.readDelay = value;
  } else if (key == "write_delay") {
    config.writeDelay = value;
  } else if (key == "rewind_delay") {
    config.rewindDelay = value;
  } else if (key == "shift_delay") {
    config.shiftDelay = value;
  } else if (key == "memory_limit") {
    config.memoryLimit = value;
  } else {
    throw std::runtime_error("Unknown config key: " + key);
  }
}

/// @brief Проверяет что все параметры конфигурации неотрицательные
/// @throws std::runtime_error Если найдено отрицательное значение
void TapeConfigFactory::validateConfig(const TapeConfig &config) const {
  const auto checkNegative = [](int value, const std::string &name) -> void {
    if (value < 0) {
      throw std::runtime_error(name + " cannot be negative");
    }
  };

  checkNegative(config.readDelay, "Read delay");
  checkNegative(config.writeDelay, "Write delay");
  checkNegative(config.rewindDelay, "Rewind delay");
  checkNegative(config.shiftDelay, "Shift delay");
}
