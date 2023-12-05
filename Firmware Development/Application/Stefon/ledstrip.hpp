#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include <mutex>
#include "tasks.hpp"

// typedef struct 
// {
//   Adafruit_NeoPixel *strip;
//   uint16_t len;
//   const char *type;
// } ledStripInfo;

class LedStrip
{
    private:
    
      TaskHandle_t Task1;
      TaskHandle_t Task2; 
      TaskHandle_t Task3;
      static void PlayingNormalRoutine(void* pvParameters);
      static void CheckingPinInterrupt(void* pvParameters);
      static void HandlingPinInterrupt(void* pvParameters);

    public:

      bool ifDelta = false;
      String nextAnimationFile = "";
      std::list<ledStripInfo *> stripList;
      std::list<ledStripInfo *>::iterator it;
      
      void startTasks() 
      {
            xTaskCreatePinnedToCore(PlayingNormalRoutine, "Task1" ,  10000 ,   NULL        ,    1      , &Task1,     1);    
                //                     func       name    stack    parameters     task          task       core of the task
              //                                         Size     of the task    pariority     handler
            
            xTaskCreatePinnedToCore(CheckingPinInterrupt, "Task2" ,  10000 ,   NULL        ,    3     , &Task2,     1);
      }

      int PinGetPullDown();
      void playAnimation(String filename , int pinNum); 
      void parseAndUpdateStrip(String line, std::list<ledStripInfo *> &stripList , int costumDuration);
      static LedStrip& getInstance();
      void clearingStrips(ledStripInfo *strip);
      void showingStrips(ledStripInfo *strip);

};
