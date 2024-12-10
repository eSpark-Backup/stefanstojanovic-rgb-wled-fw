#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.hpp"
#include "colors.hpp"
#include "debugprint.hpp"
#include "ledstrip.hpp"
#define COLORS_PRINT

static Colors& colors = Colors::getInstance();
static LedStrip& ledStrip = LedStrip::getInstance();
static Parsing& parsing = Parsing::getInstance();

void setup() 
{ 
      Serial.begin(115200);
      SPIFFS.begin();
      pinMode(25, OUTPUT);
      pinMode(33, OUTPUT);
        digitalWrite(25, LOW);
        digitalWrite(33, LOW);
      colors.LineCounter("/colortable.conf" , colors.lineCounter);
      colors.colorInstance.colorArray = new ColorConfig[colors.lineCounter];
      parsing.parseAndPrintConfig("/colortable.conf");
      File configFile = SPIFFS.open("/config.conf", "r");
      parsing.parseSection("", configFile, ledStrip.stripList);  // Pass an empty section name to state
      #ifndef COLORS_PRINT
      // printing in order to check whether the colors have been parsed or not
      for (int i = 0; i<colors.lineCounter ; i++)
      {
          printf("symbol %c color combination %d, %d, %d, %d\n " , colors.colorInstance.colorArray[i].symbol , colors.colorInstance.colorArray[i].r , colors.colorInstance.colorArray[i].g , colors.colorInstance.colorArray[i].b , colors.colorInstance.colorArray[i].w);
      }
      #endif
      ledStrip.startTasks();
}

void loop() 
{ 
    delay(5);
}



