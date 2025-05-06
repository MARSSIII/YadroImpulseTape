#include "../../include/utils/utils.hpp"
#include "../../include/entities/TapeConfig.h"
#include "../../include/entities/fileTapes/BinaryFileTape.h"
#include "../../include/entities/fileTapes/TextFileTape.h"
#include "../../include/interfaces/TapeInterface.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace fs = std::filesystem;

/// @brief Проверяет расширения входного и выходного файлов.
///
/// Реализация проверяет наличие inputExt в списке разрешенных расширений
/// (allowedExts) и равенство inputExt и outputExt.
///
/// @param inputExt Расширение входного файла (в нижнем регистре).
/// @param outputExt Расширение выходного файла (в нижнем регистре).
/// @throw std::runtime_error При несоответствии форматов или несовпадении
/// расширений.
void utils::validateExtensions(const std::string &inputExt,
                               const std::string &outputExt) {
  // Список поддерживаемых расширений
  const std::vector<std::string> allowedExts = {".txt", ".bin"};

  // Проверка, есть ли расширение входного файла в списке разрешенных
  if (std::find(allowedExts.begin(), allowedExts.end(), inputExt) ==
      allowedExts.end()) {
    throw std::runtime_error("Unsupported input format: " + inputExt +
                             ". Allowed formats: .txt, .bin");
  }

  // Проверка на совпадение расширений входного и выходного файлов
  if (inputExt != outputExt) {
    throw std::runtime_error("Extension mismatch. Input: " + inputExt +
                             ", Output: " + outputExt);
  }

  // Если проверки пройдены, функция просто завершается
}

/// @brief Извлекает расширение файла из полного имени файла.
///
/// Реализация использует std::filesystem::path для безопасного извлечения
/// расширения и std::transform для приведения его к нижнему регистру.
///
/// @param filename Полное имя файла или путь к файлу.
/// @return std::string Расширение файла в нижнем регистре (например, ".txt")
///                     или пустая строка, если расширения нет.
std::string utils::getFileExtension(const std::string &filename) {

  // Создаем объект path из строки имени файла
  fs::path path(filename);

  // Получаем расширение как строку
  std::string ext = path.extension().string();

  // Преобразуем строку расширения в нижний регистр
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  return ext;
}

/// @brief Создает объект ленты (`TapeInterface`) нужного типа.
///
/// Реализация проверяет значение `ext` и создает соответствующий
/// тип файловой ленты с помощью std::make_unique.
///
/// @param config Конфигурация ленты.
/// @param filename Имя файла для ленты.
/// @param ext Расширение файла (".txt" или ".bin").
/// @return std::unique_ptr<TapeInterface> Указатель на созданную ленту.
/// @throw std::invalid_argument Если переданное расширение `ext` не
/// является ни ".txt", ни ".bin".
std::unique_ptr<TapeInterface> utils::createTape(const size_t maxSize,
                                                 const TapeConfig &config,
                                                 const std::string &filename,
                                                 const std::string &ext) {

  // Если расширение ".txt", создаем текстовую ленту
  if (ext == ".txt") {
    return std::make_unique<TextFileTape>(filename, config);
  }

  // Если расширение ".bin", создает ленту бинарного файла
  if (ext == ".bin") {
    return std::make_unique<BinaryFileTape>(filename, maxSize, config);
  }

  // Если расширение не соответствует ни одному из поддерживаемых типов
  // Бросаем исключение, указывая на недопустимый аргумент
  throw std::invalid_argument(
      "Unknown or unsupported file extension passed to createTape: '" + ext +
      "'. Expected '.txt' or '.bin'.");
}

/// @brief Очищает содержимое указанного файла.
///
/// Реализация открывает файл с флагом std::ios::trunc, который
/// очищает файл при открытии. Также проверяется успешность открытия файла.
///
/// @param filename Имя файла для очистки.
/// @throw std::runtime_error Если файл не удалось открыть/очистить.
void utils::clearFile(const std::string &filename) {

  // Открываем файл для записи с флагом усечения
  std::ofstream file(filename, std::ios::trunc);

  // Проверяем, успешно ли открылся файл
  if (!file) { // Оператор ! перегружен для потоков для проверки состояния
               // ошибки
    throw std::runtime_error("Failed to clear file: " + filename);
  }

  // Файл будет автоматически закрыт при выходе из функции (RAII для
  // std::ofstream)
}
