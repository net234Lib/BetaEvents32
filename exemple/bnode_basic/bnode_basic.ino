/*************************************************
 *************************************************
    Sketch bnode_basic.ino   validation of lib betaEvents to deal nicely with events programing with Arduino
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
    V1.0 18/12/2023
    

 *************************************************/

#define APP_NAME "bnode_basic V1.0"


#include <ArduinoOTA.h>


#include "ESP8266.h"
#include "EventsManager32.h"


// littleFS
#include <LittleFS.h>  //Include File System Headers
#define MyLittleFS LittleFS

//WiFI
#ifdef ESP8266
//#include "ESP8266.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#include "ESP32.h"
#include <WiFi.h>
#include <HTTPClient.h>
#else
#error "ESP8266 ou ESP32 uniquement"
#endif

//#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>
//WiFiClientSecure client;

// rtc memory to keep date
//struct __attribute__((packed))
struct {
  // all these values are keep in RTC RAM
  uint8_t crc8;            // CRC for savedRTCmemory
  time_t actualTimestamp;  // time stamp restored on next boot Should be update in the loop() with setActualTimestamp
} savedRTCmemory;

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
  evStartOta,
  evStopOta,
  evUdp,
};


// instance EventManager
EventManager Events = EventManager();

// serial

evHandlerSerial Keyboard;
evHandlerDebug Debug;


// instances poussoir
evHandlerButton BP0(evBP0, BP0_PIN);

// Variable d'application locale
String nodeName = "";  // nom de  la device (a configurer avec NODE=)"

// init UDP
#include "evHandlerUdp.h"

const unsigned int localUdpPort = 23423;  // local port to listen on
evHandlerUdp myUdp(evUdp, localUdpPort, nodeName);




bool WiFiConnected = false;
time_t currentTime;   // timestamp en secondes (local time)
int8_t timeZone = 0;  //-2;  //les heures sont toutes en localtimes (par defaut hivers france)
int deltaTime = 0;    // delta timestamp UTC en heure
bool configErr = false;
bool WWWOk = false;
bool APIOk = false;
int currentMonth = -1;
bool sleepOk = true;
int multi = 0;         // nombre de clic rapide
bool configOk = true;  // global used by getConfig...
const byte postInitDelay = 15;
bool postInit = false;  // true postInitDelay secondes apres le boot (limitation des messages Slack)



// instance LED
evHandlerLed Led0(evLed0, LED_BUILTIN, false);

