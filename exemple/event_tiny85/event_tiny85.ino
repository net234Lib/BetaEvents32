/*************************************************
 *************************************************
    Sketch event_tiny85.ino   validation of lib betaEvents to deal nicely with events programing with Arduino
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
// ATMEL ATTINY45 / ARDUINO
//
//                  +-\/-+
// Ain0 (D 5) PB5  1|    |8  Vcc
// Ain3 (D 3) PB3  2|    |7  PB2 (D 2)  Ain1
// Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1
//            GND  4|    |5  PB0 (D 0) pwm0
//                  +----+
#define APP_NAME "event_tiny85 V2.0"

#include "EventsManager.h"
//#include "evHandlers.h"


EventManager MyEvent;   // local instance de eventManager (kernel of BetaEvents)

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

#if  defined(__AVR__)
#define BP0 2  // D2    
#elif defined(ESP8266) || defined(ESP32)
#define BP0 D1 // D1
#endif

// instances poussoir
evHandlerButton MyBP0(evBP0, BP0);
//evHandlerButton MyBP1(evBP1, BP1);

// instance LED
evHandlerLed    MyLed0(evLed0, PB1,true,1);

#if defined (__AVR_ATtiny85__) 
#include <SoftwareSerial.h>
#define RX    PB3   // *** D3, Pin 2
#define TX    PB4   // *** D4, Pin 3

SoftwareSerial Serial(RX, TX);
#endif



void setup() {
  Serial.begin(9600);
  Serial.println(F("\r\n\n" APP_NAME));
  // Start instance
  MyEvent.begin();
  MyLed0.setFrequence(1, 10);
  Serial.println(F("Bonjour ...."));
}

void loop() {
  // test
  MyEvent.getEvent();
  MyEvent.handleEvent();
  switch (MyEvent.currentEvent.code)
  {

    case evBP0:
      switch (MyEvent.currentEvent.ext) {
        case evxBPDown:
          MyLed0.setMillisec(500, 50);
          Serial.println(F("BP0 Down"));
          break;
        case evxBPUp:
          MyLed0.setMillisec(1000, 10);
          Serial.println(F("BP0 Up"));
          break;
        case evxBPLongDown:
          Serial.println(F("BP0 Long Down"));
          break;
        case evxBPLongUp:
          Serial.println(F("BP0 Long Up"));
          break;

      }
      break;

  }
}
