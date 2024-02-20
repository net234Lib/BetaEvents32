/*************************************************
 *************************************************
    jobs for bnode_minimal.ino   check and report by may a box and the wifi connectivity
    Copyright 2021 Pierre HENRY net23@frdev.com All - right reserved.

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
    V1.0 (21/10/2021)
    - From betaevent.h + part of betaport V1.2
      to check my box after a problem with sfr :)

  e croquis utilise 334332 octets (32%) de l'espace de stockage de programmes. Le maximum est de 1044464 octets.
  Les variables globales utilisent 28936 octets (35%) de mémoire dynamique, ce qui laisse 52984 octets pour les variables locales. Le maximum est de 81920 octets.

  e croquis utilise 334316 octets (32%) de l'espace de stockage de programmes. Le maximum est de 1044464 octets.
  Les variables globales utilisent 28964 octets (35%) de mémoire dynamique, ce qui laisse 52956 octets pour les variables locales. Le maximum est de 81920 octets.

 *************************************************/

#define HISTO_FNAME F("/histo.json")
#define CONFIG_FNAME F("/config.json")


void writeHisto(const String& aAction, const String& aInfo) {
  //MyLittleFS.remove(HISTO_FNAME);  // raz le fichier temp
  JSONVar jsonData;
  jsonData["timestamp"] = (double)currentTime;
  jsonData["action"] = aAction;
  jsonData["info"] = aInfo;
  String jsonHisto = JSON.stringify(jsonData);
  DV_println(jsonHisto);
  File aFile = MyLittleFS.open(HISTO_FNAME, "a+");
  if (!aFile) return;
  aFile.println(jsonHisto);
  aFile.close();
}



