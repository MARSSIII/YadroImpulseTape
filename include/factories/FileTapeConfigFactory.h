#include "../interfaces/TapeConfigFactory.h"

#include <string>

class FileTapeConfigFactory : public TapeConfigFactory {
public:
  explicit FileTapeConfigFactory(const std::string &filename);
  TapeConfig createConfig() const final;

private:
  std::string configFile;
};
