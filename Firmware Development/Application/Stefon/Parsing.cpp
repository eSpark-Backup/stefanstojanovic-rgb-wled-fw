#include "Parsing.hpp"
#include "debugprint.hpp"
#include <list>
#include "colors.hpp"
#include "ledstrip.hpp"

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

namespace 
{
    Parsing* parsingInstance = nullptr;
}

Parsing& Parsing::getInstance() 
{
    if(nullptr == parsingInstance)
    {
        parsingInstance = new Parsing();
    }
    return *parsingInstance;
}

static DebugPrint& debugPrint = DebugPrint::getInstance();
static Colors& colors = Colors::getInstance();
static LedStrip& ledStrip = LedStrip::getInstance();

void Parsing::parseAndPrintConfig(const char *filePath) 
{ 

    if (!SPIFFS.begin(true)) 
    {
        debugPrint.debugPrint("Failed to mount file system");
        return;
    }

    File configFile = SPIFFS.open(filePath, "r");

    if (!configFile) 
    {
        Serial.println("Failed to open config file");
        return;
    }

    // Parse and print the configuration
    while (configFile.available()) 
    { 
        String line = configFile.readStringUntil('\n');
        
        if (line.length() > 0) 
        {              
            if(parseConfigLine(line, colors.config.symbol, colors.config.r, colors.config.g, colors.config.b , colors.config.w))
            {
                colors.storeColorConfig(colors.colorInstance , colors.config);
            }
        }
    }
    configFile.close();
} 

bool Parsing::parseConfigLine(String line, char &symbol, uint8_t &r, uint8_t &g, uint8_t &b , uint8_t &w) 
{
    int count = sscanf(line.c_str(), "%c: %d,%d,%d,%d", &symbol, &r, &g, &b, &w);
    return (count == 4 || count == 5);
}

uint8_t Parsing::typeCheck(uint8_t *combination , struct ledStripMap (&stripTypes)[30] , const char *getType)
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

parseFileParameters parseCurrent;

void Parsing::parseSection(String sectionName, File configFile, std::list<ledStripInfo*>& stripList) 
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
            pinMode(parseCurrent.pin, INPUT_PULLUP);
            pinsList.push_back(parseCurrent.pin);
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
    else if (sectionName == "debug")
    { 
        // Parse debug information
        if (line.startsWith("debug="))
        {
            debugPrint.debugValue = line.substring(line.indexOf("=") + 1).toInt();
        }
    }
  } 
}