/*
String niceDisplayTime(const time_t time, bool full) {

  String txt;
  // we supose that time < NOT_A_DATE_YEAR is not a date
  if ( year(time) < NOT_A_DATE_YEAR ) {
    txt = "          ";
    txt += time / (24 * 3600);
    txt += ' ';
    txt = txt.substring(txt.length() - 10);
  } else {

    txt = Digit2_str(day(time));
    txt += '/';
    txt += Digit2_str(month(time));
    txt += '/';
    txt += year(time);
  }

  static String date;
  if (!full && txt == date) {
    txt = "";
  } else {
    date = txt;
    txt += " ";
  }
  txt += Digit2_str(hour(time));
  txt += ':';
  txt += Digit2_str(minute(time));
  txt += ':';
  txt += Digit2_str(second(time));
  return txt;
}



//get a value of a config key
String jobGetConfigStr(const String aKey) {
  String result = "";
  File aFile = MyLittleFS.open(CONFIG_FNAME, "r");
  if (!aFile) return (result);
  aFile.setTimeout(5);
  JSONVar jsonConfig = JSON.parse(aFile.readStringUntil('\n'));
  //D_println(jsonConfig);
  aFile.close();
  configOk = JSON.typeof(jsonConfig[aKey]) == F("string");
  if ( configOk ) result = (const char*)jsonConfig[aKey];
  return (result);
}

int jobGetConfigInt(const String aKey) {
  int result = 0;
  File aFile = MyLittleFS.open(CONFIG_FNAME, "r");
  if (!aFile) return (result);
  aFile.setTimeout(5);
  JSONVar jsonConfig = JSON.parse(aFile.readStringUntil('\n'));
  aFile.close();
  //D_println(JSON.typeof(jsonConfig[aKey]));
  configOk = JSON.typeof(jsonConfig[aKey]) == F("number");
  if (configOk ) result = jsonConfig[aKey];
  return (result);
}



// set a value of a config key
//todo : check if config is realy write ?
bool jobSetConfigStr(const String aKey, const String aValue) {
  // read current config
  JSONVar jsonConfig;  // empry config
  File aFile = MyLittleFS.open(CONFIG_FNAME, "r");
  if (aFile) {
    aFile.setTimeout(5);
    jsonConfig = JSON.parse(aFile.readStringUntil('\n'));
    aFile.close();
  }
  if (aValue != "") {
    jsonConfig[aKey] = aValue;
  } else {
    jsonConfig[aKey] = undefined;
  }
  aFile = MyLittleFS.open(CONFIG_FNAME, "w");
  if (!aFile) return (false);
  D_println(JSON.stringify(jsonConfig));
  aFile.println(JSON.stringify(jsonConfig));
  aFile.close();
  return (true);
}

bool jobSetConfigInt(const String aKey, const int aValue) {
  // read current config
  JSONVar jsonConfig;  // empry config
  File aFile = MyLittleFS.open(CONFIG_FNAME, "r");
  if (aFile) {
    aFile.setTimeout(5);
    jsonConfig = JSON.parse(aFile.readStringUntil('\n'));
    aFile.close();
  }
  jsonConfig[aKey] = aValue;
  aFile = MyLittleFS.open(CONFIG_FNAME, "w");
  if (!aFile) return (false);
  D_println(JSON.stringify(jsonConfig));
  aFile.println(JSON.stringify(jsonConfig));
  aFile.close();
  return (true);
}


bool jobShowConfig() {
  // read current config
  Serial.println(F("--- CONFIG START---"));
  File aFile = MyLittleFS.open(CONFIG_FNAME, "r");
  if (aFile) {
    aFile.setTimeout(5);
    Serial.println(aFile.readStringUntil('\n'));
    aFile.close();
  }
  Serial.println(F("--- CONFIG END---"));
  return (aFile);
}




void printHisto() {
  File aFile = MyLittleFS.open(HISTO_FNAME, "r");
  if (!aFile) return;
  aFile.setTimeout(5);

  bool showTime = true;
  while (aFile.available()) {
    String aLine = aFile.readStringUntil('\n');
    //D_println(aLine);  //'{"teimestamp":xxxxxx"action":"badge ok","info":"0E3F0FFA"}
    JSONVar jsonLine = JSON.parse(aLine);
    time_t aTime = (const double)jsonLine["timestamp"];
    String aAction = (const char*)jsonLine["action"];
    String aInfo = (const char*)jsonLine["info"];
    Serial.print(niceDisplayTime(aTime, showTime));
    showTime = false;
    Serial.print('\t');
    Serial.print(aAction);
    Serial.print('\t');
    Serial.println(aInfo);
  }

  aFile.close();
}

// send histo by mail

// Just standard SMTP mail
//https://fr.wikipedia.org/wiki/Simple_Mail_Transfer_Protocol
//telnet smtp.xxxx.xxxx 25
//Connected to smtp.xxxx.xxxx.
//220 smtp.xxxx.xxxx SMTP Ready
//HELO client
//250-smtp.xxxx.xxxx
//250-PIPELINING
//250 8BITMIME
//MAIL FROM: <auteur@yyyy.yyyy>
//250 Sender ok
//RCPT TO: <destinataire@xxxx.xxxx>
//250 Recipient ok.
//DATA
//354 Enter mail, end with "." on a line by itself
//Subject: Test
//
//Corps du texte
//.
//250 Ok
//QUIT
//221 Closing connection
//Connection closed by foreign host.

bool sendHistoTo(const String sendto)  {

  Serial.print(F("Send histo to "));
  Serial.println(sendto);

  String smtpServer = jobGetConfigStr(F("smtpserver"));
  String sendFrom = jobGetConfigStr(F("mailfrom"));
  if (smtpServer == "" || sendto == "" || sendFrom == "") {
    Serial.print(F("no mail config"));
    return (false);
  }
  String smtpLogin = jobGetConfigStr(F("smtplogin"));
  String smtpPass = jobGetConfigStr(F("smtppass"));



  WiFiClient tcp;  //Declare an object of Wificlass Client to make a TCP connection
  String aLine;    // to get answer of SMTP
  // Try to find a valid smtp
  if ( !tcp.connect(smtpServer, 25) ) {
    Serial.print(F("unable to connect with "));
    Serial.print(smtpServer);
    Serial.println(F(":25"));
    return false;
  }


  bool mailOk = false;
  do {
    aLine = tcp.readStringUntil('\n');
    if ( aLine.toInt() != 220 )  break;  //  not good answer
    Serial.println(F("HELO checkmybox"));
    //tcp.print(F("HELO checkmybox \r\n")); // EHLO send too much line
    tcp.println(F("HELO checkmybox")); // EHLO send too much line
    aLine = tcp.readStringUntil('\n');
    if ( aLine.toInt() != 250 )  break;  //  not good answer
    // autentification
    if (smtpLogin != "") {
      Serial.println(F("AUTH LOGIN"));
      tcp.println(F("AUTH LOGIN"));
      aLine = tcp.readStringUntil('\n');
      if (aLine.toInt() != 334 )  break;  //  not good answer

      //Serial.println(smtpLogin);
      tcp.println(smtpLogin);
      //tcp.print(F("\r\n"));
      aLine = tcp.readStringUntil('\n');
      if (aLine.toInt() != 334 )  break;  //  not good answer

      //Serial.println(smtpPass);
      tcp.println(smtpPass);
      //tcp.print(F("\r\n"));
      aLine = tcp.readStringUntil('\n');
      if (aLine.toInt() != 235 )  break;  //  not good answer
    }

    String aString = F("MAIL FROM: ");
    aString += sendFrom;
    aString.replace(F("NODE"), nodeName);
    Serial.println(aString);
    tcp.println(aString);
    //tcp.print(F("\r\n"));
    aLine = tcp.readStringUntil('\n');
    if ( aLine.toInt() != 250 )  break;  //  not good answer

    Serial.println("RCPT TO: " + sendto);
    tcp.println("RCPT TO: " + sendto);
    aLine = tcp.readStringUntil('\n');
    if ( aLine.toInt() != 250 )  break;  //  not good answer

    Serial.println(F("DATA"));
    tcp.print(F("DATA\r\n"));
    aLine = tcp.readStringUntil('\n');
    if ( aLine.toInt() != 354 )  break;  //  not goog answer

    //Serial.println( "Mail itself" );
    tcp.print(F("Subject: mail from '" ));
    tcp.print(nodeName);
    tcp.print(F("' " APP_NAME "\r\n"));

    tcp.print(F("\r\n"));  // end of header
    // body
    //    tcp.print("ceci est un mail de test\r\n");
    //    tcp.print("destine a valider la connection\r\n");
    //    tcp.print("au serveur SMTP\r\n");
    //    tcp.print("\r\n");
    tcp.print(F(" == == = histo.json == \r\n"));
    File aFile = MyLittleFS.open(HISTO_FNAME, "r");
    if (!aFile) {
      tcp.print(F(" pas de fichier  \r\n"));
    } else {
      aFile.setTimeout(5);
      bool showTime = true;
      while (aFile.available()) {
        String aLine = aFile.readStringUntil('\n');
        //D_println(aLine);  //'{"teimestamp":xxxxxx"action":"badge ok","info":"0E3F0FFA"}
        JSONVar jsonLine = JSON.parse(aLine);
        time_t aTime = (double)jsonLine["timestamp"];
        String aAction = (const char*)jsonLine["action"];
        String aInfo = (const char*)jsonLine["info"];
        tcp.print(niceDisplayTime(aTime, showTime));
        showTime = false;
        tcp.print('\t');
        tcp.print(aAction);
        tcp.print('\t');
        tcp.print(aInfo);
        tcp.print("\r\n");
      }

      aFile.close();
    }
    tcp.print(F(" == Eof histo == \r\n"));

    // end of body
    tcp.print("\r\n.\r\n");
    aLine = tcp.readStringUntil('\n');
    if ( aLine.toInt() != 250 )  break;  //  not goog answer

    mailOk = true;
    break;
  } while (false);
  D_println(mailOk);
  D_println(aLine);
  Serial.println( "quit" );
  tcp.print("QUIT\r\n");
  aLine = tcp.readStringUntil('\n');
  D_println(aLine);

  Serial.println(F("Stop TCP connection") );
  tcp.stop();
  return mailOk;
}

void eraseHisto() {
  Serial.println(F("Erase  histo") );
  MyLittleFS.remove(HISTO_FNAME);
}

void eraseConfig() {
  Serial.println(F("Erase config") );
  MyLittleFS.remove(CONFIG_FNAME);
}

void jobGetSondeName() {
  String aStr = jobGetConfigStr(F("sondename"));
  aStr.replace("#", "");
  for (int N = 0; N < sondesNumber; N++) {
    String bStr = grabFromStringUntil(aStr, ',');
    bStr.trim();
    if (bStr.length() == 0) {
      bStr = F("temperature#");
      bStr += String(N+1);
    }
    sondesName[N] = bStr;
  }
}

void jobGetSwitcheName() {
  String aStr = jobGetConfigStr(F("switchename"));
  aStr.replace("#", "");
  for (int N = 0; N < switchesNumber; N++) {
    String bStr = grabFromStringUntil(aStr, ',');
    bStr.trim();
    if (bStr.length() == 0) {
      bStr = F("switch#");
      bStr += String(N+1);
    }
    switchesName[N] = bStr;
  }
}


void jobBcastSwitch(const String& aName, const int aValue) {
  String aTxt = "{\"switch\":{\"";
        aTxt += aName;
        aTxt += "\":";
        aTxt += aValue;
        aTxt += "}}";
        TD_println("BroadCast", aTxt);
        myUdp.broadcast(aTxt);
}


// 100 HZ
void jobRefreshLeds(const uint8_t delta) {
  ESP.wdtFeed();   // this is critic sinon on a un plantage Hard Watchdow aleatoire si le wifi est instable
  ledFixe1.write();
  ledFixe2.write();
  ledFixe3.write();
  for (int8_t N = 0; N < ledsMAX; N++) {
    leds[N].write();
  }
//  for (int8_t N = ledsMAX-1; N > 0; N--) {
//   leds[N].write();
//   }
  
  leds[0].reset();  // obligatoire
  //ESP.wdtFeed();  // this is critic sinon on a un plantage Hard Watchdow aleatoire si le wifi est instable
  ledFixe1.anime(delta);
  ledFixe2.anime(delta);
  ledFixe3.anime(delta);
  for (uint8_t N = 0; N < ledsMAX; N++) {
    leds[N].anime(delta);
  }
}
*/
// helper to save and restore RTC_DATA
// this is ugly but we need this to get correct sizeof()
#define RTC_DATA(x) (uint32_t*)&x, sizeof(x)

