#include "colors.hpp"
#include "debugprint.hpp"
#include <FS.h>
#include <SPIFFS.h>

static DebugPrint& debugPrint = DebugPrint::getInstance();
static Parsing& parsing = Parsing::getInstance();
// namespace 
// {
//     Colors* colorsInstance = nullptr;
// }

// Colors& Colors::getInstance() 
// {
//     if(nullptr == colorsInstance)
//     {
//        colorsInstance = new Colors();
//     }
//     return *colorsInstance;
// }
Colors Colors::colors;
void Colors::ifWhite(struct ColorInstance &colorinst, int i)
{
  if(colorinst.colorArray[i].r == 0 && colorinst.colorArray[i].g == 0 && colorinst.colorArray[i].b == 0 && colorinst.colorArray[i].w)
  {
    colorinst.colorArray[i].r = colorinst.colorArray[i].w;
    colorinst.colorArray[i].g = colorinst.colorArray[i].w;
    colorinst.colorArray[i].b = colorinst.colorArray[i].w;
  }
}

void Colors::setPixelColorFromCode(int index, char code, struct ColorInstance &colorinst, ledStripInfo *strip, String line) 
{  
  for (int i = 0; i < colorinst.colorCount; i++) 
  { 
    
    if (colorinst.colorArray[i].symbol == code) 
    { 
      ifWhite(colorinst, i);
      strip->activeStrip->setPixelColor(index, strip->activeStrip->Color(colorinst.colorArray[i].r, colorinst.colorArray[i].g, colorinst.colorArray[i].b, colorinst.colorArray[i].w));
    }
  }
}
void Colors::storeColorConfig(struct ColorInstance &colorInst , ColorConfig config) 
{ 
  colorInst.colorArray[colorInst.colorCount].symbol = config.symbol;
  colorInst.colorArray[colorInst.colorCount].r = config.r;
  colorInst.colorArray[colorInst.colorCount].g = config.g;
  colorInst.colorArray[colorInst.colorCount].b = config.b;
  colorInst.colorArray[colorInst.colorCount].w = config.w;
  delay(10);
  colorInstance.colorCount++;
}

void Colors::LineCounter(const char *filePath , int &lineCounter)  
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
        if(parsing.parseConfigLine(line, config.symbol, config.r, config.g, config.b , config.w))
        {
            lineCounter++;
        }
    }
  }
  configFile.close();
} 