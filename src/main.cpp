#include "../include/factories/BuilerTapeConfigFactory.h"
#include "../include/factories/FileTapeConfigFactory.h"

#include "../include/interfaces/TapeConfigFactory.h"

#include "../include/entities/FileTape.h"
#include "../include/entities/TapeConfig.h"
#include "../include/entities/TapeSorter.h"

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <input> <output> [config]\n";
    return 1;
  }

  try {
    std::unique_ptr<TapeConfigFactory> configFactory;

    if (argc > 3) {
      configFactory = std::make_unique<FileTapeConfigFactory>(argv[3]);

    } else {
      configFactory = std::make_unique<BuilderTapeConfigFactory>();
    }

    TapeConfig config = configFactory->createConfig();

    FileTape input(argv[1], config);
    FileTape output(argv[2], config);

    TapeSorter sorter(1025);
    sorter.sort(input, output);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
