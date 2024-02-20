#include <sys/_stdint.h>
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
    {"ACK":numTrame,"nodeName":"sendername"}
    nodeName est le nom du module  il est suposé etre unique dans la ruche
    typeDonnée  qualifie la donnée de base de la trame les nom FULL majuscules sont reservés 
    TIME pour la transmission de l'heure
    SYNC pour la transmission d'evenement de synchronisation ils sont prioritaires
    Temperature : une temperature
    event :  un evxxxxxx
    Info : un message texte informatif


     *************************************************/
#pragma once
#include <Arduino.h>
#define NO_DEBUG
#include "EventsManager32.h"
#include <WiFiUdp.h>
//#include <IPAddress.h>


typedef enum {
  // evenement recu
  evxUdpRxMessage,  // broadcast recu
  evxRxUdp,         // udp Rx recu
  // evenements interne de gestion
  evxBcast,  // send broadcast
  //evxNoPending,            // clear pending UDP
} tUdpEventExt;

const int UDP_MAX_SIZE = 400;  // we handle short messages
#include "bListe.h"

//la txliste et ses items txTrame
class udpTxTrame : public BItem<udpTxTrame> {
public:
  udpTxTrame()
    : castCnt(0){};
  udpTxTrame(const String& aJsonStr, const uint8_t castCnt, const uint8_t numTrameUDP, const IPAddress destIp)
    : destIp(destIp), castCnt(castCnt), numTrameUDP(numTrameUDP), jsonStr(aJsonStr){};
  IPAddress destIp;     // ip de la destination de la trame
  uint8_t castCnt;      // compteur d'unicast a l'emission
  uint8_t numTrameUDP;  // numero de cette trame
  String jsonStr;
};

class udpTxList : public BList<udpTxTrame> {
public:
  udpTxList(){};
  void add(const String& aJsonStr, const IPAddress aIp);  //ajout d'une trame dans la liste
  uint8_t cntTrameUDP = 0;                                // compteur de trame UDP (de 1 a 199)
};


//la udpnodeList  et ses item udpnode
//liste des nodes ayant envoyé des trames : les membres du mesh
class udpNode : public BItem<udpNode> {
public:
  udpNode(){};
};

class udpNodeList : BList<udpNode> {
public:
  udpNodeList(){};
};


class evHandlerUdp : public eventHandler_t {
public:
  evHandlerUdp(const uint8_t aEventCode, const uint16_t aPortNumber, String& aNodename);
  virtual void begin() override;
  virtual byte get() override;
  virtual void handle() override;
  // at this level 'cast' job just push trame in txList   trame will be send on next evXBcast
  void broadcast(const String& aJsonStr);                            // std broadcst
  void broadcastInfo(const String& aText);                           // broadcast just a text
  void broadcastEvent(const String&  aevent);                           // broadcast just a text
  void unicast(const IPAddress aIPAddress, const String& aJsonStr);  // cast to a specific adress
  uint16_t ackPercent = 100;                                         // pourcentage de reponce ACK a faire

private:

  void send(const udpTxTrame* aTrame);
  void ack(const uint8_t aNum, const String& aNodename);
  uint8_t evCode;            //evcode pour dialoguer avec l'application
  uint16_t localPortNumber;  // port pour trame udp   en bNode classique  23423
  WiFiUDP UDP;               // gestionaire UDP
  String& nodename;          // pointeur sur l'identifiant du nodename
  udpTxList txList;          // Liste des message en attente
  udpNodeList nodeList;      // liste de mes voisins de reseau
  bool pendingUDP = false;   // indique qu'un event evCode,evxBcast est en attente

  uint16_t ackCnt;  // nombre de ack recu pour ma trame en cour

  //String messageUDP;  // message UDP en cours d'emission

  //bool  pendingUDP = false;   // udp less than 500ms
  time_t lastUDP;


  IPAddress lastUdpId;  // udp ID composé du numero de trame et des 3 dernier octet de l'IP
public:
  IPAddress rxIPSender;  // ip de la source de la trame
  bool bcast;            // true if rx is a bcast
  //String rxHeader;  // header of rxMessage
  String rxFrom;    // nodename of rxMessage
  String rxJson;  // json of rxMessage
};
