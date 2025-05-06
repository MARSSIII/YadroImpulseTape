#pragma once

#include "../../interfaces/TapeInterface.h"
#include "../TapeConfig.h"

#include <fstream>
#include <string>

/// @brief Ленточное устройство, работающее через бинарный файл.
/// @details Поддерживает чтение, запись, перемещение и имитацию задержек
/// операций.
class BinaryFileTape : public TapeInterface {
public:
  /// @brief Конструктор. Открывает или создает файл-ленту.
  /// @param filename Имя файла
  /// @param sizeTape Максимальный объём ленты (в байтах)
  /// @param config Конфигурация ленты (задержки)
  BinaryFileTape(const std::string &filename, const size_t sizeTape,
                 const TapeConfig &config);

  /// @brief Деструктор. Закрывает файл.
  ~BinaryFileTape() noexcept;

  /// @brief Запрет копирования, потому что нельзя "разделить" файловый поток
  /// между двумя объектами
  BinaryFileTape(const BinaryFileTape &) = delete;
  BinaryFileTape &operator=(const BinaryFileTape &) = delete;

  /// @brief Перемещающий конструктор BinaryFileTape
  /// @param other Rvalue-ссылка на передаваемый источник
  /// @details Перемещает все поля и ресурсы из other в новый объект.
  /// После перемещения объект-источник становится пустым и не владеет файлом.
  BinaryFileTape(BinaryFileTape &&other) noexcept;

  /// @brief Оператор перемещающего присваивания BinaryFileTape
  /// @param other Rvalue-ссылка на передаваемый источник
  /// @return *this
  /// @details Закрывает собственный файл (если открыт) и принимает ресурсы от
  /// other. После перемещения объект-источник становится пустым и не владеет
  /// файлом.
  BinaryFileTape &operator=(BinaryFileTape &&other) noexcept;

  /// @brief Читает int по текущей позиции ленты.
  /// @throws std::out_of_range При выходе за пределы записанных данных
  int read() final;

  /// @brief Записывает int по текущей позиции ленты.
  /// @param data Записываемое значение
  /// @throws std::out_of_range Если превышен размер ленты или некорректная
  /// позиция
  void write(int data) final;

  /// @brief Сдвигает головку ленты влево на один элемент.
  void moveLeft() final;

  /// @brief Сдвигает головку ленты вправо на один элемент.
  void moveRight() final;

  /// @brief Перемещает головку в начало ленты.
  void rewind() final;

  /// @brief Возвращает true, если вся записанная лента уже считана.
  bool isAtEnd() const final;

  /// @brief Количество элементов, записанных на ленту.
  size_t getSize() const final;

  /// @brief Максимальная емкость ленты (количество int).
  size_t getMaxSize() const;

  /// @brief Возвращает имя файла ленты.
  std::string getFilename() const;

private:
  size_t m_currentPosition; ///< Текущая позиция головки
  size_t m_size;            ///< Количество элементов на ленте

  size_t m_maxSize; ///< Максимальное количество элементов

  std::fstream m_file;    ///< Файловый поток
  std::string m_filename; ///< Имя файла

  TapeConfig m_config; ///< Конфигурация (задержки)

  /// @brief Обновляет m_size на основе размера файла.
  void updateSize();

  /// @brief Выполняет задержку операции.
  /// @param delay Задержка в миллисекундах
  void applyDelay(int delay) const;
};
