#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>
#include "Parsing.hpp"
#include "colors.hpp"
#include "tasks.hpp"
#include "debugprint.hpp"
#include "ledstrip.hpp"

static DebugPrint& debugPrint = DebugPrint::getInstance();
static Colors& colors = Colors::getInstance();
static LedStrip& ledStrip = LedStrip::getInstance();
static Parsing& parsing = Parsing::getInstance();

void setup() 
{ 
      Serial.begin(115200);
      SPIFFS.begin();
      int lineCounter = 0;
      colors.LineCounter("/colortable.conf" , lineCounter);
      Serial.println(lineCounter);
      colors.colorInstance.colorArray = new ColorConfig[lineCounter];
      parsing.parseAndPrintConfig("/colortable.conf");
      File configFile = SPIFFS.open("/config.conf", "r");
      parsing.parseSection("", configFile, ledStrip.stripList);  // Pass an empty section name to state

      // printing in order to check whether the colors have been parsed or not
      // for (int i = 0; i<lineCounter ; i++)
      // {
      // printf("symbol %c color combination %d, %d, %d, %d\n " , colorInstance.colorArray[i].symbol , colorInstance.colorArray[i].r , colorInstance.colorArray[i].g , colorInstance.colorArray[i].b , colorInstance.colorArray[i].w);
      // }
       ledStrip.startTasks();
}

void loop() 
{ 
  delay(10);
}



