#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "ledstrip.hpp"

typedef struct
{
  int pin = 0;
  uint16_t length = 0;
  int skip = 0;
  int brightness = 0;
  int debug = 0;
}parseFileParameters;

struct ledStripMap 
{
  const char *type;
  uint8_t value;
};

// typedef struct 
// {
//   Adafruit_NeoPixel *strip;
//   uint16_t len;
//   const char *type;
// } ledStripInfo;

void parseSection(String sectionName, File configFile, std::list<ledStripInfo *> &stripList);
uint8_t typeCheck(uint8_t *combination, struct ledStripMap (&stripTypes)[30], const char *getType);
bool parseConfigLine(String line, char &symbol, uint8_t &r, uint8_t &g, uint8_t &b , uint8_t &w);