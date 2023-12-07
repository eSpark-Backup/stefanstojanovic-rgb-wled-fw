#include "ledstrip.hpp"
#include "debugprint.hpp"
#include "colors.hpp"
#include "parsing.hpp"


static Parsing& parsing = Parsing::getInstance();
static DebugPrint& debugPrint = DebugPrint::getInstance();
static Colors& colors = Colors::getInstance();

namespace 
{
    LedStrip* LedStripInstance = nullptr;
}


LedStrip& LedStrip::getInstance() 
{
    if(nullptr == LedStripInstance)
    {
      LedStripInstance = new LedStrip();
    }
    return *LedStripInstance;
}
static LedStrip& ledStrip = LedStrip::getInstance();
int LedStrip::PinGetPullDown()
{ 
  for (std::list<int>::iterator it = parsing.pinsList.begin(); !(it == parsing.pinsList.end()); ++it)
  { 
    int readPinValueReturn = digitalRead(*it);
    if(0 == readPinValueReturn)
    { 
      return *it;
    }
  }
   return 0;
}

void LedStrip::showingStrips(ledStripInfo *strip)
{ 
  for (std::list<ledStripInfo *>::iterator it = stripList.begin(); it != stripList.end(); it++) 
  {
    (*it)->strip->show();
  }
}

void LedStrip::clearingStrips(ledStripInfo *strip)
{
    for (it = stripList.begin(); it != stripList.end(); it++) 
    {
        (*it)->strip->clear();
        (*it)->strip->show();
    }
}

void LedStrip::PlayingNormalRoutine(void *pvParameters)
{   
    String currentAnimationFile = "/startup.ani";
    while (1)
    { 
      
        if (ledStrip.nextAnimationFile != "")
        {                
            currentAnimationFile = ledStrip.nextAnimationFile;
            ledStrip.nextAnimationFile = "";
        }

        ledStrip.playAnimation(currentAnimationFile, -1);

        delay(100);

    }
}

void LedStrip::CheckingPinInterrupt(void *pvParameters)
{   
  
    while (1)
    { 
        int pinNum = ledStrip.PinGetPullDown();
   
        if(pinNum)
        {   
            ledStrip.clearingStrips(*(ledStrip.it));  
            debugPrint.debugPrint("if(pinNum == 4 && !checked)");
            // Create an instance of the struct and populate its fields
            ledStrip.HandlingPinInterrupt(pinNum); 
        }
        delay(10);
    }
}

void LedStrip::HandlingPinInterrupt(int pinNum)
{   
    char buffer[2];
    itoa(pinNum, buffer, 10);
    String pinNumber = String(buffer);
    String currentAnimationFile = "/PIN" + pinNumber + ".ani";
    vTaskSuspend(ledStrip.Task1);
    ledStrip.clearingStrips(*(ledStrip.it));  
    while (1)
    { 
        ledStrip.playAnimation(currentAnimationFile, pinNum);
        int readPin = digitalRead(pinNum);
        if(1 == readPin)
        { 
            break;
        }
    }
    ledStrip.clearingStrips(*(ledStrip.it));  
    vTaskResume(ledStrip.Task1);
}

void LedStrip::playAnimation(String filename , int pinNum) 
{  

      File file = SPIFFS.open(filename.c_str());
      size_t currentPosition = 0;
      if (!file) 
      {
          return;
      } 
      else 
      {
          debugPrint.debugPrint("Succesfully opened animation file: " + filename);
      }
      while (file.available()) 
      { 
            int readPin;
            if(pinNum == -1)
            {
                readPin == 0;
            }

            else
            {
                readPin = digitalRead(pinNum);
            }
                
            size_t position = file.position();
            String line = file.readStringUntil('\n');
            line.trim();
            if (!line.startsWith("STOP")) 
            { 
                currentPosition = position;
            }
            if(readPin == 1)
            {
                return;
            }
            if (line.startsWith("L:")) 
            {
                int numLeds = line.substring(2).toInt();
                (*it)->strip->updateLength(numLeds);
                (*it)->strip->show();  // Update the strip length and clear it
                continue;
            }

            if (line.endsWith(".ani")) 
            {
                nextAnimationFile = "/" + line;
                break;  // Exit out of the while loop to let the main loop handle the file switch
            }
            
            if (line.startsWith("STOP")) 
            { 
                file.seek(currentPosition);
                parseAndUpdateStrip(line, stripList , 0);
            } 
            else if (line.startsWith("PAUSE")) 
            {
                int pauseDuration = line.substring(6).toInt();
                delay(pauseDuration);
                continue;
            } 
            else if (line.indexOf('|') != -1) 
            {     
                int dividerPos = line.indexOf('|');
                int customDuration = line.substring(0, dividerPos).toInt();
                parseAndUpdateStrip(line.substring(dividerPos + 1), stripList , 0 );

                delay(customDuration);

            } 
            else if (line.indexOf('~') != -1)
            {
                int dividerPos = line.indexOf('~');
                int customDuration = line.substring(0, dividerPos).toInt();
                ifDelta = true;
                parseAndUpdateStrip(line.substring(dividerPos + 1), stripList, customDuration);
                continue;
            }
            else 
            {  
                parseAndUpdateStrip(line, stripList , 0);
                delay(100);
            }
      }
      file.close();
}

