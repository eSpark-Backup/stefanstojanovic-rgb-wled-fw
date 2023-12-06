#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>


class DebugPrint
{  
   public:
    uint8_t debugValue = 0;
    static DebugPrint& getInstance();  // Singleton accessor
    void debugPrint(String DebugText);
};



