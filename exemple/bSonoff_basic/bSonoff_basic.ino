/*************************************************
 *************************************************
    Sketch bSonoff_basic.ino   validation of lib betaEvents to deal nicely with events programing with Arduino
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


  sonoff minimal permet d'allumer et d'eteindre le relais
  avec gestion de l'OTA (desactivable) pour autoriser la programation sans connection
  et la gestion des trames UDP compatible bNodes

   History
    V1.0  (22/01/2022) with OTA

    *************************************************/


#define OTA_ON  // desactiver pour retirer l'OTA

// Definition des constantes pour les IO
//#include "ESP8266.h"
static_assert(sizeof(time_t) == 8, "This version works with time_t 64bit  move to ESP8266 kernel 3.0 or more");

#define APP_NAME "bSonoff_basic V1.0"



#ifdef OTA_ON
#include <ArduinoOTA.h>
#endif

//WiFI
#ifdef ESP8266
//#include "ESP8266.h"
#include <ESP8266WiFi.h>
//#include <ESP8266HTTPClient.h>
#else
#error "ESP8266 ESP8285 uniquement"
#endif



#include "EventsManager32.h"



const int LED0_PIN = 13;
const int RELAY0_PIN = 12;
const int BP0_PIN = 0;

EventManager Events = EventManager();

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
  evLed0,
  evRelay0,
  evStartOta,
  evStopOta,

};

// poussoir
evHandlerButton BP0(evBP0, BP0_PIN);
// led clignotante a 1Hz
evHandlerLed Led0(evLed0, LED0_PIN, LOW, 1);
// Commande relay
evHandlerLed Relay0(evRelay0, RELAY0_PIN, LOW, 0);



// serial

evHandlerSerial Keyboard;
evHandlerDebug Debug;

String deviceName = APP_NAME;

void setup() {

  enableWiFiAtBootTime();  // obligatoire pour kernel ESP > 3.0
                           //  normaly not needed
  if (WiFi.getMode() != WIFI_STA) {
    Serial.println(F("!!! Force WiFi to STA mode !!!"));
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    //WiFi.begin();
    //WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
  WiFi.begin();
  
#ifdef OTA_ON
  deviceName = grabFromStringUntil(deviceName, ' ');
  deviceName += '_';
  deviceName += WiFi.macAddress().substring(12, 14);
  deviceName += WiFi.macAddress().substring(15, 17);
  ArduinoOTA.setHostname(deviceName.c_str());
#endif


  // Start instance
  Events.begin();
  Serial.println(F("\r\n\n" APP_NAME));
  Led0.setFrequence(1, 10);

  Serial.println("Bonjour ....");
}

bool sleepOk = true;

void loop() {

#ifdef OTA_ON
  ArduinoOTA.handle();
#endif

  Events.get(sleepOk);
  Events.handle();
  //D_println(Events.code);
  switch (Events.code) {
      //    case evNill:
      //      break;


    case evInit:
      Serial.println(F("Init"));
      Events.delayedPushMilli(3000, evStartOta);
      break;


    case ev24H:
      {
        Serial.println(F("ev24H"));
      }
      break;

#ifdef OTA_ON
    case evStopOta:
      Serial.println("Stop OTA");
      //myUdp.broadcast("{\"info\":\"stop OTA\"}");
      ArduinoOTA.end();
      //writeHisto(F("Stop OTA"), nodeName);
      break;

    case evStartOta:
      {
        // start OTA

        //ArduinoOTA.setHostname(deviceName.c_str());
        ArduinoOTA.begin();
        Events.delayedPushMilli(1000L * 15 * 60, evStopOta);  // stop OTA dans 15 Min

        Serial.print(F("OTA on '"));
        Serial.print(deviceName);
        Serial.println(F("' started."));
        Serial.print(F("SSID:"));
        Serial.println(WiFi.SSID());
        //myUdp.broadcast("{\"info\":\"start OTA\"}");
        //end start OTA
      }
      break;
#endif

    case evBP0:
      switch (Events.ext) {
        case evxOn:
          {
            Led0.setMillisec(500, 50);
            Serial.println(F("BP0 On"));
            bool Status = not Relay0.isOn();
            V_println(Status);
            Relay0.setOn(Status);
          }
          break;
        case evxOff:
          Led0.setMillisec(1000, 10);
          Serial.println(F("BP0 Off"));
          break;
        case evxLongOn:
          Serial.println(F("BP0 Long On"));
          Events.push(evStartOta);
          break;
        case evxLongOff:
          Serial.println(F("BP0 Long Off"));
          break;
      }
      break;

    case evInChar:
      {
        if (Debug.trackTime < 2) {
          char aChar = Keyboard.inputChar;
          if (isPrintable(aChar)) {
            V_println(aChar);
          } else {
            V_println(int(aChar));
          }
        }
        switch (Keyboard.inputChar) {
          case '0': delay(10); break;
          case '1': delay(100); break;
          case '2': delay(200); break;
          case '3': delay(300); break;
          case '4': delay(400); break;
          case '5': delay(500); break;
        }
      }
      break;

    case evInString:
      if (Debug.trackTime < 2) {
        V_println(Keyboard.inputString);
      }



      if (Keyboard.inputString.equals(F("FREE"))) {
        V_println(Events.freeRam());
      }
      if (Keyboard.inputString.equals("S")) {
        sleepOk = !sleepOk;
        V_println(sleepOk);
      }
      break;
  }
}