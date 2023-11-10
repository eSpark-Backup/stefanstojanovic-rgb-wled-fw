#include "Parsing.h"
#include <list>

int pin = 0;
uint16_t length = 0;
auto skip = 0;
int brightness = 0;
uint8_t combination = 0;
const char* getType;
Types compare[] = {
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


void parseSection(String sectionName, File configFile, std::list<adaFruitPack*>& stripsList) 
{
  while (configFile.available()) 
  {
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
      pin = line.substring(line.indexOf("P:") + 2, line.indexOf(" T:")).toInt();
      String gettingType = line.substring(line.indexOf("T:") + 2, line.indexOf(" L:"));
      length = line.substring(line.indexOf("L:") + 2, line.indexOf(" S:")).toInt();
      skip = line.substring(line.indexOf("S:") + 2, line.indexOf(" B:")).toInt();
      brightness = line.substring(line.indexOf("B:") + 2).toInt();
      getType = gettingType.c_str();  //Converting string to const char* in order to use strcmp() function
      for (Types typ : compare) 
      {
        if (strcmp(getType, typ.typeStr) == 0) 
        {
          combination = typ.type;
        }
      }
      
      if (sectionName == "strips") 
      {
        adaFruitPack* ada = new adaFruitPack;
        ada->strips = new Adafruit_NeoPixel(length, pin, combination + NEO_KHZ800);
        ada->len = length;
        ada->gettingTypeString = getType;
        ada->strips->begin();  // Initialize the strip
        ada->strips->setBrightness(brightness);
        stripsList.push_back(ada);
      } 
      else if (sectionName == "input") 
      {
        pinMode(pin, INPUT_PULLUP);
      }
      else if (sectionName == "output") 
      {
       
        String statePinStr = line.substring(line.indexOf("S:") + 2);
        if(statePinStr[0] == 'H')
        {
          
           pinMode(pin, OUTPUT);
           digitalWrite(pin,HIGH);
        }
        else if (statePinStr[0] == 'L')
        {  
           pinMode(pin, OUTPUT);
           digitalWrite(pin,LOW);
        }
        

      }
      // Serial.print(sectionName + " - Pin: ");
      // Serial.print(pin);
      // Serial.print(", Type: ");
      // Serial.print(getType);
      // Serial.print(", Length: ");
      // Serial.print(length);
      // Serial.print(", Skip: ");
      // Serial.print(skip);
      // Serial.print(", Brightness: ");
      // Serial.println(brightness);
    }
  }
}

// uint8_t typeCheck(uint8_t combination , struct Types compare[])
// {
//   for (Types typ : compare) 
//   {
//     if (strcmp(getType, typ.typeStr) == 0) 
//     {
//       combination = typ.type;
//       return combination;
//     }
//   }
// }


