#include "ledstrip.hpp"
#include "debugprint.hpp"
#include "colors.hpp"
#include "parsing.hpp"
#include "benchmark.hpp"

static Parsing& parsing = Parsing::getInstance();
static DebugPrint& debugPrint = DebugPrint::getInstance();
static Colors& colors = Colors::getInstance();
QueueHandle_t ledStripQueue;
static int frame_delay = 0;

namespace 
{
    LedStrip* LedStripInstance = nullptr;
}


void LedStrip::startTasks() 
{
    ledStripQueue = xQueueCreate(1, sizeof(ledStripInfo *));
    xTaskCreatePinnedToCore(handleStripShow,      "FlushTask", 4096*10,  NULL        ,     2      , &Task3,    1);
    xTaskCreatePinnedToCore(PlayingNormalRoutine, "Task1" ,  10000*4 ,   NULL        ,    1      , &Task1,     1);    
        //                     func       name    stack    parameters     task          task       core of the task
        //                                          Size     of the task    pariority     handler
    xTaskCreatePinnedToCore(CheckingPinInterrupt, "Task2" ,  10000*4 ,   NULL        ,    3     , &Task2,     1);


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
  for (std::list<int>::iterator it = parsing.pinConfig.inputList.begin(); !(it == parsing.pinConfig.inputList.end()); ++it)
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
        ledStrip.clearingStrips(NULL);
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
            ledStrip.clearingStrips(NULL);  
            debugPrint.debugPrint("if(pinNum == 4 && !checked)");
            // Create an instance of the struct and populate its fields
            ledStrip.HandlingPinInterrupt(pinNum); 
        }
        delay(10);
    }
}

void LedStrip::handleStripShow(void *pvParameters)
{
    ledStripInfo* strip;
    Serial.println("Task created handleStripShow");
    while (true) {
        if (xQueueReceive(ledStripQueue, &strip, portMAX_DELAY) == pdPASS) {
                int flush_delay = millis();
                ledStrip.showingStrips(strip);
                flush_delay = millis() - flush_delay;
                int final_delay = frame_delay - flush_delay;
                if(final_delay > 0)
                {
                    delay(final_delay);
                }
        }        
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
            else if (line.indexOf("PIN") != -1)
            {
              int dividerPos = line.indexOf('|');
              int delayDuration = 0;
              String command;

              if (dividerPos != -1) {
                  delayDuration = line.substring(0, dividerPos).toInt();
                  command = line.substring(dividerPos + 1);
              } else {
                  command = line;
              }
              command.trim();

              if (command.startsWith("PIN")) {
                  int pin = -1;
                  String state;

                  // Extract pin and state from the command
                  int space1 = command.indexOf(' ', 3);  // Find the first space after "PIN"
                  int space2 = command.indexOf(' ', space1 + 1);

                  if (space1 != -1 && space2 != -1) {
                      pin = command.substring(space1 + 1, space2).toInt();
                      state = command.substring(space2 + 1);
                  }
                  bool pinFound = false;
                  for (int availablePin : parsing.pinConfig.outputList)
                  { 
                    if(pin == availablePin)
                    {
                      pinFound = true;
                      // Serial.println("Pin found in the list");
                      break;
                    }
                  }
                  // Handle the pin and state
                  if(pinFound)
                  {
                    if (pin != -1) {
                        if (state == "HIGH") {                         
                            // Serial.println("State Chanaged to HIGH");                 
                            pinMode(pin, OUTPUT);
                            digitalWrite(pin, HIGH);
                        } else if (state == "LOW") {           
                            // Serial.println("State Chanaged to LOW");                 
                            pinMode(pin, OUTPUT);
                            digitalWrite(pin, LOW);
                        }
                    }
                  }
                  else
                  {
                    Serial.print("Pin is not Configured in config.conf file :");
                    Serial.println(pin);
                  }
                  
                  if (delayDuration > 0) {
                    // Serial.println("requried Delay added");                 
                    delay(delayDuration);
                  }
              }
            }
            else if (line.indexOf('|') != -1) 
            {     
                int dividerPos = line.indexOf('|');
                int customDuration = line.substring(0, dividerPos).toInt();
                // int time_to_parse = 0;
                // MEASURE_TIME(parseAndUpdateStrip(line.substring(dividerPos + 1), stripList , 0 ), time_to_parse);
                parseAndUpdateStrip(line.substring(dividerPos + 1), stripList , 0 );
                frame_delay = customDuration;

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
                if(xQueueSend(ledStripQueue, &stripList, portMAX_DELAY) != pdPASS) {
                    // Handle the error (e.g., queue was full and timeout expired)
                    Serial.println("Queue send timed out");
                }
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
    while(xQueueSend(ledStripQueue, &stripList, pdMS_TO_TICKS(frame_delay)) != pdPASS) 
    { 
        Serial.println("Queue send timed out");
    }
    

  if(stopCommand)
  {
      delay(500);
  }
}
