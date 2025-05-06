#pragma once

#include "../entities/TapeConfig.h"

/// @brief Абстрактная фабрика для создания конфигурации магнитной ленты
/// @details Определяет интерфейс для создания объектов TapeConfig.
/// Наследующие классы должны реализовать конкретные способы создания
/// конфигурации
class TapeConfigFactoryInterface {
public:
  virtual ~TapeConfigFactoryInterface() = default;

  /// @brief Создает объект конфигурации магнитной ленты
  /// @return Заполненный объект TapeConfig
  virtual TapeConfig create() const = 0;
};
