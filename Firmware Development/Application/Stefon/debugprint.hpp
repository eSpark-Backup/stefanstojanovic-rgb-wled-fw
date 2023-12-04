#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.hpp"
#include "colors.hpp"
#include "tasks.hpp"

// class DebugPrint
// {
//    public:
//     void debugPrint(String DebugText);
// bool isDebugPrint();
// };

void debugPrint(String DebugText);
bool isDebugPrint();

