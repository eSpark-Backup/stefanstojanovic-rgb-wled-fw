#include "debugprint.hpp"
// uint8_t debug = 0;
extern parseFileParameters parseCurrent;

bool isDebugPrint()
{
    if(1 == parseCurrent.debug)
    {
      return true;
    }
    else
    {
      return false;
    }
}
void debugPrint(String DebugText) 
{
   if (isDebugPrint()) Serial.println(DebugText);
}