bool saveRTCmemory() {
  setCrc8(&savedRTCmemory.crc8 + 1, sizeof(savedRTCmemory) - 1, savedRTCmemory.crc8);
  return ESP.rtcUserMemoryWrite(0, RTC_DATA(savedRTCmemory));
}

bool getRTCMemory() {
  ESP.rtcUserMemoryRead(0, RTC_DATA(savedRTCmemory));
  //Serial.print("CRC1="); Serial.println(getCrc8( (uint8_t*)&savedRTCmemory,sizeof(savedRTCmemory) ));
  return (setCrc8(&savedRTCmemory.crc8 + 1, sizeof(savedRTCmemory) - 1, savedRTCmemory.crc8));
}

/////////////////////////////////////////////////////////////////////////
//  crc 8 tool
// https://www.nongnu.org/avr-libc/user-manual/group__util__crc.html


//__attribute__((always_inline))
inline uint8_t _crc8_ccitt_update(uint8_t crc, const uint8_t inData) {
  uint8_t i;
  crc ^= inData;

  for (i = 0; i < 8; i++) {
    if ((crc & 0x80) != 0) {
      crc <<= 1;
      crc ^= 0x07;
    } else {
      crc <<= 1;
    }
  }
  return crc;
}

bool setCrc8(const void* data, const uint16_t size, uint8_t& refCrc) {
  uint8_t* dataPtr = (uint8_t*)data;
  uint8_t crc = 0xFF;
  for (uint8_t i = 0; i < size; i++) crc = _crc8_ccitt_update(crc, *(dataPtr++));
  //Serial.print("CRC "); Serial.print(refCrc); Serial.print(" / "); Serial.println(crc);
  bool result = (crc == refCrc);
  refCrc = crc;
  return result;
}

