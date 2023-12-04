#include "ledstrip.hpp"
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