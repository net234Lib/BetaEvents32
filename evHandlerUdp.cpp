#include <sys/_stdint.h>
#include <cstdio>
#include "evHelpers.h"
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

   works with beteEvents 2.0

    V1.0  07/09/2022
    - gestion de message UDP pour communiquer en node avec des events

     *************************************************/
#if defined(ESP8266) || defined(ESP32)

#include "evHandlerUdp.h"
//#include <WiFiUdp.h>

//WiFiUDP UDP;



//const uint16_t delaySilenceUdp = 300;  // delay de silence avant d'envoyer les trames
const uint16_t delayInterUdp = 200;  // delay entre 2 trames
const uint8_t numberOfTrame = 4;     // nombre de trame repetitives


void udpTxList::add(const String& aJsonStr, const IPAddress aIp) {
  if (++cntTrameUDP == 0) cntTrameUDP++;
  BList::_add(new udpTxTrame(aJsonStr, numberOfTrame, cntTrameUDP, aIp));

};  //ajout d'une trame dans la liste




evHandlerUdp::evHandlerUdp(const uint8_t aEventCode, const uint16_t aPortNumber, String& aNodename)
  : evCode(aEventCode),
    localPortNumber(aPortNumber),
    nodename(aNodename) {
  //rxHeader.reserve(16);
  //rxNode.reserve(16);
  rxJson.reserve(UDP_MAX_SIZE);
  //messageUDP.reserve(UDP_MAX_SIZE);
  //  01:10:47.989 -> 01:10:47,CPU=18%,Loop=884,Nill=778,Ram=46456,Frag=15%,MaxMem=38984 Miss:16/0
  //  01:25:14.369 -> 01:25:13,CPU=22%,Loop=849,Nill=745,Ram=46464,Frag=2%,MaxMem=45608 Miss:12/0
}

const IPAddress broadcastIP(255, 255, 255, 255);

void evHandlerUdp::begin() {
  UDP.begin(localPortNumber);
}

//check for waiting udp
byte evHandlerUdp::get() {
  if (UDP.parsePacket()) {
    //Serial.print('.');
    evManager.ext = evxRxUdp;
    return (evManager.code = evCode);
  }
  return (evNill);
}


void evHandlerUdp::handle() {
  if (evManager.code != evCode) return;

  // broadcst out = send unicast  castCnt  fois
  switch (evManager.ext) {
    case evxBcast:
      {

        if (!txList._first) {
          pendingUDP = false;
          DT_println("stop evxBcast");
          break;
        }
        evManager.delayedPushMilli(delayInterUdp, evCode, evxBcast);
        udpTxTrame* aTrame = txList._first;


        if (aTrame->castCnt > 0) {
          send(aTrame);
          --aTrame->castCnt;
          DV_println(aTrame->castCnt);
          if (aTrame->castCnt == 0) {
            DT_println("remove trame");
            if (txList._remove(aTrame)) delete aTrame;
            ackCnt = 0;
          }
        }


        break;
      }
      break;

    case evxRxUdp:
      {
        // check for reception
        //int packetSize = UDP.parsePacket();
        //DV_println(packetSize);
        //if (packetSize == 0) break;

        DT_println("Received packet UDP");
        //DV_println(packetSize);
        //  Serial.printf("Received packet of size % d from % s: % d\n    (to % s: % d, free heap = % d B)\n",
        //                packetSize,
        //                UDP.remoteIP().toString().c_str(), UDP.remotePort(),
        //                UDP.destinationIP().toString().c_str(), UDP.localPort(),
        //                ESP.getFreeHeap());

        char udpPacketBuffer[UDP_MAX_SIZE + 1];  //buffer to hold incoming packet,
        int size = UDP.read(udpPacketBuffer, UDP_MAX_SIZE);
        //DV_println(size);
        //  // read the packet into packetBufffer
        //  if (packetSize > UDP_MAX_SIZE) {
        //    Serial.printf("UDP too big ");
        //    return;
        //  }

        //TODO: clean this   cleanup line feed
        if (size > 0 && udpPacketBuffer[size - 1] == '\n') size--;
        udpPacketBuffer[size] = 0;

        String aStr = udpPacketBuffer;

        // filtrage des trame repetitive

        String bStr = grabFromStringUntil(aStr, ',');  // should be {"TRAME":xxx,
        String head = grabFromStringUntil(bStr, ':');  // should be {"TRAME"
        byte trNum = bStr.toInt();                     // numero de la trame

        // gestion des ACK
        // TODO faire une pile de reception pour gerer les ack croisés
        //          et memorisé les modules presents
        //'{"ACK":153,"bLed256C":"BetaEvent32"}'
        if (head.equals(F("{\"ACK\"")) and aStr.endsWith("}")) {  //  trame valide
          DTV_println("got a ACK paquet", aStr);
          grabFromStringUntil(aStr, '"');
          String aDest = grabFromStringUntil(aStr, "\":\"");  //recupere le nom du destinataire
          String aFrom = grabFromStringUntil(aStr, "\"}");
          DTV_print("ack from", aFrom);
          DV_print(aDest);
          static String lastDest;
          static byte lastTrNum;
          static byte cnt;
          DV_println(cnt);
          if (lastDest.equals(aDest) and (lastTrNum == trNum)) {
            if (++cnt > 1 and ackPercent > 10) {
              ackPercent -= (ackPercent / 10);
              DV_println(ackPercent);
            }
          } else {
            lastDest = aDest;
            lastTrNum = trNum;
            cnt = 1;
          }

          if (aDest.equals(nodename)) {
            DT_println("ACK for me");
            udpTxTrame* aTrame = txList._first;
            if (aTrame and (trNum == aTrame->numTrameUDP) and (txList._remove(aTrame))) {
              delete aTrame;
              DT_println("first trame removed");
            }
          }
          break;
        }
        if (not(head.equals(F("{\"TRAME\"")) and aStr.endsWith("}}"))) {  //
          DTV_print("Bad paquet", head);
          DV_println(aStr);
          break;
        }
        // UdpId is a mix of remote IP and EVENT number
        rxIPSender = UDP.remoteIP();
        IPAddress aUdpId = rxIPSender;
        aUdpId[0] = trNum;




        // c'est une nouvelle trame
        bcast = (UDP.destinationIP() == broadcastIP);

        rxJson = '{';
        rxJson += aStr;
        // D_print(rxHeader);
        // D_print(rxNode);
        if (bcast and (random(100) < ackPercent)) {
          grabFromStringUntil(aStr, '"');
          aStr = grabFromStringUntil(aStr, '"');
          ack(trNum, aStr);
        }
        //C'est un doublon USP
        if (aUdpId == lastUdpId) {
          T_println("Doublon UDP");
          if (ackPercent < 100) {
            ackPercent++;
            DV_println(ackPercent);
          }
          break;
        }
        //Todo : filtrer les 5 dernier UdpID ?
        lastUdpId = aUdpId;
        DTV_println("valide UDP", rxJson);
        evManager.push(evCode, evxUdpRxMessage);
        break;
      }
      break;
  }
}



