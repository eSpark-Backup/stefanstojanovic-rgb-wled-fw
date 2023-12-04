#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.hpp"
#include "colors.hpp"

typedef struct {
  int pinNum;
  int *checked;
} Task3Parameters;

int PinGetPullDown();
