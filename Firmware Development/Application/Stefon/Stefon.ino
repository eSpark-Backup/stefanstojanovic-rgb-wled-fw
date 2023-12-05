#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.hpp"
#include "colors.hpp"
#include "tasks.hpp"
#include "debugprint.hpp"
#include "ledstrip.hpp"

// TaskHandle_t Task1;
// TaskHandle_t Task2;
// TaskHandle_t Task3;

// String nextAnimationFile = "";
// int NUM_LEDS = 0;
// std::list<ledStripInfo *> stripList;
// std::list<ledStripInfo *>::iterator it;

static DebugPrint& debugPrint = DebugPrint::getInstance();
static Colors& colors = Colors::getInstance();
static LedStrip& ledStrip = LedStrip::getInstance();

void setup() 
{ 
      Serial.begin(115200);
      SPIFFS.begin();
      int lineCounter = 0;
      colors.LineCounter("/colortable.conf" , lineCounter);
      Serial.println(lineCounter);
      colors.colorInstance.colorArray = new ColorConfig[lineCounter];
      parseAndPrintConfig("/colortable.conf");
      File configFile = SPIFFS.open("/config.conf", "r");
      parseSection("", configFile, ledStrip.stripList);  // Pass an empty section name to stat

      // for (int i = 0; i<lineCounter ; i++)
      // {
      // printf("symbol %c color combination %d, %d, %d, %d\n " , colorInstance.colorArray[i].symbol , colorInstance.colorArray[i].r , colorInstance.colorArray[i].g , colorInstance.colorArray[i].b , colorInstance.colorArray[i].w);
      // }
        // Initialize semaphore

      // xTaskCreatePinnedToCore(PlayingNormalRoutine, "Task1" ,  10000 ,   NULL        ,    1      , &Task1,     1);    
      //     //                     func       name    stack    parameters     task          task       core of the task
      //   //                                         Size     of the task    pariority     handler
      
      // xTaskCreatePinnedToCore(CheckingPinInterrupt, "Task2" ,  10000 ,   NULL        ,    3     , &Task2,     1);

      ledStrip.startTasks();

}

// void clearingStrips(ledStripInfo *strip)
// {
//     for (it = stripList.begin(); it != stripList.end(); it++) 
//     {
//         (*it)->strip->clear();
//         (*it)->strip->show();
//     }
// }

// void PlayingNormalRoutine(void *pvParameters)
// {
//     String currentAnimationFile = "/startup.ani";
//     while (1)
//     { 
//         if (nextAnimationFile != "")
//         {
//             currentAnimationFile = nextAnimationFile;
//             nextAnimationFile = "";
//         }
//         playAnimation(currentAnimationFile , -1);
//         delay(100);
//     }
// }

// void CheckingPinInterrupt(void *pvParameters)
// { 
//     int checked = 0;
//     while (1)
//     { 
//           int pinNum = PinGetPullDown();
//           if(pinNum)
//           { 
//               ledStrip.clearingStrips(*it);
//               BaseType_t taskCreated;
//               TaskHandle_t newTaskHandle;
//               if(!checked)
//               {
//                 checked = 1;
//                 debugPrint.debugPrint("if(pinNum == 4 && !checked)");
//                   // Create an instance of the struct and populate its fields
//                 Task3Parameters params = {pinNum, &checked};
//                 xTaskCreatePinnedToCore(HandlingPinInterrupt, "Task3" ,  10000 ,   (void*)&params ,   2    , &Task3,  1);
              
//               } 
//           }
//           delay(10);
//     }
// }

// void HandlingPinInterrupt(void *pvParameters)
// { 
//   Task3Parameters *params = (Task3Parameters *)pvParameters;
//   int pinNum = params->pinNum;
//   int *checked = params->checked;
//   char buffer[2];
//   itoa(pinNum, buffer, 10);
//   String pinNumber = String(buffer);
//   String currentAnimationFile = "/PIN" + pinNumber + ".ani";
//   vTaskSuspend(Task1);
//   ledStrip.clearingStrips(*it);
//   while (1)
//   { 
//       playAnimation(currentAnimationFile , pinNum);
//       int readPin = digitalRead(pinNum);
//       if(readPin == 1)
//       { 
//           break;
//       }
//   }
//   *checked = 0;
//   ledStrip.clearingStrips(*it);
//   vTaskResume(Task1);
//   vTaskDelete(NULL);
// }

void loop() 
{ 
  delay(10);
}

// bool ifDelta = false;
// void playAnimation(String filename , int pinNum) 
// {  
//   File file = SPIFFS.open(filename.c_str());
//   size_t currentPosition = 0;
//   if (!file) 
//   {
//     return;
//   } 
//   else 
//   {
//     debugPrint.debugPrint("Succesfully opened animation file: " + filename);
//   }
//   while (file.available()) 
//   { 
//     int readPin;
//     if(pinNum == -1)
//     {
//       readPin == 0;
//     }
//     else
//     {
//       readPin = digitalRead(pinNum);
//     }
//     size_t position = file.position();
//     String line = file.readStringUntil('\n');
//     line.trim();
//     if (!line.startsWith("STOP")) 
//     { 
//       currentPosition = position;
//     }
//     if(readPin == 1)
//     {
//       return;
//     }
//     if (line.startsWith("L:")) 
//     {
//       NUM_LEDS = line.substring(2).toInt();
//       (*it)->strip->updateLength(NUM_LEDS);
//       (*it)->strip->show();  // Update the strip length and clear it
//       continue;
//     }

