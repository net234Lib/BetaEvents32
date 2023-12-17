/*************************************************
     Header betaEvents.h   helper yo use a Event system with EventManager
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
    - V2.1 27/10/2020
    V2.2  27/10/2021
       more arduino like lib with self built in instance

    V2.3    09/03/2022   isolation of evHandler for compatibility with dual core ESP32

 *************************************************/
#pragma once


#include "EventsManager32.h"
// Events Manager build an instance called "Events" who care about events
EventManager Events = EventManager();

// instance poussoir si evBP0 existe
#ifdef DEFAULT_PIN 

// definition GPIO sur D5 pour BP0 si celuici n'est pas defini
#ifndef BP0_PIN
#if  defined(ESP8266) || defined(ESP32)
#define BP0_PIN D3 // (flash) 
#endif
#endif
#endif

#ifdef BP0_PIN
evHandlerButton BP0(evBP0, BP0_PIN);

#endif



// instance LED si evLed0 existe
#ifdef DEFAULT_PIN 

//definition GPIO sur LED_BUILTIN pour LED0 si il n'est pas defini par l'utilisateur
#ifndef LED0_PIN
#define LED0_PIN LED_BUILTIN
#endif

#endif

#ifdef  LED0_PIN
// reverted led on AVR UNO and NANO
#if  defined(__AVR__)
  const bool Led0Revert = true;
#else
  const bool Led0Revert = false;
#endif
// led clignotante a 1Hz 
evHandlerLed    Led0(evLed0, LED0_PIN, Led0Revert , 1);

#endif



#ifndef NO_SERIAL
// instance Serial
#ifndef SERIAL_SPEED
#define SERIAL_SPEED 115200
#endif

#define SERIAL_BUFFERSIZE 100

evHandlerSerial Keyboard(SERIAL_SPEED,SERIAL_BUFFERSIZE);

#ifndef NO_DEBUG

// instance debugger
evHandlerDebug  Debug;
#endif
#endif
