/*************************************************
 *************************************************
    Sketch betaEvents.ino   validation of lib betaEvents to deal nicely with events programing with Arduino
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
    - Full rebuild from PH_Event V1.3.1 (15/03/2020)
    V1.1 (30/11/2020)
    - Ajout du percentCPU pour une meilleur visualisation de l'usage CPU
    V1.2 02/01/2021
    - Ajout d'une globale EventManagerPtr pour l'acces par d'autre lib et respecter l'implantation C++
    - Amelioration du iddle mode pour l'ESP8266 (WiFi sleep mode)
    V1.4   6/3/2021
    - Inclusion TimeLib.h
    - Gestion des event en liste chainée
    V2.0  20/04/2021
    - Mise en liste chainée de modules 'events'
      evHandlerSerial   Gestion des caracteres et des chaines provenant de Serial
      evHandlerLed      Gestion d'une led avec ou sans clignotement sur un GPIO (Multiple instance possible)
      evHandlerButton   Gestion d'un pousoir sur un GPIO (Multiple instance possible)
      evHandlerDebug    Affichage de l'occupation CPU, de la memoire libre et des evenements 100Hz 10Hz et 1Hz
    V2.0.1  26/10/2021
      corections evHandlerLed sur le true/false
    V2.2  27/10/2021
       more arduino like lib with self built in instance


    *************************************************/

#define APP_NAME "betaEvents V2.0.1"

#if  defined(__AVR__)
zz#include <avr/wdt.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif




#include "EventsManager32.h"




//EventManager MyEvent;   // local instance de eventManager

