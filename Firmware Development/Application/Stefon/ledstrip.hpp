#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>
#include <list>

typedef struct 
{
  Adafruit_NeoPixel *strip;
  uint16_t len;
  const char *type;
} ledStripInfo;

class LedStrip
{
   public:
      // void PlayingNormalRoutine(void *pvParameters); 
      static LedStrip& getInstance();
      void clearingStrips(ledStripInfo *strip);
      void showingStrips(ledStripInfo *strip);

};