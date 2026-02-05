#pragma once

#include "../interfaces/TapeConfigFactoryInterface.h"
#include <string>

class TapeConfigFactory : public TapeConfigFactoryInterface {
public:
  explicit TapeConfigFactory(std::string filename);

  TapeConfig create() const final;

private:
  std::string m_configFile;

  void parseLine(const std::string &line, TapeConfig &config) const;

  void validateConfig(const TapeConfig &config) const;
};
