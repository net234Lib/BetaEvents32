


// Version evenementielle de l'exemple de la lib SSD13Wire
// Gestion d'un afficheur OLED SSD1306  128 x 64
// 2 zones de text
// 1 zone graphique
// TODO: Gestion du clignotemente
//
// source : https://github.com/ThingPulse/esp8266-oled-ssd1306
//  FONT:  // Font Demo1
  // create more fonts at http://oleddisplay.squix.ch/

  
#pragma once
#include "EventsManager32.h"


//Graphique type MONO
struct imageMap_t {
  imageMap_t() : width(0),height(0) {};
  imageMap_t(const uint16_t width,const uint16_t height,const uint8_t* data) : width(width),height(height),data(data) {};
  uint16_t width;
  uint16_t height;
  const uint8_t* data;
};

class evHandlerSSD1306 : private eventHandler_t {
public:
  evHandlerSSD1306() {};
  virtual void begin() override;
  virtual void handle() override;

  void setDsp1(const String & aText);
  void setDsp2(const String & aText);
  void setGraphique(const imageMap_t aLogo);
  void setGraphique();
  void refresh();

private:
  String dsp1;
  String dsp2;
  imageMap_t logo1;
 };
