


// Version evenementielle de l'exemple de la lib standard OneDrive
// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// https://github.com/milesburton/Arduino-Temperature-Control-Library
#pragma once
#include <Arduino.h>



#include <OneWire.h>

//#define MAXDS18x20 20  // nombre maxi de sondes pas de vrai limite mais attention a la numerotation des evenements
// chaque sonde a code code event evDS18x20 + le N° de la sonde (de 1 a MAXDS18x20)

typedef enum { evxDsStart,
               evxDsSearch,
               evxDsRead,
               evxDsError } tevxDs;



class evHandlerDS18b20 : private eventHandler_t, OneWire {
public:
  evHandlerDS18b20(const uint32_t aDelai, const uint8_t aPinNumber = D4)
    : OneWire(aPinNumber), pinNumber(aPinNumber), delai(aDelai) {};
  virtual void begin() override;
  virtual void handle() override;
  float getTemperature() {
    return (float)raw / 16.0;
    ;
  }
  uint8_t getNumberOfDevices();
  uint8_t current;
  uint8_t error;

private:

  
  uint8_t pinNumber;
  uint32_t delai;
  uint8_t addr[8];
  uint8_t type_s;
  int16_t raw;  // value
};
