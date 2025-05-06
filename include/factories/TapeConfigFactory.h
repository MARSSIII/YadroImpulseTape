#pragma once

#include "../interfaces/TapeConfigFactoryInterface.h"
#include <string>

/// @brief Фабрика конфигурации ленты из файла
/// @details Реализует создание конфигурации путем чтения параметров из
/// текстового файла. Формат файла: ключ=значение, одна пара на строку.
/// Поддерживаемые ключи: read_delay, write_delay, rewind_delay, shift_delay
/// Все значения должны быть целыми неотрицательными числами
class TapeConfigFactory : public TapeConfigFactoryInterface {
public:
  /// @brief Конструктор фабрики
  /// @param filename Путь к файлу конфигурации
  explicit TapeConfigFactory(std::string filename);

  /// @brief Создает конфигурацию на основе данных из файла
  /// @return Заполненный объект TapeConfig
  /// @throw std::runtime_error При ошибках чтения файла или невалидных данных
  TapeConfig create() const final;

private:
  std::string m_configFile; ///< Путь к файлу конфигурации

  /// @brief Парсит строку конфигурации и обновляет объект конфига
  /// @param line Обрабатываемая строка из файла
  /// @param config Объект конфигурации для заполнения
  /// @throw std::runtime_error При неверном формате строки или значении
  void parseLine(const std::string &line, TapeConfig &config) const;

  /// @brief Проверяет корректность заполненной конфигурации
  /// @param config Проверяемый объект конфигурации
  /// @throw std::runtime_error При наличии отрицательных значений
  void validateConfig(const TapeConfig &config) const;
};
