
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.hpp"
#include "colors.hpp"
// extern std::list<int> pinsList;
// int PinGetPullDown()
// { 
//   for (std::list<int>::iterator it = pinsList.begin(); !(it == pinsList.end()); ++it)
//   { 
//     int readPinValueReturn = digitalRead(*it);
//     if(0 == readPinValueReturn)
//     { 
//       return *it;
//     }
//   }
//    return 0;
// }
