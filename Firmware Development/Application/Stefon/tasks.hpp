#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>

typedef struct 
{
  int pinNum;
  int *checked;

} Task3Parameters;

typedef struct 
{
  Adafruit_NeoPixel *strip;
  uint16_t len;
  const char *type;
} ledStripInfo;

// int PinGetPullDown();
