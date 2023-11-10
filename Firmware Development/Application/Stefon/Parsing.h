#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>

struct Types {
  const char *typeStr;
  uint8_t type;
};
typedef struct {
  Adafruit_NeoPixel *strips;
  uint16_t len;
  const char *gettingTypeString;
} adaFruitPack;
const char* isCompared(const char* getType);
void parseSection(String sectionName, File configFile, std::list<adaFruitPack *> &stripsList);
// void setPixelColorFromCode(int index, char code, ColorConfig *colorArray, adaFruitPack *strip, String line);
// bool parseConfigLine(String line, char& symbol, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& w);
// void parseAndPrintConfig(const char* filePath);
// void parseAndUpdateStrip(String line);
// void playAnimation(String filename);
// void setPixelColorFromCode(int index, char code, ColorConfig *colorArray);