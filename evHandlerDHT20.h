/*************************************************
 *************************************************
    handler evHandlerDHT20.h   validation of lib betaEvents to deal nicely with events programing with Arduino
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

  V1.0 23/03/2024

   History

   works with beteEvents32 V3.0

    *************************************************/
#pragma once
#include <Arduino.h>
#include "EventsManager32.h"



/**********************************************************

   gestion d'un DHT20 en I2C
   lecture reguliere de la temperature et de l'humidité

   retourne evxValue

 ***********************************************************/
typedef enum { evxDthStart = 0, // initialise la lecture
               evxDthRun,      // getting data
               evxDthRead,    // read done
               //evxDthTemperature,    // new value for temp
               //evxDthHumidity,       // new value for Humidity
               evxDthError
             }  tevxDth;



class evHandlerDht20 : private eventHandler_t  {
  public:
    evHandlerDht20( const uint32_t aDelai = 60000UL ) :
       delai(aDelai) {};
    //    evHandlerDht20( const uint32_t aDelai = 60000UL,const float aDeltaTemp = 0.1,const float aDeltaHum= 1) :
    // deltaTemp(aDeltaTemp), deltaHum(aDeltaHum), delai(aDelai) {};
    virtual void begin()  override;
    virtual void handle()  override;
    float  getTemperature();  //read temp now      use temperature below
    float  getHumidity();     //read humidity now  use humidity below
    uint8_t error;
    //float temperature;
    //float humidity;
    //float deltaTemp;
    //float deltaHum;

  private:
    uint32_t delai;
};