void setup() {
  enableWiFiAtBootTime();  // mendatory for autoconnect WiFi with ESP8266 kernel 3.0

  // Start instance
  Events.begin();
  Serial.println(F("\r\n\n" APP_NAME));
  Led0.setFrequence(1, 10);

  DV_println(WiFi.getMode());

  //  normaly not needed
  if (WiFi.getMode() != WIFI_STA) {
    Serial.println(F("!!! Force WiFi to STA mode !!!"));
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.begin();
    //WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }

  // System de fichier
  if (!MyLittleFS.begin()) {
    Serial.println(F("erreur MyLittleFS"));
    //fatalError(3);
  }

  // recuperation de l'heure dans la static ram de l'ESP
  if (!getRTCMemory()) {
    savedRTCmemory.actualTimestamp = 0;
  }
  // little trick to leave timeStatus to timeNotSet
  // TODO: see with https://github.com/PaulStoffregen/Time to find a way to say timeNeedsSync
  adjustTime(savedRTCmemory.actualTimestamp);
  currentTime = savedRTCmemory.actualTimestamp;

  // recuperation de la config dans config.json
  nodeName = jobGetConfigStr(F("nodename"));
  if (!nodeName.length()) {
    Serial.println(F("!!! Configurer le nom de la device avec 'NODE=nodename' !!!"));
    configErr = true;
    nodeName = F(APP_NAME);
    nodeName = grabFromStringUntil(nodeName, ' ');
    nodeName += '_';
    nodeName += WiFi.macAddress().substring(12, 14);
    nodeName += WiFi.macAddress().substring(15, 17);
  }
  DV_println(nodeName);


  ArduinoOTA.setHostname(nodeName.c_str());

  // recuperation de la timezone dans la config
  timeZone = jobGetConfigInt(F("timezone"));
  if (!configOk) {
    timeZone = -2;  // par defaut France hivers
    Serial.println(F("!!! timezone !!!"));
  }
  DV_println(timeZone);




  Serial.println("Bonjour ....");
}

void loop() {

  ArduinoOTA.handle();

  Events.get();
  Events.handle();
  switch (Events.code) {

    case ev1Hz:

      // check for connection to local WiFi  1 fois par seconde c'est suffisant
      jobCheckWifi();

      // save current time in RTC memory
      currentTime = now();
      savedRTCmemory.actualTimestamp = currentTime;  // save time in RTC memory
      saveRTCmemory();

      // If we are not connected we warn the user every 30 seconds that we need to update credential
      if (!WiFiConnected && second() % 30 == 15) {
        // every 30 sec
        Serial.print(F("module non connecté au Wifi local "));
        DTV_println("SSID", WiFi.SSID());
        Serial.println(F("taper WIFI= pour configurer le Wifi"));
      }


      /*
      // au chagement de mois a partir 7H25 on envois le mail (un essais par heure)
      if (WiFiConnected && currentMonth != month() && hour() > 7 && minute() == 25 && second() == 0) {
        if (sendHistoTo(mailSendTo)) {
          if (currentMonth > 0) eraseHisto();
          currentMonth = month();
          writeHisto(F("Mail send ok"), mailSendTo);
        } else {
          writeHisto(F("Mail erreur"), mailSendTo);
        }
      }
      */

      break;
    case evInit:
      Serial.println(F("Init"));
      Events.delayedPushMilli(3000, evStartOta);
      break;


    case ev24H:
      {
        Serial.println(F("ev24H"));
      }
      break;


    case evStopOta:
      Serial.println("Stop OTA");
      myUdp.broadcast("{\"info\":\"stop OTA\"}");
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
        Serial.print(nodeName);
        Serial.println(F("' started."));
        Serial.print(F("SSID:"));
        Serial.println(WiFi.SSID());
        myUdp.broadcast("{\"info\":\"start OTA\"}");
        //end start OTA
      }
      break;



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

    case evUdp:
      if (Events.ext == evxUdpRxMessage) {
        DTV_println("got an Event UDP", myUdp.rxJson);
        String aStr = grabFromStringUntil(myUdp.rxJson, F("{\"CMD\":{\""));
        if (myUdp.rxJson.length() == 0) {
          DTV_println("Not a CMD", aStr);
          break;
        }

        aStr = grabFromStringUntil(myUdp.rxJson, '"');
        if (not aStr.equals(nodeName)) {
          DTV_println("CMD not for me", aStr);
          break;
        }
        grabFromStringUntil(myUdp.rxJson, '"');
        aStr = grabFromStringUntil(myUdp.rxJson, '"');
        aStr.trim();
        if (aStr.length()) Keyboard.setInputString(aStr);
      }
      break;

    case evInString:
      //D_println(Keyboard.inputString);
      if (Keyboard.inputString.startsWith(F("?"))) {
        Serial.println(F("Liste des commandes"));
        Serial.println(F("NODE=nodename (nom du module)"));
        Serial.println(F("WIFI=ssid,paswword"));
        //        Serial.println(F("MAILTO=adresse@mail    (mail du destinataire)"));
        //        Serial.println(F("MAILFROM=adresse@mail  (mail emetteur 'NODE' sera remplacé par nodename)"));
        //        Serial.println(F("SMTPSERV=mail.mon.fai,login,password  (SMTP serveur et credential) "));
        //        Serial.println(F("SONDENAMES=name1,name2...."));
        //        Serial.println(F("SWITCHENAMES=name1,name2...."));
        //        Serial.println(F("RAZCONF      (efface la config sauf le WiFi)"));
        //        Serial.println(F("MAIL         (envois un mail de test)"));
        //        Serial.println(F("API          (envois une commande API timezone)"));
        //        Serial.println(F("BCAST        (envoi un broadcast)"));
      }

      if (Keyboard.inputString.startsWith(F("NODE="))) {
        Serial.println(F("SETUP NODENAME : 'NODE= nodename'  ( this will reset)"));
        String aStr = Keyboard.inputString;
        grabFromStringUntil(aStr, '=');
        aStr.replace(" ", "_");
        aStr.trim();

        if (aStr != "") {
          nodeName = aStr;
          DV_println(nodeName);
          jobSetConfigStr(F("nodename"), nodeName);
          delay(1000);
          Events.reset();
        }
      }


      if (Keyboard.inputString.startsWith(F("WIFI="))) {
        Serial.println(F("SETUP WIFI : 'WIFI= WifiName, password"));
        String aStr = Keyboard.inputString;
        grabFromStringUntil(aStr, '=');
        String ssid = grabFromStringUntil(aStr, ',');
        ssid.trim();
        DV_println(ssid);
        if (ssid != "") {
          String pass = aStr;
          pass.trim();
          DV_println(pass);
          bool result = WiFi.begin(ssid, pass);
          //WiFi.setAutoConnect(true);
          DV_println(WiFi.getAutoConnect());
          Serial.print(F("WiFi begin "));
          DV_println(result);
        }
      }
      //      if (Keyboard.inputString.equals(F("RAZCONF"))) {
      //        Serial.println(F("RAZCONF this will reset"));
      //        eraseConfig();
      //        delay(1000);
      //        Events.reset();
      //      }


      if (Keyboard.inputString.equals(F("FREE"))) {
        DV_println(Events.freeRam());
        String aStr = F("{\"info\":\"FREE=");
        aStr += String(Events.freeRam());
        aStr += "\"}";
        myUdp.broadcast(aStr);
      }

      if (Keyboard.inputString.equals("OTA")) {
        Events.push(evStartOta);
        DT_println("Start OTA");
      }

      if (Keyboard.inputString.equals(F("RESET"))) {
        Serial.println(F("RESET"));
        delay(1000);
        Events.reset();
      }


      break;
  }
}
