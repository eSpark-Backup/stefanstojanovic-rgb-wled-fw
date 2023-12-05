#include "debugprint.hpp"

namespace 
{
    DebugPrint* debugPrintInstance = nullptr;
}

DebugPrint& DebugPrint::getInstance() 
{
    if(nullptr == debugPrintInstance)
    {
        debugPrintInstance = new DebugPrint();
    }
    return *debugPrintInstance;
}


void DebugPrint::debugPrint(String DebugText) 
{
   if (1 == debugValue) Serial.println(DebugText);
}