//get a value of a config key
String jobGetConfigStr(const String aKey) {
  String result = "";
  File aFile = MyLittleFS.open(CONFIG_FNAME, "r");
  if (!aFile) return (result);
  aFile.setTimeout(5);
  JSONVar jsonConfig = JSON.parse(aFile.readStringUntil('\n'));
  //D_println(jsonConfig);
  aFile.close();
  configOk = JSON.typeof(jsonConfig[aKey]) == F("string");
  if (configOk) result = (const char*)jsonConfig[aKey];
  return (result);
}

int jobGetConfigInt(const String aKey) {
  int result = 0;
  File aFile = MyLittleFS.open(CONFIG_FNAME, "r");
  if (!aFile) return (result);
  aFile.setTimeout(5);
  JSONVar jsonConfig = JSON.parse(aFile.readStringUntil('\n'));
  aFile.close();
  //D_println(JSON.typeof(jsonConfig[aKey]));
  configOk = JSON.typeof(jsonConfig[aKey]) == F("number");
  if (configOk) result = jsonConfig[aKey];
  return (result);
}



// set a value of a config key
//todo : check if config is realy write ?
bool jobSetConfigStr(const String aKey, const String aValue) {
  // read current config
  JSONVar jsonConfig;  // empry config
  File aFile = MyLittleFS.open(CONFIG_FNAME, "r");
  if (aFile) {
    aFile.setTimeout(5);
    jsonConfig = JSON.parse(aFile.readStringUntil('\n'));
    aFile.close();
  }
  if (aValue != "") {
    jsonConfig[aKey] = aValue;
  } else {
    jsonConfig[aKey] = undefined;
  }
  aFile = MyLittleFS.open(CONFIG_FNAME, "w");
  if (!aFile) return (false);
  //DV_println(JSON.stringify(jsonConfig));
  aFile.println(JSON.stringify(jsonConfig));
  aFile.close();
  return (true);
}

