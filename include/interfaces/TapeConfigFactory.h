#pragma once

#include "../entities/TapeConfig.h"

class TapeConfigFactory {
public:
  virtual ~TapeConfigFactory() = default;
  virtual TapeConfig createConfig() const = 0;
};