void LedStrip::parseAndUpdateStrip(String line, std::list<ledStripInfo *> &stripList , int costumDuration) 
{ 
  
  bool stopCommand = false;
  if(line == "STOP")
  {
     stopCommand = true;
  }
   

  it = stripList.begin();   // initializing iterator from 0


  int ledIdx = 0;
  for (int i = 0; i < line.length(); i++) 
  {
      if (line[i] == ';') 
      {
        ledIdx++;
        if (ledIdx >= (*it)->len) 
        {
            it++;
            if (it == stripList.end()) 
            {
               goto showStrip;
            }
            ledIdx = 0;
        }
        continue;
    }

    if (line[i] == '-') 
    {
      
        int dashEnd = line.indexOf('>', i);
        
        String repeatValue = line.substring(i + 1, dashEnd);
        int repeatCount = repeatValue.toInt();
        char colorCode = line[i - 1];

        if (repeatCount) 
        {
            for (int j = 0; j < repeatCount; j++) 
            {
                if (ledIdx >= (*it)->len) 
                {
                    it++;
                    if (it == stripList.end()) 
                    {
                        goto showStrip;
                    }
                    ledIdx = 0;
                }
                colors.setPixelColorFromCode(ledIdx, colorCode, colors.colorInstance, *it, line);
                ledIdx++;
            }
            i = dashEnd;
          
        } 
        else 
        {
            for (; it != stripList.end(); it++) 
            {
                for (int j = ledIdx; j < ((*it)->len); j++) 
                {
                    colors.setPixelColorFromCode(j, colorCode, colors.colorInstance, *it, line);
                }
                ledIdx = 0;
            }
            i = line.length();
        }
    } 

    else 
    {    
        if (ledIdx >= (*it)->len) // If led index will exceed from the length of the strip (that is being defined in the .config file), getting the remaing line of the animation to the next Led Strip which is connected to anothe GPIO 
        {   
            it++;
            ledIdx = 0;
            if (it == stripList.end()) 
            {
              break;
            }
        }

        if(ifDelta) // Fading logic is implemented in this condition
        { 
            uint8_t getBrightnessValue = (*it)->strip->getBrightness();
            float divideRatioValue = 0;
            if(costumDuration)
            { 
                divideRatioValue = ((float)getBrightnessValue)/costumDuration;   //if costumDuration is defined in the .ani file
            }
            else
            { 
                divideRatioValue = ((float)getBrightnessValue)/100;  //If costum duration is not defined then takin 100ms as default
            }
            unsigned long startTime;
            unsigned long measureTime = 0;
            startTime = millis();
            for(float i = getBrightnessValue ; i >= 0;)
            { 
              
                for (it = stripList.begin(); it != stripList.end(); it++)
                {
                    (*it)->strip->setBrightness(i);
                }
                showingStrips(*it);
                measureTime = millis() - startTime;
                i = getBrightnessValue - (divideRatioValue * measureTime);
            }
            ifDelta = false;
            for (it = stripList.begin(); it != stripList.end(); it++)
            {
                (*it)->strip->setBrightness(getBrightnessValue);
            }
            return;
        }
        else if(stopCommand)
        {  
            for (std::list<ledStripInfo *>::iterator it = stripList.begin(); it != stripList.end(); it++) 
            {
                (*it)->strip->clear();
            }
        }
        else
        {
            colors.setPixelColorFromCode(ledIdx, line[i], colors.colorInstance, *it, line);
            ledIdx++;
        }
    }
  }
showStrip:
  showingStrips(*it);
  if(stopCommand)
  {
      delay(500);
  }
  it = stripList.begin();
}
