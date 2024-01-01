/*************************************************
 *************************************************
    handler evHandlers.h   validation of lib betaEvents to deal nicely with events programing with Arduino
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

    V2.0  20/04/2021
    - Mise en liste chainée de modules 'events'
      evHandlerSerial   Gestion des caracteres et des chaines provenant de Serial
      evHandlerLed      Gestion d'une led avec ou sans clignotement sur un GPIO (Multiple instance possible)
      evHandlerButton   Gestion d'un pousoir sur un GPIO (Multiple instance possible)
      evHandlerDebug    Affichage de l'occupation CPU, de la memoire libre et des evenements 100Hz 10Hz et 1Hz

   History

   works with beteEvents 2.0

    V2.0  20/04/2021
    - Mise en liste chainée de modules 'events' test avec un evButton
    V2.0.1  26/10/2021
      corections evHandlerLed sur le true/false
    V2.2  27/10/2021
       more arduino like lib with self built in instance
    V2.2a  11/11/2021
       add begin in evHandles

    V2.3    09/03/2022   isolation of evHandler for compatibility with dual core ESP32

    V2.4    30/09/2022   Isolation des IO (evhandlerOutput)

    *************************************************/
#pragma once
#include <Arduino.h>
#include "EventsManager32.h"



/**********************************************************

   gestion d'un output simple

 ***********************************************************/
typedef enum {
  // evenement etendus pour les Input Output
  evxOff,      // IO Off (les ou Relais)
  evxOn,       // IO on
  evxBlink,    // clignotement actif (LED)
  evxLongOff,  // poussoir relaché longtemp
  evxLongOn,   // pousoir enfoncé longtemps
} tIOEventExt;

class evHandlerOutput : public eventHandler_t {

  public:
    evHandlerOutput(const uint8_t aEventCode, const uint8_t aPinNumber, const bool stateON = HIGH);
    virtual void begin() override;
    virtual void handle() override;
    bool isOn();
    void setOn(const bool status = true);
    void pulse(const uint32_t millisecondes);  // pulse d'allumage simple

 

  private:
    uint8_t pinNumber;
    bool stateON; // = HIGH;
    bool state; // = false;
    
 protected:
    uint8_t evCode;


};

/**********************************************************

   gestion d'une Led sur un port   clignotement en frequence ou en millisecondes

 ***********************************************************/






class evHandlerLed : public evHandlerOutput {
  public:
    evHandlerLed(const uint8_t aEventCode, const uint8_t aPinNumber, const bool stateON = HIGH, const uint8_t frequence = 0);
    //virtual void begin()  override;
    virtual void handle() override;
    void setOn(const bool status = true);
    void setFrequence(const uint8_t frequence, const uint8_t percent = 10);      // frequence de la led
    void setMillisec(const uint16_t millisecondes, const uint8_t percent = 10);  // frequence de la led

  private:
    uint16_t millisecondes;
    uint8_t percent;
};


/**********************************************************

   gestion d'un poussoir sur un port   genere evxOn, evxOff, evxLongOn, evxLongOff

 ***********************************************************/




class evHandlerButton : public eventHandler_t {
  public:
    evHandlerButton(const uint8_t aEventCode, const uint8_t aPinNumber, const uint16_t aLongDelay = 1500);
    virtual void begin() override;
    virtual void handle() override;
    bool isOn() {
      return state;
    };

  protected:
    uint8_t evCode;

  private:
    uint8_t pinNumber;
    uint16_t longDelay;
    bool state = HIGH;
};

#ifndef __AVR_ATtiny85__
/**********************************************************

   gestion de Serial pour generer les   evInChar et  evInString

 ***********************************************************/

class evHandlerSerial : public eventHandler_t {
  public:
    evHandlerSerial(const uint32_t aSerialSpeed = 115200, const uint8_t inputStringSize = 20);
    virtual void begin() override;
    //virtual void handle()  override;
    virtual byte get() override;
    void  setInputString(const String aStr);
    String inputString = "";
    char inputChar = '\0';
  private:
    
    uint32_t serialSpeed;
    uint8_t inputStringSizeMax;
    bool stringComplete = false;
    bool stringErase = false;
    
};





/**********************************************************

   gestion d'un traceur de debugage touche 'T' pour visualiser la charge CPU

 ***********************************************************/



class evHandlerDebug : public eventHandler_t {
  public:
    //evHandlerDebug();
    virtual void handle() override;
    uint8_t trackTime = 0;
  private:
    uint16_t ev100HzMissed = 0;
    uint16_t ev10HzMissed = 0;
};

#endif
