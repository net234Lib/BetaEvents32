#include "evHelpers.h"

//Helper

String Digit2_str(const uint16_t value) {
  String result = "";
  if (value < 10) result = '0';
  result += value;
  return result;
}





// extrait un element terminé pae aKey de la chaine aString
// si aKey est absent toute la chaine est extaite
String grabFromStringUntil(String& aString, const char aKey) {
  String result;
  int pos = aString.indexOf(aKey);
  if (pos == -1) {
    result = aString;
    aString = "";
    return (result);  // not match
  }
  result = aString.substring(0, pos);
  //aString = aString.substring(pos + aKey.length());
  aString = aString.substring(pos + 1);
  return (result);
}

String grabFromStringUntil(String& aString, const String aKey) {
  String result;
  int pos = aString.indexOf(aKey);
  if (pos == -1) {
    result = aString;
    aString = "";
    return (result);  // not match
  }
  result = aString.substring(0, pos);
  aString = aString.substring(pos + aKey.length());
  //aString = aString.substring(pos + 1);
  return (result);
}



// affichage d'un delay en secondes sous la forme
// xx s             si < 1 minute
// xx m yy s        si < 1 heure
// xx h yy m        si < 1 jour
// xx j yy h        pour le reste


String niceDisplayDelay(time_t aDelay) {
  String txt;
  if (aDelay < 60) {
    txt = String(aDelay);
    txt += "s";
    return txt;
  }
  if (aDelay < 3600) {
    txt = String(aDelay / 60);
    txt += "m ";
    txt += String(aDelay % 60);
    txt += "s";
    return txt;
  }
  if (aDelay < 3600L * 24) {
    aDelay = aDelay / 60;
    txt = String(aDelay / 60);
    txt += "h ";
    txt += String(aDelay % 60);
    txt += "m";
    return txt;
  }
  aDelay = aDelay / 3600;
  txt = String(aDelay / 24);
  txt += "J ";
  txt += String(aDelay % 24);
  txt += "h";
  return txt;
}

String niceDisplayTime(const time_t time, bool full) {

  String txt;
  // we supose that time < NOT_A_DATE_YEAR is not a date
  if (year(time) < NOT_A_DATE_YEAR) {
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
