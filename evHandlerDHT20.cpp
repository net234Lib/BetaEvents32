#include "EventsManager32.h"
/*************************************************
 *************************************************
    handler evHandlerDHT20.h   validation of lib betaEvents to deal nicely with events programing with Arduino
    Copyright 2020 Pierre HENRY net23@frdev.com All - right reserved.

  This file is part of betaEvents.

    betaEvents is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    betaEvents is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with betaEvents.  If not, see <https://www.gnu.org/licenses/lglp.txt>.

  info in evHandlerDHT20.h
*/
//    FILE: DHT20_async.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: Demo for DHT20 I2C humidity & temperature sensor
//     URL: https://github.com/RobTillaart/DHT20
//
//  Always check datasheet - front view
//
//          +--------------+
//  VDD ----| 1            |
//  SDA ----| 2    DHT20   |
//  GND ----| 3            |
//  SCL ----| 4            |
//          +--------------+


#include "DHT20.h"

DHT20 DHT;


#include  "evHandlerDHT20.h"
void evHandlerDht20::begin() {
  // T_println("evHandlerDht20::begin");
  Wire.begin();

  DHT.begin();  //  ESP32 default 21, 22
  //TODO: check for minimum delay 5s
  Events.repeatedPushMilli(delai, evDth20); // relecture dans le delai imposé  evxDthStart est imposé
  //Events.forceDelayedPushMilli(2000, evDs18x20, evxDthRead, true); // next read in 2 seconds
};

// gestion des evenements
void evHandlerDht20::handle() {
  if (Events.code != evDth20) return;
  if (Events.ext == evxDthStart) {
    if (!DHT.requestData()) {
      Events.forceDelayedPushMilli(1100, evDth20, evxDthRun); // lecture dans 1 secondes
      return;
    }
    error = DHT.readStatus();
    Events.push(evDth20, evxDthError, error);
    return;
  }
  if (Events.ext == evxDthRun) {
    DHT.readData();
    DHT.convert();
    //float aTemperature = DHT.getTemperature();
    //float aHumidity = DHT.getHumidity();
    Events.push(evDth20, evxDthRead); //inform user
    /*
    if ( abs(humidity-aHumidity)>deltaHum ) {
      if (!humidity)  humidity=aHumidity;
      humidity = (humidity  + aHumidity) / 2;
      Events.push(evDth20, evxDthHumidity, humidity*100);
    }
    if ( abs(temperature-aTemperature)>deltaTemp ) {
      if (!temperature) temperature = aTemperature;
      temperature = (temperature + aTemperature) / 2;
      Events.push(evDth20, evxDthTemperature, temperature*100);
    }
    */
  }
  return;
};

float  evHandlerDht20::getTemperature() {
  return (DHT.getTemperature());
}
float  evHandlerDht20::getHumidity() {
  return (DHT.getHumidity());
}
