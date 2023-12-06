#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>

typedef struct 
{
  int pinNum;
  int *checked;
  
} Task3Parameters;

class InputHandle
{
  private:
     TaskHandle_t Task2; 
     static void CheckingPinInterrupt(void* pvParameters);
  public:
      static InputHandle& getInstance();
      int PinGetPullDown();
      void startTasks()
      {
         xTaskCreatePinnedToCore(CheckingPinInterrupt, "Task2" ,  10000 ,   NULL        ,    3     , &Task2,     1);
      }
};