//     if (line.endsWith(".ani")) 
//     {
//       nextAnimationFile = "/" + line;
//       break;  // Exit out of the while loop to let the main loop handle the file switch
//     }
    
//     if (line.startsWith("STOP")) 
//     { 
//       file.seek(currentPosition);
//       parseAndUpdateStrip(line, stripList , 0);
//     } 
//     else if (line.startsWith("PAUSE")) 
//     {
//       int pauseDuration = line.substring(6).toInt();
//       delay(pauseDuration);
//       continue;
//     } 
//     else if (line.indexOf('|') != -1) 
//     {
//       int dividerPos = line.indexOf('|');
//       int customDuration = line.substring(0, dividerPos).toInt();
//       parseAndUpdateStrip(line.substring(dividerPos + 1), stripList , 0 );
//       delay(customDuration);
//     } 
//     else if (line.indexOf('~') != -1)
//     {
//       int dividerPos = line.indexOf('~');
//       int customDuration = line.substring(0, dividerPos).toInt();
//       ifDelta = true;
//       parseAndUpdateStrip(line.substring(dividerPos + 1), stripList, customDuration);
//       continue;
//     }
//     else 
//     {
//       parseAndUpdateStrip(line, stripList , 0);
//       delay(100);
//     }
//   }
//   file.close();
// }

// void parseAndUpdateStrip(String line, std::list<ledStripInfo *> &stripList , int costumDuration) 
// { 
//   bool stopCommand = false;
//   if(line == "STOP")
//   {
//     stopCommand = true;
//   }
//   it = stripList.begin();
//   int ledIdx = 0;

//   for (int i = 0; i < line.length(); i++) 
//   {
//     if (line[i] == ';') 
//     {
//       ledIdx++;
//       if (ledIdx >= (*it)->len) 
//       {
//         it++;
//         if (it == stripList.end()) 
//         {
//           goto showStrip;
//         }
//         ledIdx = 0;
//       }
//       continue;
//     }

//     if (line[i] == '-') 
//     {
      
//       int dashEnd = line.indexOf('>', i);
      
//       String repeatValue = line.substring(i + 1, dashEnd);
//       int repeatCount = repeatValue.toInt();
//       char colorCode = line[i - 1];

//       if (repeatCount) 
//       {
//         for (int j = 0; j < repeatCount; j++) 
//         {
//           if (ledIdx >= (*it)->len) 
//           {
//             it++;
//             if (it == stripList.end()) 
//             {
//               goto showStrip;
//             }
//             ledIdx = 0;
//           }
//           colors.setPixelColorFromCode(ledIdx, colorCode, colors.colorInstance, *it, line);
//           ledIdx++;
//         }
//         i = dashEnd;
        
//       } 
//       else 
//       {
//         for (; it != stripList.end(); it++) 
//         {
//           for (int j = ledIdx; j < ((*it)->len); j++) 
//           {
//             colors.setPixelColorFromCode(j, colorCode, colors.colorInstance, *it, line);
//           }
//           ledIdx = 0;
//         }
//         i = line.length();
//       }
//     } 

//     else 
//     {
//       if (ledIdx >= (*it)->len) 
//       {
//         it++;
//         ledIdx = 0;
//         if (it == stripList.end()) 
//         {
//           break;
//         }
//       }
//       if(ifDelta)
//       { 
//         uint8_t getBrightnessValue = (*it)->strip->getBrightness();
//         float divideRatioValue = 0;
//         if(costumDuration)
//         { 
//           divideRatioValue = ((float)getBrightnessValue)/costumDuration;   //costumDuration
//         }
//         else
//         { 
//           divideRatioValue = ((float)getBrightnessValue)/100;
//         }
//         unsigned long startTime;
//         unsigned long measureTime = 0;
//         startTime = millis();
//         for(float i = getBrightnessValue ; i >= 0;)
//         { 
          
//           for (it = stripList.begin(); it != stripList.end(); it++)
//           {
//            (*it)->strip->setBrightness(i);
//           }
//           ledStrip.showingStrips(*it);
//           measureTime = millis() - startTime;
//           i = getBrightnessValue - (divideRatioValue * measureTime);
//         }
//         // unsigned long endTime = millis();
//         // unsigned long loopTime = endTime - startTime;
//         // Serial.println(loopTime);
//         ifDelta = false;
//         for (it = stripList.begin(); it != stripList.end(); it++)
//         {
//           (*it)->strip->setBrightness(getBrightnessValue);
//         }
//         return;
//       }
//       else if(stopCommand)
//       {  
//         for (std::list<ledStripInfo *>::iterator it = stripList.begin(); it != stripList.end(); it++) 
//         {
//           (*it)->strip->clear();
//         }
//       }
//       else
//       {
//         colors.setPixelColorFromCode(ledIdx, line[i], colors.colorInstance, *it, line);
//         ledIdx++;
//       }
//     }
//   }
// showStrip:
//   ledStrip.showingStrips(*it);
//   if(stopCommand)
//   {
//     delay(500);
//   }
//   it = stripList.begin();
// }

// void showingStrips(ledStripInfo *strip)
// { 
//   for (std::list<ledStripInfo *>::iterator it = stripList.begin(); it != stripList.end(); it++) 
//   {
//     (*it)->strip->show();
//   }
// }

