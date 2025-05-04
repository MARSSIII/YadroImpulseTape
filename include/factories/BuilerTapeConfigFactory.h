#pragma once

#include "../interfaces/TapeConfigFactory.h"

class BuilderTapeConfigFactory : public TapeConfigFactory {
public:
  BuilderTapeConfigFactory &setReadDelay(int delay);
  BuilderTapeConfigFactory &setWriteDelay(int delay);
  BuilderTapeConfigFactory &setRewindDelay(int delay);
  BuilderTapeConfigFactory &setShiftDelay(int delay);

  TapeConfig createConfig() const final;

private:
  TapeConfig config;
};
