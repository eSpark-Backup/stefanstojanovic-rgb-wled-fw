#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.h"
#include "strips.hpp"

typedef struct {
  int pinNum;
  int *checked;
} Task3Parameters;

int PinGetPullDown();
// void PlayingNormalRoutine(void *pvParameters);
// void CheckingPinInterrupt(void *pvParameters);
// void HandlingPinInterrupt(void *pvParameters);