// #include "strips.hpp"
// std::list<ledStripInfo *> stripList;
// void clearingStrips(ledStripInfo *strip)
// {
//     for (it = stripList.begin(); it != stripList.end(); it++) 
//     {
//         (*it)->strip->clear();
//         (*it)->strip->show();
//     }
// }

// void parseAndUpdateStrip(String line, std::list<ledStripInfo *> &stripList , int costumDuration) 
// { 
//   bool stopCommand = false;
//   if(line == "STOP")
//   {
//     stopCommand = true;
//   }
//   it = stripList.begin();
//   int ledIdx = 0;

//   for (int i = 0; i < line.length(); i++) 
//   {
//     if (line[i] == ';') 
//     {
//       ledIdx++;
//       if (ledIdx >= (*it)->len) 
//       {
//         it++;
//         if (it == stripList.end()) 
//         {
//           goto showStrip;
//         }
//         ledIdx = 0;
//       }
//       continue;
//     }

//     if (line[i] == '-') 
//     {
      
//       int dashEnd = line.indexOf('>', i);
      
//       String repeatValue = line.substring(i + 1, dashEnd);
//       int repeatCount = repeatValue.toInt();
//       char colorCode = line[i - 1];

//       if (repeatCount) 
//       {
//         for (int j = 0; j < repeatCount; j++) 
//         {
//           if (ledIdx >= (*it)->len) 
//           {
//             it++;
//             if (it == stripList.end()) 
//             {
//               goto showStrip;
//             }
//             ledIdx = 0;
//           }
//           setPixelColorFromCode(ledIdx, colorCode, colorInstance, *it, line);
//           ledIdx++;
//         }
//         i = dashEnd;
        
//       } 

//       else 
//       {
//         for (; it != stripList.end(); it++) 
//         {
//           for (int j = ledIdx; j < ((*it)->len); j++) 
//           {
//             setPixelColorFromCode(j, colorCode, colorInstance, *it, line);
//           }
//           ledIdx = 0;
//         }
//         i = line.length();
//       }
//     } 

//     else 
//     {
//       if (ledIdx >= (*it)->len) 
//       {
//         it++;
//         ledIdx = 0;
//         if (it == stripList.end()) 
//         {
//           break;
//         }
//       }
//       if(ifDelta)
//       { 
//         uint8_t getBrightnessValue = (*it)->strip->getBrightness();
//         float divideRatioValue = 0;
//         if(costumDuration)
//         { 
//           divideRatioValue = ((float)getBrightnessValue)/costumDuration;   //costumDuration
//         }
//         else
//         { 
//           divideRatioValue = ((float)getBrightnessValue)/100;
//         }
//         unsigned long startTime;
//         unsigned long measureTime = 0;
//         startTime = millis();
//         for(float i = getBrightnessValue ; i >= 0;)
//         { 
          
//           for (it = stripList.begin(); it != stripList.end(); it++)
//           {
//            (*it)->strip->setBrightness(i);
//           }
//           showingStrips(*it);
//           measureTime = millis() - startTime;
//           i = getBrightnessValue - (divideRatioValue * measureTime);
//         }
//         // unsigned long endTime = millis();
//         // unsigned long loopTime = endTime - startTime;
//         // Serial.println(loopTime);
//         ifDelta = false;
//         for (it = stripList.begin(); it != stripList.end(); it++)
//         {
//           (*it)->strip->setBrightness(getBrightnessValue);
//         }
//         return;
//       }
//       else if(stopCommand)
//       {  
//         for (std::list<ledStripInfo *>::iterator it = stripList.begin(); it != stripList.end(); it++) 
//         {
//           (*it)->strip->clear();
//         }
//       }
//       else
//       {
//         setPixelColorFromCode(ledIdx, line[i], colorInstance, *it, line);
//         ledIdx++;
//       }
//     }
//   }
// showStrip:
//   showingStrips(*it);
//   if(stopCommand)
//   {
//     delay(500);
//   }
//   it = stripList.begin();
// }