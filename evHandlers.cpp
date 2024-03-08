/*************************************************
 *************************************************
    handler evHandlers.cpp   validation of lib betaEvents to deal nicely with events programing with Arduino
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

   works with beteEvents 2.0

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

    *************************************************/
#include "evHandlers.h"


/**********************************************************

   gestion d'un output generique

 ***********************************************************/

evHandlerOutput::evHandlerOutput(const uint8_t aEventCode, const uint8_t aPinNumber, const bool aStateON)
  : pinNumber(aPinNumber), stateON(aStateON), evCode(aEventCode) {};

void evHandlerOutput::begin() {
  pinMode(pinNumber, OUTPUT);
}


void evHandlerOutput::handle() {
  if (Events.code == evCode) {
    switch (Events.ext) {
      case evxOff:
        setOn(false);
        break;

      case evxOn:
        setOn(true);
        //digitalWrite(pinNumber, (percent == 0) xor levelON);
        break;
    }
  }
}

bool evHandlerOutput::isOn() {
  return state;
};

void evHandlerOutput::setOn(const bool status) {
  state = status;
  digitalWrite(pinNumber, (not status) xor stateON);
}

void evHandlerOutput::pulse(const uint32_t aDelay) {  // pulse d'allumage simple
  if (aDelay == 0) {
    Events.delayedPushMilli(0, evCode, evxOff);
    return;
  }
  Events.delayedPushMilli(0, evCode, evxOn);
  Events.delayedPushMilli(aDelay, evCode, evxOff);
}


/**********************************************************

   gestion d'une Led sur un port   clignotement en frequence ou en millisecondes

 ***********************************************************/

evHandlerLed::evHandlerLed(const uint8_t aEventCode, const uint8_t aPinNumber, const bool revert, const uint8_t frequence)
  : evHandlerOutput(aEventCode, aPinNumber, revert) {
  setFrequence(frequence);
};

void evHandlerLed::handle() {
  if (Events.code == evCode) {
    evHandlerOutput::handle();
    switch (Events.ext) {

      case evxBlink:
        Events.push(evCode, (percent > 0) ? evxOn : evxOff);  // si percent d'allumage = 0 on allume pas
        if (percent > 0 && percent < 100) {                      // si percent = 0% ou 100% on ne clignote pas
          Events.delayedPushMilli(millisecondes * percent / 100, evCode, evxOff);
          Events.forceDelayedPushMilli(millisecondes, evCode, evxBlink);
        }
        break;
    }
  }
}

void evHandlerLed::setOn(const bool status) {
  Events.removeDelayEvent(evCode);
  evHandlerOutput::setOn(status);  // make result instant needed  outside event loop
}


void evHandlerLed::setMillisec(const uint16_t aMillisecondes, const uint8_t aPercent) {
  millisecondes = max(aMillisecondes, (uint16_t)2);
  percent = aPercent;
  Events.delayedPushMilli(0, evCode, evxBlink);
}

void evHandlerLed::setFrequence(const uint8_t frequence, const uint8_t percent) {
  if (frequence == 0) {
    setMillisec(0, 0);
    return;
  }
  setMillisec(1000U / frequence, percent);
}


/**********************************************************

   gestion d'un poussoir sur un port   genere evBPDown, evBPUp, evBPLongDown, evBPLongUp

 ***********************************************************/



evHandlerButton::evHandlerButton(const uint8_t aEventCode, const uint8_t aPinNumber, const uint16_t aLongDelay)
  : evCode(aEventCode), pinNumber(aPinNumber),  longDelay(aLongDelay) {};

void evHandlerButton::begin() {
  pinMode(pinNumber, INPUT_PULLUP);
};

void evHandlerButton::handle() {
  if (Events.code == ev10Hz) {
    if (state != (digitalRead(pinNumber) == LOW)) {  // changement d'etat BP0
      state = !state;
      if (state) {
        Events.push(evCode, evxOn);
        Events.delayedPushMilli(longDelay, evCode, evxLongOn);  // arme un event BP long On
      } else {
        Events.push(evCode, evxOff);
        Events.delayedPushMilli(longDelay, evCode, evxLongOff);  // arme un event BP long Off
      }
    }
  }
}

