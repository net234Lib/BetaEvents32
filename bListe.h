#include "evHelpers.h"
/****************
 *  liste simplifiée pour EvManager


 *  cloné a partir de SList.h de ESP8266 WiFi
 *  V1.0  pour les evHandlerUdp   
 * TODO : a utiliser pour evmanager
 * TODO : gerer correctement les protectde
 * 
 * Variables and constants in RAM (global, static), used 28500 / 80192 bytes (35%)

without UDP
Variables and constants in RAM (global, static), used 28500 / 80192 bytes (35%)
Code in flash (default, ICACHE_FLASH_ATTR), used 247896 / 1048576 bytes (23%)
With UDP  (+5Krom +200ram)
. Variables and constants in RAM (global, static), used 28672 / 80192 bytes (35%)
. Code in flash (default, ICACHE_FLASH_ATTR), used 252708 / 1048576 bytes (24%)
 * 
. Code in flash (default, ICACHE_FLASH_ATTR), used 252916 / 1048576 bytes (24%) * 
 * 
 * 
 ***********************/


#pragma once


// Base pour un node de la liste
template<typename T>
class BItem {
public:
  BItem()
    : _next(nullptr) {}
  //T* next(T* aItem) {return(aItem->_next);}
  //protected:
  T* _next;
};

// Base pour une classe list
template<typename T>
class BList {

public:
  BList()
    : _first(nullptr) {}


  //protected:
  void _add(T* self) {

    if (_first == nullptr) {
      _first = self;
      return;
    }

    T* prev = _first;
    while (prev->_next) prev = prev->_next;
    prev->_next = self;
  }

  void
  _remove(T* self) {
    if (_first == self) {
      _first = self->_next;
      self->_next = 0;
      return;
    }

    for (T* prev = _first; prev->_next; prev = prev->_next) {
      if (prev->_next == self) {
        prev->_next = self->_next;
        self->_next = 0;
        return;
      }
    }
    DT_println("node not found!!!")
  }

  T* _first;
};
