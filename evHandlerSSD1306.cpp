


// Version evenementielle de l'exemple de la lib SSD13Wire
// Gestion d'un afficheur OLED SSD1306  128 x 64
// 2 zones de text
// Gestion du clignotemente
//
// source : https://github.com/ThingPulse/esp8266-oled-ssd1306
//

#include "evHandlerSSD1306.h"


// For a connection via I2C using the Arduino Wire include:
//#include <Wire.h>         // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"  // legacy: #include "SSD1306.h"

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, SDA, SCL);  // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your


void evHandlerSSD1306::begin() {

  //T_println("evHandlerSSD1306::begin");
  //Wire.begin();
  // Initialising the UI will init the display too.
  display.init();
  //display.flipScreenVertically();
}

void evHandlerSSD1306::handle() {
  /*  if (Events.code != ev10Hz) return;
    //T_println("evHandlerSSD1306::handle");
    display.clear();


    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 54, String(millis()));
    // write the buffer to the display
    display.display();
  */
}

void evHandlerSSD1306::setDsp1(const String& aText) {
  dsp1 = aText;
  refresh();
}
void evHandlerSSD1306::setDsp2(const String& aText) {
  dsp2 = aText;
  refresh();
}

// clear graphique
void evHandlerSSD1306::setGraphique()  {
  logo1 = imageMap_t();
  refresh();
}

void evHandlerSSD1306::setGraphique(const imageMap_t aLogo)  {
  logo1 = aLogo;
  refresh();
}

void evHandlerSSD1306::refresh() {
  display.clear();
  if (dsp1.length()) {
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 0, dsp1);
  }
  if (dsp2.length()) {
    display.setFont(ArialMT_Plain_24);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 18, dsp2);
  }
  if (logo1.width) {
    display.drawXbm(64 - (logo1.width / 2), 0, logo1.width, logo1.height, logo1.data);
  }

  // write the buffer to the display
  display.display();
}
