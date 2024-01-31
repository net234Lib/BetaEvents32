/*************************************************
 *************************************************
    handler evUdp.h   validation of lib betaEvents to deal nicely with events programing with Arduino
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

   works with betaEvents32 3.0

    V1.0  07/09/2022
    - gestion de message UDP pour communiquer en node avec des events
    V1.1  23/10/2023
    - passage des message udp en 100% Json (suggestion Artur)
    - TODO : ajouter une pile pour permetre d'envoyer plusieurs messages
     V2.0  28/01/2024    version 32bit
       ajout d'une liste chainee pour pour gerer les trames en full asynchrone

    les trame btrame sont des trames courte moins de 100 char (200 maxi) transmise sur le port 23423 a un rhytme de 100 par secondes maximum
    format de la trame :
    {"TRAME":numTrame,"nodeName":{"typeDonee":{.....}}}
    nodeName est le nom du module  il est suposé etre unique dans la ruche
    typeDonnée  qualifie la donnée de base de la trame les nom FULL majuscules sont reservés 
    TIME pour la transmission de l'heure
    SYNC pour la transmission d'evenement de synchronisation ils sont prioritaires
    Temperature : une temperature
    Info : un message texte informatif


     *************************************************/
#pragma once
#include <Arduino.h>
#define DEBUG_ON
#include  "EventsManager32.h"
#include <WiFiUdp.h>
//#include <IPAddress.h>


typedef enum   {
  // evenement recu
  evxUdpRxMessage,           // broadcast recu
  //evxUdpRUCast,            // Unicast recu
  // evenements interne de gestion
  evxBcast,                // send broadcast
  //evxNoPending,            // clear pending UDP
} tUdpEventExt;

const int UDP_MAX_SIZE = 250;  // we handle short messages
#include "bListe.h"
class udpTxTrame : public BItem<udpTxTrame> {
  public: 
  udpTxTrame() {};
};

class udpTxList : public BItem<udpTxTrame> {
  public: 
  udpTxList() {};
};

class evHandlerUdp : public eventHandler_t {
  public:
    evHandlerUdp(const uint8_t aEventCode, const uint16_t aPortNumber, String& aNodename);
    virtual void begin()  override;
    virtual void handle()  override;
    void broadcast(const String& aJsonStr);
    void broadcastInfo(const String& aText);
    void unicast(const IPAddress aIPAddress,const String& aJsonStr);
  private:
    void cast(const IPAddress aIPAddress);

    uint8_t evCode;             //evcode pour dialoguer avec l'application
    uint16_t localPortNumber;   // port pour trame udp   en bNode classique  23423
    WiFiUDP UDP;                // 
    String & nodename;  // pointeur sur l'identifiant du nodename
    String messageUDP;  // message UDP en cours d'emission
    IPAddress txIPDest; // ip de la destination de la trame
    //bool  pendingUDP = false;   // udp less than 500ms
    time_t  lastUDP;
    uint8_t numTrameUDP = 0; // numeroteur de trame UDP
    uint8_t castCnt;      // compteur d'unicast a l'emission
    IPAddress lastUdpId;      // udp ID composé du numero de trame et des 3 dernier octet de l'IP
  public:
    IPAddress rxIPSender; // ip de la source de la trame
    bool bcast;   // true if rx is a bcast
    //String rxHeader;  // header of rxMessage
    //String rxNode;    // nodename of rxMessage
    String rxJson;    // json of rxMessage
};
