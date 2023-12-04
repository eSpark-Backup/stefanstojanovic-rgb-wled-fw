// #include "tasks.hpp"
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.h"
#include "strips.hpp"
extern std::list<int> pinsList;
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

// void PlayingNormalRoutine(void *pvParameters)
// {
//     String currentAnimationFile = "/startup.ani";
//     while (1)
//     { 
//         if (nextAnimationFile != "")
//         {
//             currentAnimationFile = nextAnimationFile;
//             String nextAnimationFile = "";
//         }
//         playAnimation(currentAnimationFile , -1);
//         delay(100);
//     }
// }

// void CheckingPinInterrupt(void *pvParameters)
// { 
//   int checked = 0;
//   while (1)
//   { 
//       Serial.print("checked: ");
//       Serial.println(checked);
//       int pinNum = PinGetPullDown();
//       if(pinNum)
//       { 
//           clearingStrips(*it);
//           BaseType_t taskCreated;
//           TaskHandle_t newTaskHandle;
//           if(!checked)
//           {
//               checked = 1;
//               debugPrint("if(pinNum == 4 && !checked)");
//               // Create an instance of the struct and populate its fields
//               Task3Parameters params = {pinNum, &checked};
//               xTaskCreatePinnedToCore(HandlingPinInterrupt, "Task3" ,  10000 ,   (void*)&params ,   2    , &Task3,  1);
              
          
//           } 
//       }
//       delay(10);
//     }
// }

// void HandlingPinInterrupt(void *pvParameters)
// { 
//     Task3Parameters *params = (Task3Parameters *)pvParameters;
//     int pinNum = params->pinNum;
//     int *checked = params->checked;
//     char buffer[2];
//     itoa(pinNum, buffer, 10);
//     String pinNumber = String(buffer);
//     String currentAnimationFile = "/PIN" + pinNumber + ".ani";
//     vTaskSuspend(Task1);
//     clearingStrips(*it);
//     while (1)
//     { 
//         playAnimation(currentAnimationFile , pinNum);
//         int readPin = digitalRead(pinNum);
//         if(readPin == 1)
//         { 
//             break;
//         }
//     }
//     *checked = 0;
//     clearingStrips(*it);
//     vTaskResume(Task1);
//     vTaskDelete(NULL);
// }