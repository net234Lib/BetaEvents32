


// Version evenementielle de l'exemple de la lib SSD13Wire
// Gestion d'un afficheur OLED SSD1306  128 x 64
// 2 zones de text
// Gestion du clignotemente
//
// source : https://github.com/ThingPulse/esp8266-oled-ssd1306
//
#pragma once
//#include <Arduino.h>

#include "EventsManager32.h"

class evHandlerSSD1306 : private eventHandler_t {
public:
  evHandlerSSD1306() {};
  virtual void begin() override;
  virtual void handle() override;

  void setDsp1(const String & aText);
  void setDsp2(const String & aText);
  void refresh();

private:
  String dsp1;
  String dsp2;
  
  
 
};
