#pragma once

#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.h"

// Define a struct to store the parsed configuration
struct ColorConfig {
  char symbol;
  uint8_t r, g, b, w;
};

// void clearingStrips(ledStripInfo *strip);