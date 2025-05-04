// FileTapeConfigFactory.h
#pragma once

#include "../interfaces/TapeConfigFactoryInterface.h"
#include <string>

class FileTapeConfigFactory : public TapeConfigFactoryInterface {
public:
  explicit FileTapeConfigFactory(std::string filename);
  TapeConfig create() const final;

private:
  std::string configFile_;

  void parseLine(const std::string &line, TapeConfig &config) const;
  void validateConfig(const TapeConfig &config) const;
};
