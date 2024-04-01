/*************************************************

  MAIN***********************************************
    Sketch betaEvents32.ino   validation of lib betaEvents to deal nicely with events programing with Arduino
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
    V2.2a  11/11/2021
       add begin in evHandles

    V2.3    09/03/2022   isolation of evHandler for compatibility with dual core ESP32
    V2.4    30/09/2022   Isolation des IO (evhandlerOutput)
    V3.0    Octobre 2023   BetaEvent32
     version specifique pour CPU 32 bit
     V3.0.B3  02/2024   delayedevent  remove ALL  previous evants
    V3.0.C    10/02/2024   renomage des delayedEvents
    V3.0.C1   27/02/2024   depart clone Betaevent32DEV

    *************************************************/


#define APP_NAME "betaEvents32 V3.0.C"


#if defined(ESP8266)
#include <ESP8266WiFi.h>
//#elif defined(ESP32)
//#include <WiFi.h>
#else
#error "ESP8266  uniquement"
#endif

#include <Arduino_JSON.h>

#define DEFAULT_PIN
//par defaut le debug
//#define NO_DEBUG   // this keyword remove all Dxx_print  from code
//#define NO_DEBUGGER // this remove debug instance
#define DEBUG_ON
#include "EventsManager32.h"
//EventManager Events = EventManager();
// instance Serial
evHandlerSerial Keyboard;
evHandlerDebug Debug;

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
  evKeypad,
  evLed0,
  evLed1,
  ev1S = 151,
  ev2S,
  ev3S,
  evR1,
  evR2,
  //evUdp,
  // evenement action
  doReset,
};


#define BP0_PIN D3  // 0  //D3
#define BP1_PIN 14
#define LED1_PIN 16
#define ONEWIRE_PIN D4  //SHARE LED BUILTIN !!!
#define BEEP_PIN D3     // SHARE BP0
// instances poussoir
evHandlerButton BP0(evBP0, BP0_PIN);
//evHandlerButton BP1(evBP1, BP1_PIN);





// instance LED
evHandlerLed Led0(evLed0, LED_BUILTIN, HIGH);
evHandlerLed Led1(evLed1, LED1_PIN, HIGH);

// instance keypad
evHandlerKeypad monKeypad(evKeypad, A0);

//instance OLED
#include "evHandlerSSD1306.h"
evHandlerSSD1306 myOled;




#include "BetaLogo.h"


// instance DHT20
#include "evHandlerDHT20.h"
evHandlerDht20 monDHT20(1000L * 60);
bool dhtInfo = false;

// instance DS18b20
#include "evHandlerDS18b20.h"
evHandlerDS18b20 monDS18b20(1000L * 60);
bool dsInfo = false;



/*
  // init UDP
  #include "evHandlerUdp.h"
  String nodeName = "BetaEvent32";
  const unsigned int localUdpPort = 23423;  // local port to listen on
  evHandlerUdp myUdp(evUdp, localUdpPort, nodeName);
*/
bool sleepOk = true;
int multi = 0;  // nombre de clic rapide
bool sendInfo = false;

