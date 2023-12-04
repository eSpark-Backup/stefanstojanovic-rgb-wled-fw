#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.h"
#include "strips.hpp"
#include "tasks.hpp"
TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;
bool debugMode = false;
int aniFileLinecount = 0;
String nextAnimationFile = "";
int NUM_LEDS = 0;
std::list<ledStripInfo *> stripList;

int lineCounter = 0;

struct ColorInstance{ 
  ColorConfig *colorArray;
  int colorCount = 0;
};

ColorInstance colorInstance;
ColorConfig config;
extern parseFileParameters parseCurrent;
std::list<ledStripInfo *>::iterator it;
File configFile;

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

void LineCounter(const char *filePath)  
{ 
  if (!SPIFFS.begin(true)) 
  {
    debugPrint("Failed to mount file system");
    return;
  }
  
  configFile = SPIFFS.open(filePath, "r");
  
  if (!configFile) 
  {
    debugPrint("Failed to open config file");
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



void parseAndPrintConfig(const char *filePath) 
{ 

  if (!SPIFFS.begin(true)) 
  {
    debugPrint("Failed to mount file system");
    return;
  }

  configFile = SPIFFS.open(filePath, "r");

  if (!configFile) 
  {
    Serial.println("Failed to open config file");
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
      }
    }
  }
  configFile.close();
} 

void setup() 
{ 
      Serial.begin(115200);
      SPIFFS.begin();
      LineCounter("/colortable.conf");
      colorInstance.colorArray = new ColorConfig[lineCounter];
      parseAndPrintConfig("/colortable.conf");
      File configFile = SPIFFS.open("/config.conf", "r");
      parseSection("", configFile, stripList);  // Pass an empty section name to stat
      if(1 == parseCurrent.debug)
      {
        debugMode = true;
      }

      // for (int i = 0; i<lineCounter ; i++)
      // {
      // printf("symbol %c color combination %d, %d, %d, %d\n " , colorInstance.colorArray[i].symbol , colorInstance.colorArray[i].r , colorInstance.colorArray[i].g , colorInstance.colorArray[i].b , colorInstance.colorArray[i].w);
      // }
        // Initialize semaphore

      xTaskCreatePinnedToCore(PlayingNormalRoutine, "Task1" ,  10000 ,   NULL        ,    1      , &Task1,     1);    
          //                     func       name    stack    parameters     task          task       core of the task
        //                                         Size     of the task    pariority     handler
      
      xTaskCreatePinnedToCore(CheckingPinInterrupt, "Task2" ,  10000 ,   NULL        ,    3     , &Task2,     1);

}

void debugPrint(String DebugText) 
{
   if (debugMode) Serial.println(DebugText);
}

void clearingStrips(ledStripInfo *strip)
{
    for (it = stripList.begin(); it != stripList.end(); it++) 
    {
        (*it)->strip->clear();
        (*it)->strip->show();
    }
}

void PlayingNormalRoutine(void *pvParameters)
{
    String currentAnimationFile = "/startup.ani";
    while (1)
    { 
        if (nextAnimationFile != "")
        {
            currentAnimationFile = nextAnimationFile;
            nextAnimationFile = "";
        }
        playAnimation(currentAnimationFile , -1);
        delay(100);
    }
}

void CheckingPinInterrupt(void *pvParameters)
{ 
    int checked = 0;
    while (1)
    { 
          Serial.print("checked: ");
          Serial.println(checked);
          int pinNum = PinGetPullDown();
          if(pinNum)
          { 
              clearingStrips(*it);
              BaseType_t taskCreated;
              TaskHandle_t newTaskHandle;
              if(!checked)
              {
                checked = 1;
                debugPrint("if(pinNum == 4 && !checked)");
                  // Create an instance of the struct and populate its fields
                Task3Parameters params = {pinNum, &checked};
                xTaskCreatePinnedToCore(HandlingPinInterrupt, "Task3" ,  10000 ,   (void*)&params ,   2    , &Task3,  1);
              
              } 
          }
          delay(10);
    }
}

void HandlingPinInterrupt(void *pvParameters)
{ 
  Task3Parameters *params = (Task3Parameters *)pvParameters;
  int pinNum = params->pinNum;
  int *checked = params->checked;
  char buffer[2];
  itoa(pinNum, buffer, 10);
  String pinNumber = String(buffer);
  String currentAnimationFile = "/PIN" + pinNumber + ".ani";
  vTaskSuspend(Task1);
  clearingStrips(*it);
  while (1)
  { 
      playAnimation(currentAnimationFile , pinNum);
      int readPin = digitalRead(pinNum);
      if(readPin == 1)
      { 
          break;
      }
  }
  *checked = 0;
  clearingStrips(*it);
  vTaskResume(Task1);
  vTaskDelete(NULL);
}

