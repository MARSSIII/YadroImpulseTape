#include "../include/entities/OldTapeSorter.h"
#include "../include/entities/TapeConfig.h"
#include "../include/entities/fileTapes/BinaryFileTape.h"
#include "../include/entities/fileTapes/TextFileTape.h"

#include "../include/factories/TapeConfigFactory.h"

#include "../include/interfaces/TapeInterface.h"

#include "../include/utils/utils.hpp"

#include <cctype>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0]
              << " <input_file> <output_file> [config_file]\n";
    return 1;
  }

  const fs::path inputPath(argv[1]);
  const fs::path outputPath(argv[2]);
  const std::string configFile = (argc > 3) ? argv[3] : "";

  const std::string inputExt = utils::getFileExtension(inputPath.string());
  const std::string outputExt = utils::getFileExtension(outputPath.string());

  try {
    utils::validateExtensions(inputExt, outputExt);
    utils::clearFile(outputPath.string());

    auto configFactory = std::make_unique<TapeConfigFactory>(configFile);
    TapeConfig config = configFactory->create();

    std::unique_ptr<TapeInterface> inputTape;
    std::unique_ptr<TapeInterface> outputTape;

    inputTape = utils::createTape(config, inputPath.string(), inputExt);
    outputTape = utils::createTape(config, outputPath.string(), outputExt);

    TapeSorter sorter(3);
    sorter.sort(*inputTape, *outputTape);
    /*
    if (outputExt == ".txt") {

              utils::processTapes<TextFileTape>(config, inputPath.string(),
                                                outputPath.string());
            } else {
              utils::processTapes<BinaryFileTape>(config, inputPath.string(),
                                                  outputPath.string());
            }
     */
  } catch (const std::exception &e) {
    std::cerr << "Error: \n" << e.what() << '\n';
    return 1;

  } catch (...) {
    std::cerr << "An unexpected error occurred.\n";
    return 1;
  }

  std::cout << "completed." << std::endl;
  return 0;
}
