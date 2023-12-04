#include "debugprint.hpp"

// bool DebugPrint::isDebugPrint()
// {
//     if(1 == parseCurrent.debug)
//     {
//       return true;
//     }
//     else
//     {
//       return false;
//     }
// }
namespace {
    DebugPrint* debugPrintInstance = new DebugPrint();
}

DebugPrint& DebugPrint::getInstance() {
    return *debugPrintInstance;
}


void DebugPrint::debugPrint(String DebugText) 
{
   if (1 == debugValue) Serial.println(DebugText);
}