#ifndef __AVR_ATtiny85__
/**********************************************************

   gestion de Serial pour generer les   evInChar et  evInString

 ***********************************************************/

evHandlerSerial::evHandlerSerial(const uint32_t aSerialSpeed, const uint8_t inputStringSize)
  : serialSpeed(aSerialSpeed),
    inputStringSizeMax(inputStringSize) {
  inputString.reserve(inputStringSize);
  //evManager.(this);
}

void evHandlerSerial::begin() {
  Serial.begin(serialSpeed);
}

byte evHandlerSerial::get() {
  if (stringComplete) {
    stringComplete = false;
    stringErase = true;  // la chaine sera effacee au prochain caractere recu
    Events.StringPtr = &inputString;
    return (Events.code = evInString);
  }
  if (Serial.available()) {
    inputChar = Serial.read();
    if (stringErase) {
      inputString = "";
      stringErase = false;
    }
    if (isPrintable(inputChar) && (inputString.length() <= inputStringSizeMax)) {
      inputString += inputChar;
    };
    if (inputChar == '\n' || inputChar == '\r') {
      stringComplete = (inputString.length() > 0);
    }
    Events.charExt = inputChar;
    return (Events.code = evInChar);
  }
  return (evNill);
}


/**********************
 * 
 * forcage d'une inputstring
 */

void  evHandlerSerial::setInputString(const String aStr) {
  inputString = aStr;
  stringComplete = (inputString.length() > 0);
}



/**********************************************************

   gestion d'un traceur de debugage touche 'T' pour visualiser la charge CPU

 ***********************************************************/


void evHandlerDebug::handle() {
  switch (Events.code) {
    case ev1Hz:

      if (trackTime) {
        Serial.print(Digit2_str(hour()));
        Serial.print(':');
        Serial.print(Digit2_str(minute()));
        Serial.print(':');
        Serial.print(Digit2_str(second()));
        Serial.print(F(",CPU="));
        Serial.print(Events._percentCPU);
        Serial.print('%');
        if (trackTime < 2) {

          Serial.print(F(",Loop="));
          Serial.print(Events._loopParsec);
          Serial.print(F(",Nill="));
          Serial.print(Events._evNillParsec);
          Serial.print(F(",Ram="));
          Serial.print(Events.freeRam());
#ifdef ESP8266
          Serial.print(F(",Frag="));
          Serial.print(ESP.getHeapFragmentation());
          Serial.print(F("%,MaxMem="));
          Serial.print(ESP.getMaxFreeBlockSize());
#endif
        }
        if (ev100HzMissed + ev10HzMissed) {
          Serial.print(F(" Miss:"));
          Serial.print(ev100HzMissed);
          Serial.print('/');
          Serial.print(ev10HzMissed);
          ev100HzMissed = 0;
          ev10HzMissed = 0;
        }
        Serial.println();
      }

      break;

    case ev10Hz:

      ev10HzMissed += Events.intExt - 1;
      if (trackTime > 1) {

        if (Events.intExt > 1) {
          //        for (int N = 2; N<currentEvent.param; N++) Serial.print(' ');
          Serial.print('X');
          Serial.print(Events.intExt - 1);
        } else {
          Serial.print('|');
        }
      }
      break;

    case ev100Hz:
      ev100HzMissed += Events.intExt - 1;

      if (trackTime > 2) {

        if (Events.intExt > 1) {
          //      for (int N = 3; N<currentEvent.param; N++) Serial.print(' ');
          Serial.print('x');
          Serial.print(Events.intExt - 1);
        } else {
          Serial.print('_');
        }
      }
      break;
    case evInString:
      if (Events.StringPtr->equals("T")) {
        if (++(trackTime) > 3) {

          trackTime = 0;
        }
        Serial.print("\nTrackTime=");
        Serial.println(trackTime);
      }

      break;
  }
};

#endif