void setup() {
  enableWiFiAtBootTime();  // mendatory for autoconnect WiFi with ESP8266 kernel 3.0
  // IO Setup



  //Serial.begin(115200);
  //Serial.println(F("\r\n\n" APP_NAME));
  //Serial.println(F("Test1"));
  // Start instance
  Events.begin();
  Serial.println(F("\r\n\n" APP_NAME));
  Led0.setFrequence(1, 10);
  Led1.setMillisec(2000, 10);

  //  force Wifi en STA
  if (WiFi.getMode() != WIFI_STA) {
    Serial.println(F("!!! Force WiFi to STA mode !!!"));
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.begin();
    //WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
  // WiFi.forceSleepBegin();
  //WiFi.mode(WIFI_OFF);



  Serial.println("Bonjour ....");
  DV_println(sizeof(stdEvent_t));
  displaySizeofItems();
  DV_println(sizeof(size_t));
  DV_println(sizeof(int));
  DV_println(sizeof(long));
  DV_println(sizeof(long long));
  DV_println(sizeof(float));
  DV_println(sizeof(double));
  //Serial.println(sizeof(eventItem_t));
  //DV_println(sizeof(delayEventItem_t));
  //DV_println(sizeof(longDelayEventItem_t));
}

byte BP0Multi = 0;
int16_t consigne = 180;

//int ev1000HzCnt = 0;
//int ev100HzCnt = 0;
//int ev10HzCnt = 0;


void loop() {
  // test
  Events.get(sleepOk);
  Events.handle();
  switch (Events.code) {

    case evInit:

      Serial.println("ev init");
      myOled.setGraphique(BETAlogo);
      Events.delayedPushMilli(4000, evPostInit);
      //      myUdp.broadcastInfo("Boot");

      break;

    case evPostInit:
      myOled.setGraphique();
      myOled.setDsp2(String(consigne / 10.0, 1));
      break;

    case ev1Hz:
      {

        String aStr = niceDisplayTime(now(), false);
        aStr += "  ";
        aStr += String(monDHT20.getTemperature(), 1);
        myOled.setDsp1(aStr);
      }
      break;

    case evKeypad:
      {
        int aKey = monKeypad.getKey();
        String aStr = "";
        if (aKey & 0b00000001) aStr += "Millieu ";
        if (aKey & 0b00000010) aStr += "Droite ";
        if (aKey & 0b00000100) aStr += "Gauche ";
        if (aKey & 0b00001000) aStr += "Haut ";
        if (aKey & 0b00010000) aStr += "Bas ";
        switch (Events.ext) {
          case evxOn:
            TV_println("Keypad on: ", aStr);
            if (aKey == 1) {
              beep(443, 100);
              myOled.setGraphique(BETAlogo);
              Events.delayedPushMilli(4000, evPostInit);
            } else {
              beep(1000, 50);
            }
            if (aKey == 8 and consigne < 240) {
              consigne += 5;
              myOled.setDsp2(String(consigne / 10.0, 1));
            }
            if (aKey == 16 and consigne > 80) {
              consigne -= 5;
              myOled.setDsp2(String(consigne / 10.0, 1));
            }
            break;
          case evxOff:
            TV_println("Keypad relaché: ", aStr);
            break;
          case evxRepeat:
            TV_println("Keypad repeat: ", aStr);
            beep(1000, 10);
            if (aKey == 8 and consigne < 240) {
              consigne += 5;
              myOled.setDsp2(String(consigne / 10.0, 1));
            }
            if (aKey == 16 and consigne > 80) {
              consigne -= 5;
              myOled.setDsp2(String(consigne / 10.0, 1));
            }
            break;
        }
      }
      break;

    case ev24H:
      {
        Serial.println("---- 24H ---");
        int aDay = Events.ext;
        DV_println(aDay);
      }
      break;

    case evDth20:
      {
        switch (Events.ext) {
          case evxDthStart:
            if (dhtInfo) T_println("evxDhtStart");
            break;
          case evxDthRun:
            if (dhtInfo) T_println("evxDhtRun");
            break;
          case evxDthRead:


            V_println(monDHT20.getTemperature());
            V_println(monDHT20.getHumidity());
            break;
          /*
                   case evxDthHumidity:
                     TV_println("Humidity", (float)Events.intExt2 / 100);
                     break;
                   case evxDthTemperature:
                     TV_println("Temperature", (float)Events.intExt2 / 100);
                     break;
          */
          case evxDthError:
            V_println(monDHT20.error);
            break;
        }
      }
      break;

    case evDs18b20:
      {
        switch (Events.ext) {
          case evxDsStart:
            if (dsInfo) T_println("evxDsStart");
            break;
          case evxDsSearch:
            if (dsInfo) T_println("evxDsRun");
            break;
          case evxDsRead:
            if (dsInfo) T_println("evxDsRead");
            TV_print("sonde", monDS18b20.current);
            V_println(monDS18b20.getTemperature());
            break;

          case evxDsError:
            TV_println("evxDsError", monDS18b20.error);
            break;
        }
      }
      break;







    case evBP0:
      switch (Events.ext) {
        case evxOn:
          Led0.setMillisec(500, 50);
          BP0Multi++;
          Serial.println(F("BP0 Down"));
          if (BP0Multi > 1) {
            DV_println(BP0Multi);
          }
          break;
        case evxOff:
          Led0.setMillisec(1000, 10);
          Serial.println(F("BP0 Up"));
          break;
        case evxLongOn:
          if (BP0Multi == 5) {
            Serial.println(F("RESET"));
            Events.push(doReset);
          }

          Serial.println(F("BP0 Long Down"));
          break;
        case evxLongOff:
          BP0Multi = 0;
          Serial.println(F("BP0 Long Up"));
          break;
      }
      break;
    case evBP1:
      switch (Events.ext) {
        case evxOn:
          Led1.setFrequence(3, 50);
          Serial.print(F("BP1 Down "));
          Serial.println(BP0.isOn() ? "and BP0 Down" : "and BP0 Up");
          break;
        case evxOff:
          Led1.setOn(false);
          Serial.println(F("BP1 Up"));
          break;
        case evxLongOn:
          Led1.setFrequence(1, 50);
          Serial.println(F("BP1 Long Down"));
          break;
        case evxLongOff: Serial.println(F("BP1 Long Up")); break;
      }
      break;



    /*
        case evUdp:
          if (Events.ext == evxUdpRxMessage) {
            DTV_println("got an Event UDP", myUdp.rxJson);

            JSONVar rxJson = JSON.parse(myUdp.rxJson);
            String from = rxJson.keys()[0];


            //CMD
            JSONVar rxJson2 = rxJson[from]["CMD"];
            if (JSON.typeof(rxJson2).equals("object")) {
              String dest = rxJson2.keys()[0];
              // Les CMD acceptée doivent etre adressé a ce module
              if (dest.equals("ALL") or (dest.length() > 3 and nodeName.startsWith(dest))) {
                String aCmd = rxJson2[dest];
                aCmd.trim();
                DV_println(aCmd);
                if (aCmd.startsWith(F("NODE=")) and !nodeName.equals(dest)) break;  // NODE= not allowed on aliases
                if (aCmd.length()) Keyboard.setInputString(aCmd);
              } else {
                DTV_println("CMD not for me.", dest);
              }
            }
          }
          break;
    */
    case ev1S:
      Serial.println(F("EV1S"));
      V_println(helperFreeRam());
      V_println(Events.intExt);
      V_println(Events.intExt2);
      break;
    case ev2S:
      V_println(helperFreeRam());
      Serial.println(F("EV2S"));
      V_println(Events.intExt);
      V_println(Events.intExt2);
      break;
    case ev3S:
      V_println(helperFreeRam());
      Serial.println(F("EV3S"));
      V_println(Events.intExt);
      V_println(Events.intExt2);
      break;

    case evR1:
      {
        Events.delayedPushMilli(1000, evR1);
        static long start = millis();
        static long cnt = 0;
        TV_println("evR1 err=", (millis() - start) - cnt++ * 1000);
      }
      break;

    case evR2:
      {
        static long start = millis();
        static long cnt = 0;
        TV_println("evR2   err=", (millis() - start) - cnt++ * 1000);
      }
      break;


    case doReset:
      helperReset();
      break;


    case evInChar:
      switch (toupper(Events.charExt)) {
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
        Serial.print(F("Sleep="));
        Serial.println(sleepOk);
        DV_println(*Events.StringPtr);
      }
      if (Keyboard.inputString.equals(F("B"))) {
        sendInfo = !sendInfo;
        DV_println(sendInfo);
      }

      if (Keyboard.inputString.equals(F("O"))) {
        Serial.println(F("Push 3 delay events"));
        Serial.print(F("Ram="));
        Serial.println(helperFreeRam());
        Events.delayedPushMilli(500, ev1S);
        Events.delayedPushMilli(11 * 1000, ev2S);
        Events.delayedPushMilli(11L * 60 * 1000, ev3S);
        Serial.print(F("Ram="));
        Serial.println(helperFreeRam());
      }
      if (Keyboard.inputString.equals(F("P1"))) {
        Serial.println(F("Push 3 delay events"));
        Serial.print(F("Ram="));
        Serial.println(helperFreeRam());
        Events.push(ev1S, 1011, 1002);
        Events.forceDelayedPushMilli(1000, ev1S, 1111, 1102);
        Events.delayedPushMilli(2000, ev2S, 2222, 2202);
        Events.delayedPushMilli(3000, ev3S, 3333, 3302);
        Serial.print(F("Ram="));
        Serial.println(helperFreeRam());
      }
      if (Keyboard.inputString.equals(F("P2"))) {
        Serial.println(F("Push 3 events"));
        Serial.print(F("Ram="));
        Serial.println(helperFreeRam());
        Events.delayedPushMilli(0, ev1S, 1, 11);
        Events.delayedPushMilli(0, ev2S, 2, 12);
        Events.delayedPushMilli(0, ev3S, 3, 13);
        Serial.print(F("Ram="));
        Serial.println(helperFreeRam());
      }
      /*
        if (Keyboard.inputString.equals(F("BCAST"))) {
        Serial.println(F("bCast 3 messages"));
        myUdp.broadcastInfo(F("message1"));
        myUdp.broadcastInfo(F("message2"));
        myUdp.broadcastInfo(F("message3"));
        }
      */
      if (Keyboard.inputString.equals(F("R0"))) {
        Serial.println(F("remove repeat 1s"));
        Events.removeDelayEvent(evR1);
        Events.removeDelayEvent(evR2);
      }



      if (Keyboard.inputString.equals(F("R1"))) {
        Serial.println(F("repeat 1s classique "));
        Events.delayedPushMilli(1000, evR1);
        Events.repeatedPushMilli(1000, evR2);
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
      if (Keyboard.inputString.equals(F("FREE"))) {
        Serial.print(F("Ram="));
        Serial.println(helperFreeRam());
        String aStr = F("FREE=");
        aStr += String(helperFreeRam());
        //myUdp.broadcastInfo(aStr);
      }

      if (Keyboard.inputString.equals(F("RESET"))) {
        Serial.println(F("RESET"));
        Events.push(doReset);
      }
      if (Keyboard.inputString.equals(F("DHTINFO"))) {
        dhtInfo = !dhtInfo;
        V_println(dhtInfo);
      }
      if (Keyboard.inputString.equals(F("DSINFO"))) {
        dsInfo = !dsInfo;
        V_println(dsInfo);
      }
      if (Keyboard.inputString.equals(F("INFO"))) {
        String aStr = F(" CPU=");
        aStr += String(Events._percentCPU);
        aStr += F(" ack=");
        /*

          aStr += String(myUdp.ackPercent);
                aStr += F("%");
                myUdp.broadcastInfo(aStr);
        */
        DV_print(aStr)
      }


      if (Keyboard.inputString.equals(F("WIFIOFF"))) {
        WiFi.mode(WIFI_OFF);
        //WiFi.forceSleepBegin();
        T_print("WIFI_OFF")
        V_println(WiFi.getMode());
      }


      if (Keyboard.inputString.equals(F("WIFISTA"))) {
        WiFi.mode(WIFI_STA);
        //WiFi.begin();
        T_print("WIFI_STA")
        V_println(WiFi.getMode());
      }


      break;
  }
}


// fatal error
// flash led0 same number as error 10 time then reset
//
void fatalError(const uint8_t error) {
  Serial.print(F("Fatal error "));
  Serial.println(error);


  // display error on LED_BUILTIN
  for (uint8_t N = 1; N <= 5; N++) {
    for (uint8_t N1 = 1; N1 <= error; N1++) {
      delay(150);
      Led0.setOn(true);
      beep(988, 100);
      delay(150);
      Led0.setOn(false);
    }
    delay(500);
  }
  delay(2000);
  helperReset();
}


void beep(const uint16_t frequence, const uint16_t duree) {
  tone(BEEP_PIN, frequence, duree);
  delay(duree);
  pinMode(BEEP_PIN, INPUT_PULLUP);
}
