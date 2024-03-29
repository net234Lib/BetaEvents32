#include "EventsManager32.h"


// Version evenementielle de l'exemple de la lib standard OneDrive
// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// https://github.com/milesburton/Arduino-Temperature-Control-Library


#include "evHandlerDS18b20.h"
#define MAXDS18b20 99


//evHandlerDS18x20::evHandlerDS18x20(const uint8_t aPinNumber, const uint16_t aDelai) :
//  delai(aDelai),
//  OneWire(aPinNumber)
//{};

void evHandlerDS18b20::begin() {
  //Events.delayedPushMilli(delai, evDs18b20, evxDsStart); // relecture dans le delai imposé
  //
  //Events.forceDelayedPushMilli(250, evDs18b20, evxDsSearch, true); // next read in 250ms
  //getNumberOfDevices();
  
  //reset();
  //delay(300);
  Events.repeatedPushMilli(delai, evDs18b20); // relecture dans le delai imposé  evxDsStart est imposé
  //reset_search();
};

// gestion des evenements
void evHandlerDS18b20::handle() {
  if (Events.code != evDs18b20) return;
  if (Events.ext == evxDsStart) {
    reset();
    reset_search();
    Events.forceDelayedPushMilli(500, evDs18b20, evxDsSearch);
  //    delay(250);
  current = 0;
    return;
  }
  if (Events.ext == evxDsSearch) {
    //DTV_println("evxDsSearch",current);
    if ( !search(addr)) {
      //Serial.println("No more addresses.");
      if (current == 0) {
        error = 1; // aucune sondes
        Events.push( evDs18b20, evxDsError);
      }
      //derniere sonde lue
      if (pinNumber == D4) pinMode(pinNumber,OUTPUT);  // give back output mode
      return;
    }
    //   Serial.print("ROM =");
    //   for ( uint8_t i = 0; i < 8; i++) {
    //     Serial.write(' ');
    //      Serial.print(addr[i], HEX);
    //    }
    if (OneWire::crc8(addr, 7) != addr[7]) {
      //  Serial.println("CRC is not valid!");
      error = 2;  // crc error
      Events.push( evDs18b20, evxDsError);
      return;
    }
    current++;
    // the first ROM byte indicates which chip
    switch (addr[0]) {
      case 0x10:
        // Serial.println("  Chip = DS18S20");  // or old DS1820
        type_s = 1;
        break;
      case 0x28:
        //Serial.println("  Chip = DS18B20");
        type_s = 0;
        break;
      case 0x22:
        //Serial.println("  Chip = DS1822");
        type_s = 0;
        break;
      default:
        //Serial.println("Device is not a DS18x20 family device.");
        error = 3; // bad device
        Events.push( evDs18b20, evxDsError);
        return;
    }
    error = 0;
    reset();
    select(addr);
    write(0x44, 1);        // start conversion, with parasite power on at the end
    //delay(1000);
    Events.forceDelayedPushMilli(800, evDs18b20, evxDsRead, true); // get converted value in 1000ms ( > 750ms)
    return;
  }
  if (Events.ext == evxDsRead) {
    //uint8_t present = 
    reset();
    select(addr);
    write(0xBE);         // Read Scratchpad
    byte data[9];
    //    Serial.print("  Data = ");
    //    Serial.print(present, HEX);
    //    Serial.print(" ");
    for ( uint8_t i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = read();
      //      Serial.print(data[i], HEX);
      //      Serial.print(" ");
    }
    //    Serial.print(" CRC=");
    //    Serial.print(OneWire::crc8(data, 8), HEX);
    //    Serial.println();
    error = (OneWire::crc8(data, 8) == data[8]) ? 0 : 2; // erreur crc non bloquante
    if (error) Events.push( evDs18b20, evxDsError);

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    raw = (data[1] << 8) | data[0];
    if (type_s) {
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } else {
      byte cfg = (data[4] & 0x60);
      // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
      //// default is 12 bit resolution, 750 ms conversion time
    }

    //if (current < MAXDS18b20) Events.push(evDs18b20+current, 100L * raw / 16);

    Events.forceDelayedPushMilli(100, evDs18b20, evxDsSearch, true); // recherche de la sonde suivante
    return;
  }
  return;
}

uint8_t evHandlerDS18b20::getNumberOfDevices() {
  uint8_t numberOfDevices = 0;
  reset_search();
  while (search(addr)) {
    numberOfDevices++;
  }
  return numberOfDevices;
}
