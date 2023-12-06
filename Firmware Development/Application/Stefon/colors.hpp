#pragma once
#include "Parsing.hpp"
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "tasks.hpp"
struct ColorConfig {
  char symbol;
  uint8_t r, g, b, w;
};

struct ColorInstance{ 
  ColorConfig *colorArray;
  int colorCount = 0;
};

// Define a struct to store the parsed configuration
class Colors
{  
   private:
      
   public:
      int lineCounter = 0;
      ColorConfig config;
      ColorInstance colorInstance;
      static Colors& getInstance();
      void ifWhite(struct ColorInstance &colorinst, int i);
      void setPixelColorFromCode(int index, char code, struct ColorInstance &colorinst, ledStripInfo *strip, String line);
      void storeColorConfig(struct ColorInstance &colorInst , ColorConfig config);
      void LineCounter(const char *filePath , int &lineCounter);
};