/* Evenements du Manager (voir betaEvents.h)
  evNill = 0,      // No event  about 1 every milisecond but do not use them for delay Use delayedPushEvent(delay,event)
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
  evBP1,
  evLed0,
  evLed1,
  ev1S,
  ev2S,
  ev3S,
  // evenement action
  doReset,
};

#if  defined(__AVR__)
#define BP0_PIN 5
#define BP1_PIN 6
#define LED1_PIN 4
#elif defined(ESP8266) || defined(ESP32)
#define BP0_PIN D5 // D1
#define BP1_PIN D6 // D2
#define LED1_PIN D0 // GPIO16

#endif

// instances poussoir
evHandlerButton BP0(evBP0, BP0_PIN);
evHandlerButton BP1(evBP1, BP1_PIN);
evHandlerDebug  Debug;

// instance LED
evHandlerLed    Led0(evLed0, LED_BUILTIN, HIGH);
evHandlerLed    Led1(evLed1, LED1_PIN, HIGH);

// instance Serial
evHandlerSerial Keyboard;

bool sleepOk = true;
int  multi = 0; // nombre de clic rapide


void setup() {
  // IO Setup
#if defined(ESP8266)
  WiFi.forceSleepBegin();
  //   WiFi.mode(WIFI_OFF);
#elif defined(ESP32)
  WiFi.mode(WIFI_OFF);
  btStop();
#endif
  //Serial.begin(115200);
 
  // Start instance
  Events.begin();
   Serial.println(F("\r\n\n" APP_NAME));
  Led0.setFrequence(1, 10);
  Led1.setMillisec(2000, 10);
  Serial.println("Bonjour ....");
  DV_println(sizeof(stdEvent_t));
  DV_println(sizeof(size_t));
  DV_println(sizeof(eventItem_t));
  DV_println(sizeof(delayEventItem_t));
  DV_println(sizeof(longDelayEventItem_t));
}

byte BP0Multi = 0;

//int ev1000HzCnt = 0;
//int ev100HzCnt = 0;
//int ev10HzCnt = 0;


void loop() {
  // test
  Events.get(sleepOk);
  Events.handle();
  switch (Events.code)
  {

    case ev24H:
      Serial.println("---- 24H ---");
      break;



    case evBP0:
      switch (Events.ext) {
        case evxBPDown:
          Led0.setMillisec(500, 50);
          BP0Multi++;
          Serial.println(F("BP0 Down"));
          if (BP0Multi > 1) {
            D_println(BP0Multi);
          }
          break;
        case evxBPUp:
          Led0.setMillisec(1000, 10);
          Serial.println(F("BP0 Up"));
          break;
        case evxBPLongDown:
          if (BP0Multi == 5) {
            Serial.println(F("RESET"));
            Events.push(doReset);
          }

          Serial.println(F("BP0 Long Down"));
          break;
        case evxBPLongUp:
          BP0Multi = 0;
          Serial.println(F("BP0 Long Up"));
          break;

      }
      break;
    case evBP1:
      switch (Events.ext) {
        case evxBPDown:
          Led1.setFrequence(3, 50);
          Serial.print(F("BP1 Down "));
          Serial.println(BP0.isDown() ? "and BP0 Down" : "and BP0 Up");
          break;
        case evxBPUp:
          Led1.setOn(false);
          Serial.println(F("BP1 Up"));
          break;
        case evxBPLongDown:
          Led1.setFrequence(1, 50);
          Serial.println(F("BP1 Long Down"));
          break;
        case evxBPLongUp:  Serial.println(F("BP1 Long Up")); break;

      }
      break;



    case ev1S:
      Serial.println(F("EV1S"));
      TV.println("Ram=",Serial.println(helperFreeRam());
      V_println(Events.intExt);
      V_println(Events.intExt2);
      break;
    case ev2S:
      Serial.print(F("Ram=")); Serial.println(helperFreeRam());
      Serial.println(F("EV2S"));
      V_println(Events.intExt);
      V_println(Events.intExt2);
      break;
    case ev3S:
      Serial.print(F("Ram=")); Serial.println(helperFreeRam());
      Serial.println(F("EV3S"));
      V_println(Events.intExt);
      V_println(Events.intExt2);
      break;

    case doReset:
      delay(100);
#ifdef  __AVR__
      wdt_enable(WDTO_120MS);
#else
      ESP. restart();
#endif
      while (1)
      {
        delay(1);
      }
      break;


    case evInChar:
      switch (toupper(Events.aChar))
      {
        case '0': delay(10); break;
        case '1': delay(100); break;
        case '2': delay(200); break;
        case '3': delay(300); break;
        case '4': delay(400); break;
        case '5': delay(500); break;
      }


      break;


    case evInString:

      if (Keyboard.inputString.equals(F("S"))) {
        sleepOk = !sleepOk;
        Serial.print(F("Sleep=")); Serial.println(sleepOk);
        D_println(*Events.aStringPtr);
      }

      if (Keyboard.inputString.equals(F("O"))) {
        Serial.println(F("Push 3 delay events"));
        Serial.print(F("Ram=")); Serial.println(helperFreeRam());
        Events.delayedPushMilli(500, ev1S);
        Events.delayedPushMilli(11 * 1000, ev2S);
        Events.delayedPushMilli(11L * 60  * 1000, ev3S);
        Serial.print(F("Ram=")); Serial.println(helperFreeRam());
      }
      if (Keyboard.inputString.equals(F("P"))) {
        Serial.println(F("Push 3 delay events"));
        Serial.print(F("Ram=")); Serial.println(helperFreeRam());
        Events.delayedPushMilli(1000, ev1S);
        Events.delayedPushMilli(2000, ev2S);
        Events.delayedPushMilli(3000, ev3S);
        Serial.print(F("Ram=")); Serial.println(helperFreeRam());
      }
      if (Keyboard.inputString.equals(F("Q"))) {
        Serial.println(F("Push 3 events"));
        Serial.print(F("Ram=")); Serial.println(helperFreeRam());
        Events.delayedPushMilli(0, ev1S);
        Events.delayedPushMilli(0, ev2S);
        Events.delayedPushMilli(0, ev3S);
        Serial.print(F("Ram=")); Serial.println(helperFreeRam());
      }


      if (Keyboard.inputString.equals(F("FREE"))) {
        Serial.print(F("Ram=")); Serial.println(helperFreeRam());
      }

      if (Keyboard.inputString.equals(F("RESET"))) {
        Serial.println(F("RESET"));
        Events.push(doReset);
      }

      break;

  }
}
