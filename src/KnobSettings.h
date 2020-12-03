#pragma once
#include <stdint.h>

struct KnobSettings {
  int32_t currentValue;
  int32_t minValue;
  int32_t maxValue;
  bool loopRotary;
};