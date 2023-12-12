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
String grabFromStringUntil(String & aString, const char aKey) {
  String result;
  int pos = aString.indexOf(aKey);
  if ( pos == -1 ) {
    result = aString;
    aString = "";
    return (result);  // not match
  }
  result = aString.substring(0, pos);
  //aString = aString.substring(pos + aKey.length());
  aString = aString.substring(pos + 1);
  return (result);
}
