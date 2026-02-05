#pragma once

#include "../entities/TapeConfig.h"

class TapeConfigFactoryInterface {
public:
  virtual ~TapeConfigFactoryInterface() = default;

  virtual TapeConfig create() const = 0;
};