void evHandlerUdp::broadcastInfo(const String& aTxt) {
  //{"info":"Boot"}
  String aJsonStr = F("{\"info\":\"");
  aJsonStr += aTxt;
  aJsonStr += "\"}";
  broadcast(aJsonStr);
}

void evHandlerUdp::broadcast(const String& aJsonStr) {

  unicast(broadcastIP, aJsonStr);
}

void evHandlerUdp::unicast(const IPAddress aIPAddress, const String& aJsonStr) {
  DTV_println("memo unicast ", aJsonStr);
  txList.add(aJsonStr, aIPAddress);  // ajoute la trame a la liste
                                     //  messageUDP = aJsonStr;


  //txIPDest = aIPAddress;
  if (!pendingUDP) {
    evManager.push(evCode, evxBcast);
    pendingUDP = true;
  }
}

// {"WiFiLuce":"{\"temperature_radiateur\":21.25}","DOMO02":"{\"ext\":21.25}"}
void evHandlerUdp::send(const udpTxTrame* aTrame) {

  String message;
  message.reserve(200);
  message = F("{\"TRAME\":");
  message += aTrame->numTrameUDP;
  message += ",\"";
  message += nodename;
  message += "\":";
  message += aTrame->jsonStr;
  message += '}';
  DTV_println("send unicast ", message);
  if (!UDP.beginPacket(aTrame->destIp, localPortNumber)) {
    DT_println("Error sending UDP");
    return;
  }
  DTV_println("UDP send", message);
  UDP.write(message.c_str(), message.length());
  UDP.endPacket();
}

void evHandlerUdp::ack(const uint8_t aNum, const String& aNodename) {
  T_println("Send ack ");
  String message;
  message.reserve(60);
  message = F("{\"ACK\":");
  message += aNum;
  message += ",\"";
  message += aNodename;
  message += "\":\"";
  message += nodename;
  message += "\"}";
  //TODO  send to unicast ip if it was an unicast send ?
  if (!UDP.beginPacket(broadcastIP, localPortNumber)) {
    DT_println("Error sending UDP");
    return;
  }
  DTV_println("UDP send", message);
  UDP.write(message.c_str(), message.length());
  UDP.endPacket();
}




#endif
