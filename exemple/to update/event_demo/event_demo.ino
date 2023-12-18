/*************************************************
 *************************************************
    Sketch event_demo.ino   validation of lib betaEvents to deal nicely with events programing with Arduino
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

    V2.2  27/10/2021
       more arduino like lib with self built in instance
    V2.2.1  02/12/2021  test esp32

 *************************************************/

#define APP_NAME "event_demo V2.2"

/* Evenements du Manager (voir EventsManager.h)
  evNill = 0,      // No event  about 1 every milisecond but do not use them for delay Use pushDelayEvent(delay,event)
  ev100Hz,         // tick 100HZ    non cumulative (see betaEvent.h)
  ev10Hz,          // tick 10HZ     non cumulative (see betaEvent.h)
  ev1Hz,           // un tick 1HZ   cumulative (see betaEvent.h)
  ev24H,           // 24H when timestamp pass over 24H
  evInit,
  evInChar,
  evInString,
*/

// Liste des evenements specifique a ce projet
enum tUserEventCode {
  // evenement utilisateurs
  evBP0 = 100,
  evLed0,
  // evenement action
  doReset,
};


//  betaEvent.h est une aide pour construire les elements de base d'une programation evenementiel

//  une instance "Events" avec un poussoir "BP0" une LED "Led0" un clavier "Keyboard"
//  BP0 genere un evenement evBP0 a chaque pression le poussoir connecté sur BP0_PIN
//  Led0 genere un evenement evLed0 a chaque clignotement de la led precablée sur la platine LED_BUILTIN
//  Keyboard genere un evenement evChar a char caractere recu et un evenement evString a chaque ligne recue
//  Debug permet sur reception d'un "T" sur l'entrée Serial d'afficher les infos de charge du CPU

#define BP0_PIN   5                //   Par defaut BP0 est sur D5
#define LED0_PIN  2      //   Par defaut Led0 est sur LED_BUILTIN
//#define SERIAL_SPEED 115200        //   Default at 115200

#include <BetaEvents.h>

int  multi = 0; // nombre de clic rapide

//#if defined(ESP8266)
//#include <ESP8266WiFi.h>
//#elif defined(ESP32)
//#include <WiFi.h>
//#endif



void setup() {

  // Start instance`
  // will setup Serial speed at 115200 by default
  Events.begin();
  Serial.println(F("\r\n\n" APP_NAME));
//  #if defined(ESP8266)
//  WiFi.forceSleepBegin();
//  //   WiFi.mode(WIFI_OFF);
//#elif defined(ESP32)
//  WiFi.mode(WIFI_OFF);
//  btStop();
//#endif
  Serial.println("Bonjour ....");
  //D_println(LED_BUILTIN);

}

byte BP0Multi = 0;  // detection de click rapide sur le poussoir
bool sleepOk = true;

void loop() {

  Events.get(sleepOk);  // generation du prochain evenement
  Events.handle();      // passage de l'evenement au systeme
  switch (Events.code)  // gestion de l'evenement
  {
    case evInit:
      Serial.println("Init");
      break;


    case ev24H:
      Serial.println("---- 24H ---");
      break;



    case evBP0:
      switch (Events.ext) {
        case evxBPDown:                           // push button 0 went down
          Led0.setMillisec(500, 50);              // set led fast blink
          BP0Multi++;                             // count multi push
          Serial.println(F("BP0 Down"));          // warn user on console
          if (BP0Multi > 1) {                     // if more than 1 multi push
            Serial.print(F("BP0 Multi ="));       //   warn user
            Serial.println(BP0Multi);
          }
          break;
          
        case evxBPUp:                             // push button 0 went up
          Led0.setMillisec(1000, 10);             // set led slow blink 
          Serial.println(F("BP0 Up"));            // warn user
          break;
          
        case evxBPLongDown:                       // push button is down for a long time (1.5 sec) 

          // if you click fast 5 times then hold down button a reset will be done
          if (BP0Multi == 5) {                    // if multi is 5 
            Serial.println(F("RESET"));           //   do a reset
            Events.push(doReset);
          }

          Serial.println(F("BP0 Long Down"));     // warn user
          break;
          
        case evxBPLongUp:                         // push button is up for a long time (1.5 sec) 
          BP0Multi = 0;                           // raz multi who count number of fast click
          Serial.println(F("BP0 Long Up"));       // warn user
          break;

      }
      break;

    case doReset:
      helperReset();
      break;


    case evInChar: {
        if (Debug.trackTime < 2) {
          char aChar = Events.charExt;
          if (isPrintable(aChar)) {
            D_println(Keyboard.inputChar);
          } else {
            D_println(int(aChar));
          }
        }
        switch (toupper(Events.charExt))
        {
          case '0': delay(10); break;
          case '1': delay(100); break;
          case '2': delay(200); break;
          case '3': delay(300); break;
          case '4': delay(400); break;
          case '5': delay(500); break;
        }


        break;

      }

    case evInString:
      if (Debug.trackTime < 2) {
        D_println(Keyboard.inputString);
      }
      if (Keyboard.inputString.equals("RESET")) {
        Serial.println(F("RESET"));
        Events.push(doReset);
      }
      if (Keyboard.inputString.equals("S")) {
        //            if (Events.aStringPtr->equals("S")) {

        sleepOk = !sleepOk;
        D_println(sleepOk);
      }

      break;

  }
}