bool jobSetConfigInt(const String aKey, const int aValue) {
  // read current config
  JSONVar jsonConfig;  // empry config
  File aFile = MyLittleFS.open(CONFIG_FNAME, "r");
  if (aFile) {
    aFile.setTimeout(5);
    jsonConfig = JSON.parse(aFile.readStringUntil('\n'));
    aFile.close();
  }
  jsonConfig[aKey] = aValue;
  aFile = MyLittleFS.open(CONFIG_FNAME, "w");
  if (!aFile) return (false);
  //DV_println(JSON.stringify(jsonConfig));
  aFile.println(JSON.stringify(jsonConfig));
  aFile.close();
  return (true);
}


bool jobShowConfig() {
  // read current config
  Serial.println(F("--- CONFIG START---"));
  File aFile = MyLittleFS.open(CONFIG_FNAME, "r");
  if (aFile) {
    aFile.setTimeout(5);
    Serial.println(aFile.readStringUntil('\n'));
    aFile.close();
  }
  Serial.println(F("--- CONFIG END---"));
  return (aFile);
}



// check wifi status
void jobCheckWifi() {
  static uint8_t oldWiFiStatus = 99;
  uint8_t WiFiStatus = WiFi.status();
  if (oldWiFiStatus != WiFiStatus) {
    oldWiFiStatus = WiFiStatus;
    DV_println(WiFiStatus);
    //    WL_IDLE_STATUS      = 0,
    //    WL_NO_SSID_AVAIL    = 1,
    //    WL_SCAN_COMPLETED   = 2,
    //    WL_CONNECTED        = 3,
    //    WL_CONNECT_FAILED   = 4,
    //    WL_CONNECTION_LOST  = 5,
    //    WL_DISCONNECTED     = 6
    //    7: WL_AP_LISTENING
    //    8: WL_AP_CONNECTED

    // Check changement Connecté // non connecté
    WiFiConnected = (WiFiStatus == WL_CONNECTED);
    static bool wasConnected = false;
    if (wasConnected == WiFiConnected) return;

    wasConnected = WiFiConnected;
    jobUpdateLed0();
    if (WiFiConnected) {
      //setSyncProvider(getWebTime);
      //setSyncInterval(6 * 3600);
      // lisen UDP 23423
      //        Serial.println("Listen broadcast");
      //        myUdp.begin();
      //        Events.delayedPushMilli(illi(checkWWW_DELAY, evCheckWWW);  // will send mail
      //        Events.delayedPushMilli(checkAPI_DELAY, evCheckAPI);
    }
    DV_println(WiFiConnected);
    writeHisto(WiFiConnected ? F("wifi Connected") : F("wifi lost"), WiFi.SSID());
  }
}


// ajustement du clignotement de la led0
// si le poussoir BP0 est enfoncé  clignotement tout les 500ms ()
// si le wifi est OK clignotement toute les 5 secondes
// sinon clignotemnt toute les secondes (wifi off)
// la duree du pulse allumé correspond a l'utilisation CPU
void jobUpdateLed0() {
  uint cpu = 80*Events._percentCPU/100+2;
  DV_println(cpu);
  if (BP0.isOn()) {
    Led0.setMillisec(500, cpu);
    //ledLifeColor = rvb_blue;
    return;
  }
  if (WiFiConnected) {
    Led0.setMillisec(5000, cpu);
    //ledLifeColor = rvb_green;
    return;
  }
  Led0.setMillisec(1000, cpu);
  //ledLifeColor = rvb_red;
}
