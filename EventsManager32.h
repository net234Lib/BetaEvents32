#include <sys/_stdint.h>
/*************************************************
    EventsManager.h   validation of lib betaEvents to deal nicely with events programing with Arduino
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
    V1.3 13/01/2021
    - correction pour mieux gerer les pulses dans le cas 0 ou 100 percent
    - ajout setLedOn(true/false)
    V1.3.1 23/01/2021
  	- correction setLedOn pour un resultat immediat
    V1.4   6/3/2021
    - Inclusion TimeLib.h
    - Gestion des event en liste chainée
     V2.0  20/04/2021
    - Mise en liste chainée de modules 'events'
      evHandlerSerial   Gestion des caracteres et des chaines provenant de Serial
      evHandlerLed      Gestion d'une led avec ou sans clignotement sur un GPIO (Multiple instance possible)
      evHandlerButton   Gestion d'un pousoir sur un GPIO (Multiple instance possible)
      evHandlerDebug    Affichage de l'occupation CPU, de la memoire libre et des evenements 100Hz 10Hz et 1Hz

     V2.1  05/06/2021
       split EventManger and BetaEvents
     V2.2  27/10/2021
       more arduino like lib with self built in instance
       TODO : faire une liste de get event separés

    V2.3    09/03/2022   isolation of evHandler for compatibility with dual core ESP32
     V3.0 
 *************************************************/
#pragma once
#include <Arduino.h>


// betaEvent handle a minimal time system to get for seconds() minutes() or hours()

#include <TimeLib.h>  // uncomment this if you prefer to use arduino TimeLib.h  (it will use little more ram and flash)



void displaySizeofItems();

class EventManager;

#ifndef _Time_h
extern byte second();
extern byte minute();
extern byte hour();
#endif


//Basic system events
enum tEventCode {
  evNill = 0,  // No event  about 1 every milisecond but do not use them for delay Use delayedPushEvent(delay,event)
  ev100Hz,     // tick 100HZ    non cumulative (see betaEvent.h)
  ev10Hz,      // tick 10HZ     non cumulative (see betaEvent.h)
  ev1Hz,       // un tick 1HZ   cumulative (see betaEvent.h)
  ev24H,       // 24H when timestamp pass over 24H
  evInit,      // event pushed par MyEvent.begin()
  evInChar,
  evInString,
  //evPB0,
  //evLED0
  //  evWEB = 20,
  //  evUser = 100,
};


// Base structure for event
struct stdEvent_t {
  //  stdEvent_t(const uint8_t code = evNill, const int8_t ext = 0) : code(code), ext(ext) {}
  stdEvent_t()
    : code(evNill), intExt(0){};
  stdEvent_t(const uint8_t code, const int aInt = 0)
    : code(code), intExt(aInt){};
  stdEvent_t(const uint8_t code, const int16_t int1, const int16_t int2)
    : code(code), intExt2(int2), intExt(int1){};
  //stdEvent_t(const uint8_t code = evNill, const float aFloat = 0) : code(code), aFloat(aFloat) {};
  //  stdEvent_t(const uint8_t code = evNill, const uint8_t ext ) : code(code), ext(ext) {};
  //  stdEvent_t(const uint8_t code = evNill, const char aChar) : code(code), aChar(aChar) {};

  stdEvent_t(const stdEvent_t& stdevent) 
    : code(stdevent.code), intExt2(stdevent.intExt2), data(stdevent.data){};

  uint8_t code;     // code of the event
  int16_t intExt2;  // only in Manager32
  union {
    uint8_t ext;  // extCode of the event
    char charExt;
    int intExt;
    String* StringPtr;
    size_t data;
  };
};




// base pour un eventHandler (gestionaire avec un handleEvent);
class eventHandler_t {
public:
  eventHandler_t* next;  // handle suivant
  eventHandler_t();
  virtual void begin(){};   // called with eventManager::begin
  virtual void handle(){};  // called
  virtual byte get() {
    return evNill;
  };
  //    EventManager evManager;
};

#include "evHandlers.h"
//#define evManager Events
struct eventItem_t;
struct delayEventItem_t;
struct longDelayEventItem_t;
class EventManager : public stdEvent_t {
public:

  EventManager()
    : stdEvent_t(evNill) {  // constructeur
#ifdef EventManagerInstance
#error "EventManager already intancied"
#else
#define EventManagerInstance
#endif
  }
  void begin();
  byte get(const bool sleep = true);
  void handle();

  bool removeDelayEvent(const byte codeevent);
  bool push(const stdEvent_t& eventPtr);
  bool push(const uint8_t code, const int16_t param = 0);
  bool push(const uint8_t code, const int16_t param1, const int16_t param2);
  //   bool   pushFloat(const uint8_t code, const float   afloat);
  bool delayedPushMilli(const uint32_t delayMillisec, const uint8_t code);
   bool repeatedPushMilli(const uint32_t delayMillisec, const uint8_t code);
  bool delayedPushMilli(const uint32_t delayMillisec, const uint8_t code, const int16_t param1, const int16_t param2 = 0);
  bool repeatedPushMilli(const uint32_t delayMillisec, const uint8_t code, const int16_t param1, const int16_t param2 , const bool repeat= false);
  //bool forceDelayedPushMilli(const uint32_t delayMillisec, const uint8_t code);
  bool forceDelayedPushMilli(const uint32_t delayMillisec, const uint8_t code, const int16_t param1 = 0, const int16_t param2 = 0);
  //    int    syncroSeconde(const int millisec = 0);
#ifndef _Time_h
  friend byte second();
  friend byte minute();
  friend byte hour();
#endif
  size_t freeRam();
  void reset();
#ifndef _Time_h
  uint32_t timestamp = 0;  //timestamp en seconde  (more than 100 years)
#endif

  void addHandleEvent(eventHandler_t* eventHandlerPtr);
  //    void   addGetEvent(eventHandler_t* eventHandlerPtr);
private:
  byte nextEvent();  // Recherche du prochain event disponible
  void parseDelayList(delayEventItem_t** ItemPtr, const uint16_t aDelay);
  void parseLongDelayList(longDelayEventItem_t** ItemPtr, const uint16_t aDelay);
  void addDelayEvent(delayEventItem_t** ItemPtr, delayEventItem_t* aItem);
  void addLongDelayEvent(longDelayEventItem_t** ItemPtr, longDelayEventItem_t* aItem);
  bool removeDelayEventFromList(const byte codeevent, delayEventItem_t** nextItemPtr);
  bool removeLongDelayEventFromList(const byte codeevent, longDelayEventItem_t** nextItemPtr);

public:
  unsigned long _loopCounter = 0;
  unsigned long _loopParsec = 0;
  unsigned long _evNillParsec = 0;
  byte _percentCPU = 0;
private:
  unsigned long _evNillCounter = 0;
  uint16_t _idleMillisec = 0;  // CPU millisecondes en pause
  eventItem_t* eventList = nullptr;
  delayEventItem_t* eventMillisList = nullptr;       // event < 1 seconde
  delayEventItem_t* eventTenthList = nullptr;        // event < 1 Minute
  longDelayEventItem_t* eventSecondsList = nullptr;  // autres events up
  eventHandler_t* handleEventList = nullptr;
  //eventHandler_t*   getEventList = nullptr;
};



extern EventManager Events;
 
