#include "Parsing.h"
#include <list>

typedef struct{
  int pin = 0;
  uint16_t length = 0;
  int skip = 0;
  int brightness = 0;
}parseFileParameters;

std::list<int> pinsList;


ledStripMap stripTypes[] = {
  { "RGB", NEO_RGB },
  { "RBG", NEO_RBG },
  { "GRB", NEO_GRB },
  { "GBR", NEO_GBR },
  { "BRG", NEO_BRG },
  { "BGR", NEO_BGR },
  { "WRGB", NEO_WRGB },
  { "WRBG", NEO_WRBG },
  { "WGRB", NEO_WGRB },
  { "WGBR", NEO_WGBR },
  { "WBRG", NEO_WBRG },
  { "WBGR", NEO_WBGR },
  { "RWGB", NEO_RWGB },
  { "RWBG", NEO_RWBG },
  { "RGWB", NEO_RGWB },
  { "RGBW", NEO_RGBW },
  { "RBWG", NEO_RBWG },
  { "RBGW", NEO_RBGW },
  { "GWRB", NEO_GWRB },
  { "GWBR", NEO_GWBR },
  { "GRWB", NEO_GRWB },
  { "GRBW", NEO_GRBW },
  { "GBWR", NEO_GBWR },
  { "GBRW", NEO_GBRW },
  { "BWRG", NEO_BWRG },
  { "BWGR", NEO_BWGR },
  { "BRWG", NEO_BRWG },
  { "BRGW", NEO_BRGW },
  { "BGWR", NEO_BGWR },
  { "BGRW", NEO_BGRW }
};

uint8_t typeCheck(uint8_t *combination , struct ledStripMap (&stripTypes)[30] , const char *getType)
{ 
  ledStripMap ifNotGetAnyValue;
  for (ledStripMap typ : stripTypes) 
  {
    if (strcmp(getType, typ.type) == 0) 
    {
      *combination = typ.value;
      return *combination;
    }
  }
  if(!(*combination))
  {
    Serial.println("else of the function:" );
    ifNotGetAnyValue.type = "RGB";
    *combination = ifNotGetAnyValue.value;
    return *combination;
  }
}

void parseSection(String sectionName, File configFile, std::list<ledStripInfo*>& stripList) 
{
  while (configFile.available()) 
  { 
    parseFileParameters parseCurrent;
    String line = configFile.readStringUntil('\n');
    line.trim();

    if (line.startsWith("#")) 
    {
      // Skip comments
      continue;
    }

    if (line.startsWith("[")) 
    {
      sectionName = line.substring(1, line.length() - 1);
      // Print section name

    } 
    else if (line.startsWith("P:")) 
    { 
      parseCurrent.pin = line.substring(line.indexOf("P:") + 2, line.indexOf(" T:")).toInt();
      String gettingType = line.substring(line.indexOf("T:") + 2, line.indexOf(" L:"));
      parseCurrent.length = line.substring(line.indexOf("L:") + 2, line.indexOf(" S:")).toInt();
      parseCurrent.skip = line.substring(line.indexOf("S:") + 2, line.indexOf(" B:")).toInt();
      parseCurrent.brightness = line.substring(line.indexOf("B:") + 2).toInt();
      const char* getType = gettingType.c_str();  //Converting string to const char* in order to use strcmp() function
      uint8_t combination;

      if (sectionName == "strips") 
      { 
        typeCheck(&combination , stripTypes , getType);
        ledStripInfo* ada = new ledStripInfo;
        ada->strip = new Adafruit_NeoPixel(parseCurrent.length, parseCurrent.pin, combination + NEO_KHZ800);
        ada->len = parseCurrent.length;
        ada->type = getType;
        ada->strip->begin();  // Initialize the strip
        ada->strip->setBrightness(parseCurrent.brightness);
        stripList.push_back(ada);
      } 
      else if (sectionName == "input") 
      { 
        pinsList.push_back(parseCurrent.pin);
        pinMode(parseCurrent.pin, INPUT_PULLUP);
        delay(50);
      }
      else if (sectionName == "output") 
      {
        String statePinStr = line.substring(line.indexOf("S:") + 2);
        if(statePinStr[0] == 'H')
        {
          pinMode(parseCurrent.pin, OUTPUT);
          digitalWrite(parseCurrent.pin,HIGH);
        }
        else if (statePinStr[0] == 'L')
        {  
          pinMode(parseCurrent.pin, OUTPUT);
          digitalWrite(parseCurrent.pin,LOW);
        }
      }
    }
  }
}



