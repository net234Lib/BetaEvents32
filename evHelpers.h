//Helpers
// generic helpers out of evManager library
#pragma once
#include <arduino.h>

/*
#ifndef time_t
#if  defined(__AVR__)

#define time_t unsigned long
#endif
#endif
*/

#ifndef NO_DEBUG

// D_println(variable); permet d'afficher le nom de variable suivit de sa valeur
#define DT_println(x) Serial.println(F(#x));
#define DV_println(x) Serial.print(F(#x " => '")); Serial.print(x); Serial.println("'");
#define DTV_println(x,y) Serial.print(F(#x " => '")); Serial.print(y); Serial.println("'");

//idem sans retour chariot
#define DT_print(x) Serial.print(F(#x));Serial.print(", ");
#define DV_print(x) Serial.print(F(#x " => '")); Serial.print(x); Serial.print("', ");
#define DTV_print(x,y) Serial.print(F(#x " => '")); Serial.print(y); Serial.print("', ");

//idem mais en HEXA
#define D_printHEXln(x) Serial.print(F(#x " => '0x")); Serial.print(x,HEX); Serial.println("'");


#else

#define DV_println(...)  ;
#define DV_print(...)    ;
#define DT_println(...)  ;
#define DT_print(...)    ;
#define DTV_println(...)  ;
#define DTV_print(...)    ;
#define D_printHEXln(...)  ;

#endif

#define T_println(x) Serial.println(F(#x));
#define V_println(x) Serial.print(F(#x " => '")); Serial.print(x); Serial.println("'");
#define TV_println(x,y) Serial.print(F(#x " => '")); Serial.print(y); Serial.println("'");
#define T_println(x) Serial.println(F(#x));
#define V_print(x) Serial.print(F(#x " => '")); Serial.print(x); Serial.print("', ");
#define TV_print(x,y) Serial.print(F(#x " => '")); Serial.print(y); Serial.print("', ");
#define V_printHEXln(x) Serial.print(F(#x " => '0x")); Serial.print(x,HEX); Serial.println("'");


String niceDisplayDelay(time_t aDelay);
String Digit2_str(const uint16_t value);
void   helperReset();
int    helperFreeRam();

// extrait un element terminé par aKey de la chaine aString
// si aKey est absent toute la chaine est extaite
String grabFromStringUntil(String & aString, const char aKey);
String grabFromStringUntil(String & aString, const String aKey);
