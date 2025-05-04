#include "../../include/factories/BuilerTapeConfigFactory.h"

BuilderTapeConfigFactory &BuilderTapeConfigFactory::setReadDelay(int delay) {
  config.readDelay = delay;
  return *this;
}

BuilderTapeConfigFactory &BuilderTapeConfigFactory::setWriteDelay(int delay) {
  config.writeDelay = delay;
  return *this;
}

BuilderTapeConfigFactory &BuilderTapeConfigFactory::setRewindDelay(int delay) {
  config.rewindDelay = delay;
  return *this;
}

BuilderTapeConfigFactory &BuilderTapeConfigFactory::setShiftDelay(int delay) {
  config.shiftDelay = delay;
  return *this;
}

TapeConfig BuilderTapeConfigFactory::createConfig() const { return config; }
