#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>

typedef struct {
    Adafruit_NeoPixel *strip;  // The strip ready for flushing
    uint16_t len;
    const char *type;
    int user_delay;

} ledStripInfo;

class LedStrip
{
    private:
    
      TaskHandle_t Task1;
      TaskHandle_t Task2; 
      TaskHandle_t Task3; 
      bool ifDelta = false;
      static void PlayingNormalRoutine(void* pvParameters);
      static void CheckingPinInterrupt(void* pvParameters);
      static void handleStripShow(void *pvParameters); 
    public:
      String nextAnimationFile = "";
      std::list<ledStripInfo *> stripList;
      std::list<ledStripInfo *>::iterator it; //iterator to the list of ledStripInfo
      
      void HandlingPinInterrupt(int pinNum);
      void startTasks();
      int PinGetPullDown();  // This function checks if the pin gets pull down
      static LedStrip& getInstance(); 
      void playAnimation(String filename , int pinNum); 
      void parseAndUpdateStrip(String line, std::list<ledStripInfo *> &stripList , int costumDuration);
      void clearingStrips(ledStripInfo *strip);
      void showingStrips(ledStripInfo *strip);

};
