#include "colors.hpp"
#include "debugprint.hpp"
#include "parsing.hpp"
#include <FS.h>
#include <SPIFFS.h>


ColorInstance colorInstance;
ColorConfig config;

static DebugPrint& debugPrint = DebugPrint::getInstance();

void ifWhite(struct ColorInstance &colorinst, int i)
{
  if(colorinst.colorArray[i].r == 0 && colorinst.colorArray[i].g == 0 && colorinst.colorArray[i].b == 0 && colorinst.colorArray[i].w)
  {
    colorinst.colorArray[i].r = colorinst.colorArray[i].w;
    colorinst.colorArray[i].g = colorinst.colorArray[i].w;
    colorinst.colorArray[i].b = colorinst.colorArray[i].w;
  }
}

void setPixelColorFromCode(int index, char code, struct ColorInstance &colorinst, ledStripInfo *strip, String line) 
{  
  for (int i = 0; i < colorinst.colorCount; i++) 
  { 
    
    if (colorinst.colorArray[i].symbol == code) 
    { 
      ifWhite(colorinst, i);
      strip->strip->setPixelColor(index, strip->strip->Color(colorinst.colorArray[i].r, colorinst.colorArray[i].g, colorinst.colorArray[i].b, colorinst.colorArray[i].w));
    }
  }
}
void storeColorConfig(struct ColorInstance &colorInst , ColorConfig config) 
{ 
  colorInst.colorArray[colorInst.colorCount].symbol = config.symbol;
  colorInst.colorArray[colorInst.colorCount].r = config.r;
  colorInst.colorArray[colorInst.colorCount].g = config.g;
  colorInst.colorArray[colorInst.colorCount].b = config.b;
  colorInst.colorArray[colorInst.colorCount].w = config.w;
  delay(10);
  colorInstance.colorCount++;
}

void LineCounter(const char *filePath , int &lineCounter)  
{ 
  if (!SPIFFS.begin(true)) 
  {
    debugPrint.debugPrint("Failed to mount file system");
    return;
  }
  
  File configFile = SPIFFS.open(filePath, "r");
  
  if (!configFile) 
  {
    debugPrint.debugPrint("Failed to open config file");
    return;
  }

  // Parse and print the configuration
  while (configFile.available()) 
  { 
    String line = configFile.readStringUntil('\n');
    
    if (line.length() > 0) 
    {              
      if(parseConfigLine(line, config.symbol, config.r, config.g, config.b , config.w))
      {
        lineCounter++;
      }
    }
  }
  configFile.close();
} 