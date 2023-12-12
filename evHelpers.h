//Helpers
// generic helpers out of evManager library
#pragma once
#include <arduino.h>

// D_println(variable); permet d'afficher le nom de variable suivit de sa valeur

#define D_println(x) Serial.print(F(#x " => '")); Serial.print(x); Serial.println("'");

//idem sans retour chario
#define D_print(x) Serial.print(F(#x " => '")); Serial.print(x); Serial.print("', ");

//idem mais en HEXA
#define DX_println(x) Serial.print(F(#x " => '0x")); Serial.print(x,HEX); Serial.println("'");

String Digit2_str(const uint16_t value);
//void   helperReset();
//int    helperFreeRam();

// extrait un element terminé par aKey de la chaine aString
// si aKey est absent toute la chaine est extaite
String grabFromStringUntil(String & aString, const char aKey);
