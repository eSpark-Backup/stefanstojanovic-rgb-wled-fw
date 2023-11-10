#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.h"

int NUM_LEDS = 0;
extern const char* getType;
int tracker = 0;
std::list<adaFruitPack *> stripsList;
File file;
Adafruit_NeoPixel input;
Adafruit_NeoPixel output;

// Define a struct to store the parsed configuration
struct ColorConfig {
  char symbol;
  uint8_t r, g, b, w;
};
int maxColors = 0;
struct ColorConfig *colorArray = new ColorConfig[maxColors];
int colorCount = 0;
ColorConfig config;
String currentAnimationFile = "/startup.ani";
String nextAnimationFile = "";
std::list<adaFruitPack *>::iterator it;

void storeColorConfig(ColorConfig config) 
{
  colorArray[colorCount].symbol = config.symbol;
  colorArray[colorCount].r = config.r;
  colorArray[colorCount].g = config.g;
  colorArray[colorCount].b = config.b;
  colorArray[colorCount].w = config.w;
}

void parseAndPrintConfig(const char *filePath) 
{
  if (!SPIFFS.begin(true)) 
  {
    // Serial.println("Failed to mount file system");
    return;
  }

  File configFile = SPIFFS.open(filePath, "r");

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
      if (parseConfigLine(line, config.symbol, config.r, config.g, config.b , config.w)) 
      {
        storeColorConfig(config);
        // Serial.print("Symbol: ");
        // Serial.print(config.symbol);
        // Serial.print(", RGBW: ");
        // Serial.print(config.r);
        // Serial.print(", ");
        // Serial.print(config.g);
        // Serial.print(", ");
        // Serial.print(config.b);
        // Serial.print(", ");
        // Serial.println(config.w);
        delay(100);
        colorCount++;
      }
    }
  }
  configFile.close();
}


void setup() 
  {
    Serial.begin(115200);
    parseAndPrintConfig("/colortable.conf");
    if (SPIFFS.begin()) 
    {
      File configFile = SPIFFS.open("/config.conf", "r");

      if (configFile) 
      {
        parseSection("", configFile, stripsList);  // Pass an empty section name to stat
        // Serial.println((uint32_t)stripsList.front());

        // strip = stripsList.front();
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
      (*it)->strips->updateLength(NUM_LEDS);
      (*it)->strips->show();  // Update the strip length and clear it
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
      parseAndUpdateStrip(line.substring(dividerPos + 1), stripsList);
      delay(customDuration);
    } 
    else 
    {
      parseAndUpdateStrip(line, stripsList);
      delay(100);
    }
  }
  file.close();
}

void parseAndUpdateStrip(String line, std::list<adaFruitPack *> &stripsList) 
{
  it = stripsList.begin();
  int ledIdx = 0;

  for (int i = 0; i < line.length(); i++) 
  {
    if (line[i] == ';') 
    {
      ledIdx++;
      if (ledIdx >= (*it)->len) 
      {
        it++;
        if (it == stripsList.end()) 
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
            if (it == stripsList.end()) 
            {
              goto showStrip;
            }
            ledIdx = 0;
          }
          setPixelColorFromCode(ledIdx, colorCode, colorArray, *it, line);
          ledIdx++;
        }
        i = dashEnd;
        
      } 
      else 
      {
        for (; it != stripsList.end(); it++) 
        {
          for (int j = ledIdx; j < ((*it)->len); j++) 
          {
            setPixelColorFromCode(j, colorCode, colorArray, *it, line);
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
        if (it == stripsList.end()) 
        {
          break;
        }
      }
      setPixelColorFromCode(ledIdx, line[i], colorArray, *it, line);
      (*it)->strips->show();
      ledIdx++;
    }
  }

showStrip:
  showingStrips(*it);
  it = stripsList.begin();
}

void showingStrips(adaFruitPack *strip)
{
  for (it = stripsList.begin(); it != stripsList.end(); it++) 
  {
    (*it)->strips->show();
  }
}


bool parseConfigLine(String line, char &symbol, uint8_t &r, uint8_t &g, uint8_t &b , uint8_t &w) 
{
  int count = sscanf(line.c_str(), "%c: %d,%d,%d,%d", &symbol, &r, &g, &b, &w);
  return (count);
}

void ifWhite(ColorConfig *colorArray, int i)
{
  if(colorArray[i].r == 0 && colorArray[i].g == 0 && colorArray[i].b == 0 && colorArray[i].w)
  {
    colorArray[i].r = colorArray[i].w;
    colorArray[i].g = colorArray[i].w;
    colorArray[i].b = colorArray[i].w;
  }
}

void setPixelColorFromCode(int index, char code, ColorConfig *colorArray, adaFruitPack *strip, String line) 
{
  for (int i = 0; i < colorCount; i++) 
  {
    if (colorArray[i].symbol == code) 
    {  
      ifWhite(colorArray, i);
      strip->strips->setPixelColor(index, strip->strips->Color(colorArray[i].r, colorArray[i].g, colorArray[i].b, colorArray[i].w));
    }
  }
}
