#include "inputhandle.hpp"
#include "parsing.hpp"
#include "ledstrip.hpp"
#include "debugprint.hpp"

namespace 
{
    InputHandle* inputHandleInstance = nullptr;
}

InputHandle& InputHandle::getInstance() 
{
    if(nullptr == inputHandleInstance)
    {
        inputHandleInstance = new InputHandle();
    }
    return *inputHandleInstance;
}

static Parsing& parsing = Parsing::getInstance();
static LedStrip& ledStrip = LedStrip::getInstance();
static DebugPrint& debugPrint = DebugPrint::getInstance();
static InputHandle& inputHandle = InputHandle::getInstance();

void InputHandle::CheckingPinInterrupt(void *pvParameters)
{   
    int checked = 0;
    while (1)
    { 
        int pinNum = inputHandle.PinGetPullDown();
   
        if(pinNum)
        {   
            ledStrip.clearingStrips(*(ledStrip.it));  // Pass nullptr or the appropriate ledStripInfo pointer
            BaseType_t taskCreated;
            TaskHandle_t newTaskHandle;
            if(!checked)
            {     
                checked = 1;
                debugPrint.debugPrint("if(pinNum == 4 && !checked)");
                // Create an instance of the struct and populate its fields
                Task3Parameters params = {pinNum, &checked};
                xTaskCreatePinnedToCore(ledStrip.HandlingPinInterrupt, "Task3", 10000, (void*)&params, 2, &(ledStrip.Task3), 1);

            } 
        }
        delay(10);
    }
}

int InputHandle::PinGetPullDown()
{ 
  for (std::list<int>::iterator it = parsing.pinsList.begin(); !(it == parsing.pinsList.end()); ++it)
  { 
    int readPinValueReturn = digitalRead(*it);
    if(0 == readPinValueReturn)
    { 
      return *it;
    }
  }
   return 0;
}