#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.h"
#include "StripColors.hpp"

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;

int aniFileLinecount = 0;
String nextAnimationFile = "";
int NUM_LEDS = 0;
std::list<ledStripInfo *> stripList;

int lineCounter = 0;
struct ColorInstance{ 
  ColorConfig *colorArray;
  int colorCount = 0;
};
extern uint8_t inputPins[10];

ColorInstance colorInstance;
ColorConfig config;
extern std::list<int> pinsList;

std::list<String> recordingAni;
std::list<String>::iterator recordAniItr;
std::list<ledStripInfo *>::iterator it;
File configFile;

void storeColorConfig(struct ColorInstance &colorInst , ColorConfig config) 
{ 
  colorInst.colorArray[colorInst.colorCount].symbol = config.symbol;
  colorInst.colorArray[colorInst.colorCount].r = config.r;
  colorInst.colorArray[colorInst.colorCount].g = config.g;
  colorInst.colorArray[colorInst.colorCount].b = config.b;
  colorInst.colorArray[colorInst.colorCount].w = config.w;
}

void LineCounter(const char *filePath)  
{ 
  if (!SPIFFS.begin(true)) 
  {
    // Serial.println("Failed to mount file system");
    return;
  }
  
  configFile = SPIFFS.open(filePath, "r");
  
  if (!configFile) 
  {
    //Serial.println("Failed to open config file");
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
        delay(10);
        colorInstance.colorCount++;

      }
    }
  }
  configFile.close();
} 

int PinGetPullDown()
{ 
  for (std::list<int>::iterator it = pinsList.begin(); !(it == pinsList.end()); ++it)
  { 
    int readPinValueReturn = digitalRead(*it);
    if(0 == readPinValueReturn)
    { 
      return *it;
    }
  }
   return 0;
}
struct Task3Parameters {
  int pinNum;
  bool whenTrue;
};
void setup() 
{
  Serial.begin(115200);
  LineCounter("/colortable.conf");
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
  // int sizee = pinsList.size();
  // Serial.print("Size of the List: ");
  // Serial.println(sizee);
  for (int i = 0; i<lineCounter ; i++)
  {
  printf("symbol %c color combination %d, %d, %d, %d\n " , colorInstance.colorArray[i].symbol , colorInstance.colorArray[i].r , colorInstance.colorArray[i].g , colorInstance.colorArray[i].b , colorInstance.colorArray[i].w);
  }
    // Initialize semaphore

  xTaskCreatePinnedToCore(Task1code, "Task1" ,  10000 ,   NULL        ,    1      , &Task1,     1);    
      //                     func       name    stack    parameters     task          task       core of the task
     //                                         Size     of the task    pariority     handler
  
  xTaskCreatePinnedToCore(Task2code, "Task2" ,  10000 ,   NULL        ,    3     , &Task2,     1);

}


void clearingStrips(ledStripInfo *strip)
{
  for (it = stripList.begin(); it != stripList.end(); it++) 
  {
    (*it)->strip->clear();
    (*it)->strip->show();
  }
}

void Task1code(void *pvParameters)
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
bool whenTrue = false;
int checked = 0;
void Task2code(void *pvParameters)
{ 
  while (1)
  { 
    // Serial.println("Task2");
    whenTrue = false;
    int pinNum = PinGetPullDown();
    if(pinNum)
    { 
      // Serial.println("if pinNum :::::: ");
      clearingStrips(*it);
      whenTrue = true;
      BaseType_t taskCreated;
      TaskHandle_t newTaskHandle;
      if(!checked)
      {
       Serial.println("if(pinNum == 4 && !checked)");
        // Create an instance of the struct and populate its fields
       xTaskCreatePinnedToCore(Task3code, "Task3" ,  10000 ,   (void*)&pinNum ,   2    , &Task3,  1);
       checked = 1;
      } 
    }
    else
    {
      whenTrue = false;
    }
    delay(10);
  }
}

void Task3code(void *pvParameters)
{ 
  int *pPinNum = (int*)pvParameters; // Cast the parameter back to int pointer
  int pinNum = *pPinNum; // Dereference the pointer to get the value
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
      checked = 0;
      Serial.println("read pin condition");
      break;
    }
  }
  Serial.println("While se bahar");
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

void playAnimation(String filename , int pinNum) 
{ 
 File file = SPIFFS.open(filename.c_str());
 
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
    int readPin;
    if(pinNum == -1)
    {
      readPin == 0;
    }
    else
    {
      readPin = digitalRead(pinNum);
    }
    String line = file.readStringUntil('\n');
    line.trim();
    // count++;
    if(readPin == 1)
    {
      return;
    }
    // Serial.println(whenTrue);
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
      delay(10);
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
  for (int i = 0; i < colorinst.colorCount; i++) 
  { 
    
    if (colorinst.colorArray[i].symbol == code) 
    { 
      ifWhite(colorinst, i);
      strip->strip->setPixelColor(index, strip->strip->Color(colorinst.colorArray[i].r, colorinst.colorArray[i].g, colorinst.colorArray[i].b, colorinst.colorArray[i].w));
    }
  }
}
