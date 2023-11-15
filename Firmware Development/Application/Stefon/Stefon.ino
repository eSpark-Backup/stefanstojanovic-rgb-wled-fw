#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.h"
#include "StripColors.hpp"

int NUM_LEDS = 0;
std::list<ledStripInfo *> stripList;
File file;
int lineCounter = 0;
struct ColorInstance{ 
  ColorConfig *colorArray;
  int colorCount = 0;
};


ColorInstance colorInstance;
ColorConfig config;

String currentAnimationFile = "/startup.ani";
String nextAnimationFile = "";
std::list<ledStripInfo *>::iterator it;

void storeColorConfig(struct ColorInstance &colorInst , ColorConfig config) 
{ 
  colorInst.colorArray[colorInst.colorCount].symbol = config.symbol;
  colorInst.colorArray[colorInst.colorCount].r = config.r;
  colorInst.colorArray[colorInst.colorCount].g = config.g;
  colorInst.colorArray[colorInst.colorCount].b = config.b;
  colorInst.colorArray[colorInst.colorCount].w = config.w;
}

int LineCounter(const char *filePath , int &lineCounter)  
{  
  File configFile = SPIFFS.open(filePath, "r");
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
  return lineCounter;
} 

File configFile;

void parseAndPrintConfig(const char *filePath) 
{ 

  if (!SPIFFS.begin(true)) 
  {
    // Serial.println("Failed to mount file system");
    return;
  }

  configFile = SPIFFS.open(filePath, "r");

  if (!configFile) 
  {
    // Serial.println("Failed to open config file");
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
        storeColorConfig(colorInstance , config);
        delay(100);
        colorInstance.colorCount++;
      
      }
    }
  }
  configFile.close();
} 

void setup() 
  {
    Serial.begin(115200);
    LineCounter("/colortable.conf" , lineCounter);
    Serial.print("linecouter Value: ");
    Serial.println(lineCounter);
    colorInstance.colorArray = new ColorConfig[lineCounter];
    parseAndPrintConfig("/colortable.conf");
     
    if (SPIFFS.begin()) 
    { 
      File configFile = SPIFFS.open("/config.conf", "r");

      if (configFile) 
      {
        parseSection("", configFile, stripList);  // Pass an empty section name to stat
        // Serial.println((uint32_t)stripList.front());

        // strip = stripList.front();
      } 
      else 
      {
        Serial.println("Failed to open config file");
      }
    } 
  else 
  {
    Serial.println("Failed to mount SPIFFS");
  }
  for (int i = 0; i<lineCounter ; i++)
  {
   printf("symbol %c color combination %d, %d, %d, %d\n " , colorInstance.colorArray[i].symbol , colorInstance.colorArray[i].r , colorInstance.colorArray[i].g , colorInstance.colorArray[i].b , colorInstance.colorArray[i].w);
  }
}

void loop() 
{
  if (nextAnimationFile != "") 
  { 
    currentAnimationFile = nextAnimationFile;
    nextAnimationFile = "";
  }
  playAnimation(currentAnimationFile);
}



void playAnimation(String filename) 
{
  file = SPIFFS.open(filename.c_str());
  if (!file) 
  {
    return;
  } 
  else 
  {
    // Serial.print("Successfully opened: ");
    // Serial.println(filename);
  }
  
  while (file.available()) 
  {
    String line = file.readStringUntil('\n');
    line.trim();
    
    if (line.startsWith("L:")) 
    {
      NUM_LEDS = line.substring(2).toInt();
      (*it)->strip->updateLength(NUM_LEDS);
      (*it)->strip->show();  // Update the strip length and clear it
      continue;
    }

    if (line.endsWith(".ani")) 
    {
      nextAnimationFile = "/" + line;
      break;  // Exit out of the while loop to let the main loop handle the file switch
    }

    if (line.startsWith("STOP")) 
    {
      

    } 
    else if (line.startsWith("PAUSE")) 
    {
      int pauseDuration = line.substring(6).toInt();
      delay(pauseDuration);

      continue;
    } 
    else if (line.indexOf('|') != -1) 
    {

      int dividerPos = line.indexOf('|');
      int customDuration = line.substring(0, dividerPos).toInt();
      parseAndUpdateStrip(line.substring(dividerPos + 1), stripList);
      delay(customDuration);
    } 
    else 
    {
      parseAndUpdateStrip(line, stripList);
      delay(100);
    }
  }
  file.close();
}

void parseAndUpdateStrip(String line, std::list<ledStripInfo *> &stripList) 
{ 
  it = stripList.begin();
  int ledIdx = 0;

  for (int i = 0; i < line.length(); i++) 
  {
    if (line[i] == ';') 
    {
      ledIdx++;
      if (ledIdx >= (*it)->len) 
      {
        it++;
        if (it == stripList.end()) 
        {
          goto showStrip;
        }
        ledIdx = 0;
      }
      continue;
    }

    if (line[i] == '-') 
    {
      
      int dashEnd = line.indexOf('>', i);
      
      String repeatValue = line.substring(i + 1, dashEnd);
      int repeatCount = repeatValue.toInt();
      char colorCode = line[i - 1];

      if (repeatCount) 
      {
        for (int j = 0; j < repeatCount; j++) 
        {
          if (ledIdx >= (*it)->len) 
          {
            it++;
            if (it == stripList.end()) 
            {
              goto showStrip;
            }
            ledIdx = 0;
          }
          setPixelColorFromCode(ledIdx, colorCode, colorInstance, *it, line);
          ledIdx++;
        }
        i = dashEnd;
        
      } 
      else 
      {
        for (; it != stripList.end(); it++) 
        {
          for (int j = ledIdx; j < ((*it)->len); j++) 
          {
            setPixelColorFromCode(j, colorCode, colorInstance, *it, line);
          }
          ledIdx = 0;
        }
        i = line.length();
      }
    } 

    else 
    {
      if (ledIdx >= (*it)->len) 
      {
        it++;
        ledIdx = 0;
        if (it == stripList.end()) 
        {
          break;
        }
      }
      setPixelColorFromCode(ledIdx, line[i], colorInstance, *it, line);
      (*it)->strip->show();
      ledIdx++;
    }
  }

showStrip:
  showingStrips(*it);
  it = stripList.begin();
}

void showingStrips(ledStripInfo *strip)
{
  for (it = stripList.begin(); it != stripList.end(); it++) 
  {
    (*it)->strip->show();
  }
}

bool parseConfigLine(String line, char &symbol, uint8_t &r, uint8_t &g, uint8_t &b , uint8_t &w) 
{
  int count = sscanf(line.c_str(), "%c: %d,%d,%d,%d", &symbol, &r, &g, &b, &w);
  return (count == 4 || count == 5);
}

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
  // Serial.print("colorCount: ");
  // Serial.println(colorinst.colorCount);   
  for (int i = 0; i < colorinst.colorCount; i++) 
  { 
    
    if (colorinst.colorArray[i].symbol == code) 
    { 


      ifWhite(colorinst, i);
      strip->strip->setPixelColor(index, strip->strip->Color(colorinst.colorArray[i].r, colorinst.colorArray[i].g, colorinst.colorArray[i].b, colorinst.colorArray[i].w));
    }
  }
}
