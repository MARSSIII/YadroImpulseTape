#pragma once

// #include "../../include/entities/TapeSorter.hpp"

#include <memory>
#include <string>

// Предварительные объявления классов, чтобы избежать циклической зависимости
class TapeConfig;
class TapeInterface;

namespace utils {

/// @brief Проверяет расширения входного и выходного файлов.
///
/// Функция проверяет, является ли расширение входного файла поддерживаемым
/// (например, ".txt" или ".bin") и совпадает ли оно с расширением выходного
/// файла.
///
/// @param inputExt Расширение входного файла (в нижнем регистре, например,
/// ".txt").
/// @param outputExt Расширение выходного файла (в нижнем регистре, например,
/// ".txt").
/// @throw std::runtime_error Если расширение входного файла не поддерживается
///                           или если расширения не совпадают.
void validateExtensions(const std::string &inputExt,
                        const std::string &outputExt);

/// @brief Извлекает расширение файла из полного имени файла.
///
/// Использует std::filesystem для получения расширения и преобразует его
/// в нижний регистр для унификации.
///
/// @param filename Полное имя файла или путь к файлу.
/// @return std::string Расширение файла в нижнем регистре (включая точку,
/// например, ".txt")
///                     или пустая строка, если расширение отсутствует.
std::string getFileExtension(const std::string &filename);

/// @brief Создает объект ленты (`TapeInterface`) нужного типа (текстовый или
/// бинарный).
///
/// Фабричная функция, которая инстанцирует либо TextFileTape, либо
/// BinaryFileTape в зависимости от переданного расширения файла.
/// Предполагается, что расширение уже было проверено функцией
/// validateExtensions, но здесь выполняется дополнительная проверка для
/// надежности.
///
/// @param config Конфигурация ленты, передаваемая в конструктор создаваемой
/// ленты.
/// @param filename Имя файла, с которым будет работать создаваемая лента.
/// @param ext Расширение файла (в нижнем регистре, ожидается ".txt" или
/// ".bin"),
///            определяющее тип создаваемой ленты.
/// @return std::unique_ptr<TapeInterface> Умный указатель на созданный объект
/// ленты.
/// @throw std::invalid_argument Если переданное расширение `ext` не является ни
/// ".txt", ни ".bin".
std::unique_ptr<TapeInterface> createTape(const size_t maxSize,
                                          const TapeConfig &config,
                                          const std::string &filename,
                                          const std::string &ext);

/// @brief Очищает содержимое указанного файла.
///
/// Реализация открывает файл с флагом std::ios::trunc, который
/// очищает файл при открытии. Также проверяется успешность открытия файла.
///
/// @param filename Имя файла для очистки.
/// @throw std::runtime_error Если файл не удалось открыть/очистить.
void clearFile(const std::string &filename);

/// @brief Возвращает размер указанного файла в байтах.
/// @param filename Имя файла
/// @return Размер файла в байтах. Если файл не найден, возвращает 0.
size_t getFileSize(const std::string &filename);

} // namespace utils
