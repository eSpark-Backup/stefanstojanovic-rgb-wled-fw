#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>

struct ledStripMap {
  const char *type;
  uint8_t value;
};

typedef struct {
  Adafruit_NeoPixel *strip;
  uint16_t len;
  const char *type;
} ledStripInfo;


void parseSection(String sectionName, File configFile, std::list<ledStripInfo *> &stripList);
uint8_t typeCheck(uint8_t *combination, struct ledStripMap (&stripTypes)[30], const char *getType);
