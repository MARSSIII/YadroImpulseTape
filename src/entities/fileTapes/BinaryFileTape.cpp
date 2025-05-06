#include "../../../include/entities/fileTapes/BinaryFileTape.h"

#include <chrono>
#include <stdexcept>
#include <thread>

/// @brief Конструктор BinaryFileTape. Открывает или создаёт файл для ленты.
/// @param filename Имя файла ленты.
/// @param sizeTape Максимальный размер файла (в байтах).
/// @param config Параметры конфигурации (задержки операций).
/// @throws std::runtime_error Если размер существующего файла превышает
/// максимум.
BinaryFileTape::BinaryFileTape(const std::string &filename,
                               const size_t sizeTape, const TapeConfig &config)
    : m_currentPosition(0), m_size(0), m_maxSize(sizeTape / sizeof(int)),
      m_filename(filename), m_config(config) {

  // Пробуем открыть файловый поток для чтения-записи
  m_file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

  // Если файла не существует - создаём пустой файл и открываем снова
  if (!m_file.is_open()) {
    m_file.clear();
    m_file.open(filename, std::ios::out);
    m_file.close();
    m_file.open(filename, std::ios::in | std::ios::out | std::ios::ate |
                              std::ios::binary);
  }

  updateSize();

  if (m_size > m_maxSize) {
    throw std::runtime_error("File size exceeds maximum allowed size");
  }
}

/// @brief Деструктор BinaryFileTape. Закрывает файловый поток.
BinaryFileTape::~BinaryFileTape() noexcept {
  if (m_file.is_open())
    m_file.close();
}

/// @brief Перемещающий конструктор BinaryFileTape
/// @param other Rvalue-ссылка на передаваемый источник
/// @details Перемещает все поля и ресурсы из other в новый объект.
/// После перемещения объект-источник становится пустым и не владеет файлом.
BinaryFileTape::BinaryFileTape(BinaryFileTape &&other) noexcept
    : m_currentPosition(other.m_currentPosition), m_size(other.m_size),
      m_maxSize(other.m_maxSize), m_file(std::move(other.m_file)),
      m_filename(std::move(other.m_filename)),
      m_config(std::move(other.m_config)) {
  // Сброс полей у источника, чтобы избежать двойного владения
  other.m_currentPosition = 0;
  other.m_size = 0;
  other.m_maxSize = 0;
}

/// @brief Оператор перемещающего присваивания BinaryFileTape
/// @param other Rvalue-ссылка на передаваемый источник
/// @return *this
/// @details Закрывает собственный файл (если открыт) и принимает ресурсы от
/// other. После перемещения объект-источник становится пустым и не владеет
/// файлом.
BinaryFileTape &BinaryFileTape::operator=(BinaryFileTape &&other) noexcept {
  if (this != &other) {
    if (m_file.is_open())
      m_file.close();
    m_currentPosition = other.m_currentPosition;
    m_size = other.m_size;
    m_maxSize = other.m_maxSize;
    m_file = std::move(other.m_file);
    m_filename = std::move(other.m_filename);
    m_config = std::move(other.m_config);

    other.m_currentPosition = 0;
    other.m_size = 0;
    other.m_maxSize = 0;
  }
  return *this;
}

/// @brief Обновляет значение m_size на основе текущего размера файла.
void BinaryFileTape::updateSize() {
  m_file.seekg(0, std::ios::end);

  int bytes = m_file.tellg();
  m_size = bytes / sizeof(int);

  m_file.seekg(m_currentPosition * sizeof(int));
}

/// @brief Считывает int с текущей позиции ленты.
/// @return Считанное значение.
/// @throws std::out_of_range Если позиция головы вне диапазона записанных
/// данных.
int BinaryFileTape::read() {
  if (m_currentPosition >= m_size)
    throw std::out_of_range("Read position out of range");

  applyDelay(m_config.readDelay);

  m_file.seekg(m_currentPosition * sizeof(int));

  int value;
  m_file.read(reinterpret_cast<char *>(&value), sizeof(int));

  return value;
}

/// @brief Записывает int по текущей позиции ленты.
/// @param data Записываемое значение.
/// @throws std::out_of_range Если позиция головы превышает m_maxSize или
/// некорректна.
void BinaryFileTape::write(int data) {
  applyDelay(m_config.writeDelay);

  if (m_currentPosition >= m_maxSize) {
    throw std::out_of_range("Write position exceeds maximum size");
  }

  if (m_currentPosition > m_size) {
    throw std::out_of_range("Write position out of range");
  }

  m_file.seekp(m_currentPosition * sizeof(int));
  m_file.write(reinterpret_cast<char *>(&data), sizeof(int));
  m_file.flush();

  // Если записали новый элемент - увеличиваем размер ленты
  if (m_currentPosition == m_size) {
    m_size++;
    updateSize();
  }
}

/// @brief Сдвигает головку ленты влево на один элемент.
/// @details Если головка в начале — ничего не происходит.
void BinaryFileTape::moveLeft() {
  if (m_currentPosition > 0) {

    applyDelay(m_config.shiftDelay);

    m_currentPosition--;
    m_file.seekg(m_currentPosition * sizeof(int));
  }
}

/// @brief Сдвигает головку ленты вправо на один элемент.
/// @details Если за пределами ленты — ничего не происходит.
void BinaryFileTape::moveRight() {
  if (m_currentPosition < m_maxSize || m_currentPosition > m_size) {
    applyDelay(m_config.shiftDelay);

    m_currentPosition++;
    m_file.seekg(m_currentPosition * sizeof(int));
  }
}

/// @brief Перемещает головку ленты в начало.
/// @details После вызова позиция становится 0.
void BinaryFileTape::rewind() {
  applyDelay(m_config.rewindDelay);

  m_currentPosition = 0;
  m_file.seekg(0);
}

/// @brief Проверяет, достигнут ли конец ленты.
/// @return true, если головка находится за последним элементом.
bool BinaryFileTape::isAtEnd() const { return m_currentPosition >= m_size; }

/// @brief Возвращает количество элементов, записанных на ленту.
/// @return Текущее количество элементов.
size_t BinaryFileTape::getSize() const { return m_size; }

/// @brief Возвращает ёмкость ленты в элементах.
/// @return Максимальное количество int, которое можно записать.
size_t BinaryFileTape::getMaxSize() const { return m_maxSize; }

/// @brief Возвращает имя файла, связанного с лентой.
/// @return Имя файла.
std::string BinaryFileTape::getFilename() const { return m_filename; }

/// @brief Делает паузу на время, заданное параметром.
/// @param delay Задержка в миллисекундах.
void BinaryFileTape::applyDelay(int delay) const {
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}