void loop() 
{ 
 
  delay(10);
  // Serial.println(xPortGetCoreID());
  // if (nextAnimationFile != "") 
  // { 
  //   currentAnimationFile = nextAnimationFile;
  //   nextAnimationFile = "";
  // }
  // playAnimation(currentAnimationFile);
}

bool ifDelta = false;
void playAnimation(String filename , int pinNum) 
{  
  File file = SPIFFS.open(filename.c_str());
  size_t currentPosition = 0;
  if (!file) 
  {
    return;
  } 
  else 
  {
    debugPrint("Succesfully opened animation file: " + filename);
  }
  while (file.available()) 
  { 
    int readPin;
    if(pinNum == -1)
    {
      readPin == 0;
    }
    else
    {
      readPin = digitalRead(pinNum);
    }
    size_t position = file.position();
    String line = file.readStringUntil('\n');
    line.trim();
    if (!line.startsWith("STOP")) 
    { 
      currentPosition = position;
    }
    if(readPin == 1)
    {
      return;
    }
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
      file.seek(currentPosition);
      parseAndUpdateStrip(line, stripList , 0);
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
      parseAndUpdateStrip(line.substring(dividerPos + 1), stripList , 0 );
      delay(customDuration);
    } 
    else if (line.indexOf('~') != -1)
    {
      int dividerPos = line.indexOf('~');
      int customDuration = line.substring(0, dividerPos).toInt();
      ifDelta = true;
      parseAndUpdateStrip(line.substring(dividerPos + 1), stripList, customDuration);
      continue;
    }
    else 
    {
      parseAndUpdateStrip(line, stripList , 0);
      delay(100);
    }
  }
  file.close();
}

void parseAndUpdateStrip(String line, std::list<ledStripInfo *> &stripList , int costumDuration) 
{ 
  bool stopCommand = false;
  if(line == "STOP")
  {
    stopCommand = true;
  }
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
      if(ifDelta)
      { 
        uint8_t getBrightnessValue = (*it)->strip->getBrightness();
        float divideRatioValue = 0;
        if(costumDuration)
        { 
          divideRatioValue = ((float)getBrightnessValue)/costumDuration;   //costumDuration
        }
        else
        { 
          divideRatioValue = ((float)getBrightnessValue)/100;
        }
        unsigned long startTime;
        unsigned long measureTime = 0;
        startTime = millis();
        for(float i = getBrightnessValue ; i >= 0;)
        { 
          
          for (it = stripList.begin(); it != stripList.end(); it++)
          {
           (*it)->strip->setBrightness(i);
          }
          showingStrips(*it);
          measureTime = millis() - startTime;
          i = getBrightnessValue - (divideRatioValue * measureTime);
        }
        // unsigned long endTime = millis();
        // unsigned long loopTime = endTime - startTime;
        // Serial.println(loopTime);
        ifDelta = false;
        for (it = stripList.begin(); it != stripList.end(); it++)
        {
          (*it)->strip->setBrightness(getBrightnessValue);
        }
        return;
      }
      else if(stopCommand)
      {  
        for (std::list<ledStripInfo *>::iterator it = stripList.begin(); it != stripList.end(); it++) 
        {
          (*it)->strip->clear();
        }
      }
      else
      {
        setPixelColorFromCode(ledIdx, line[i], colorInstance, *it, line);
        ledIdx++;
      }
    }
  }
showStrip:
  showingStrips(*it);
  if(stopCommand)
  {
    delay(500);
  }
  it = stripList.begin();
}

void showingStrips(ledStripInfo *strip)
{ 
  for (std::list<ledStripInfo *>::iterator it = stripList.begin(); it != stripList.end(); it++) 
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
  for (int i = 0; i < colorinst.colorCount; i++) 
  { 
    
    if (colorinst.colorArray[i].symbol == code) 
    { 
      ifWhite(colorinst, i);
      strip->strip->setPixelColor(index, strip->strip->Color(colorinst.colorArray[i].r, colorinst.colorArray[i].g, colorinst.colorArray[i].b, colorinst.colorArray[i].w));
    }
  }
}
