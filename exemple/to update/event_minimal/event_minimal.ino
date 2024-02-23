/*************************************************
 *************************************************
    Sketch event_minimal.ino   validation of lib betaEvents to deal nicely with events programing with Arduino
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


  History
    V1.0 (21/11/2020)
    V2.0  20/04/2021

 *************************************************/

#define APP_NAME "event_minimal V2.0"


#include "EventsManager32.h"
#include "ESP8266.h"



/* Evenements du Manager (voir BetaEvents.h)
  evNill = 0,      // No event  about 1 every milisecond but do not use them for delay Use pushDelayEvent(delay,event)
  ev100Hz,         // tick 100HZ    non cumulative (see betaEvent.h)
  ev10Hz,          // tick 10HZ     non cumulative (see betaEvent.h)
  ev1Hz,           // un tick 1HZ   cumulative (see betaEvent.h)
  ev24H,           // 24H when timestamp pass over 24H
  evInChar,
  evInString,
*/

// Liste des evenements specifique a ce projet
enum tUserEventCode {
  // evenement recu
  evBP0 = 100,
  evLed0,
};


// instance EventManager
EventManager Events = EventManager();

// instance clavier
evHandlerSerial Keyboard(115200);

// instances poussoir
evHandlerButton BP0(evBP0,BP0_PIN);

// instance LED
evHandlerLed    Led0(evLed0, LED_BUILTIN,false);

void setup() {
  // Start instance
  Events.begin();
  Serial.println(F("\r\n\n" APP_NAME));

  Led0.setFrequence(1, 10);
  Serial.println("Bonjour ....");
}

void loop() {
  // test
  Events.get();
  Events.handle();
  switch (Events.code)
  {

    case evBP0:
      switch (Events.ext) {
        case evxOn:
          Led0.setMillisec(500, 50);
          Serial.println(F("BP0 On"));
          break;
        case evxOff:
          Led0.setMillisec(1000, 10);
          Serial.println(F("BP0 Off"));
          break;
        case evxLongOn:
          Serial.println(F("BP0 Long On"));
          break;
        case evxLongOff:
          Serial.println(F("BP0 Long Off"));
          break;

      }
      break;

  }
}
