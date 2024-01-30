/****************
 *  liste simplifiée pour EvManager


 *  cloné a partir de SList.h de ESP8266 WiFi
 *  V1.0  pour les evHandlerUdp   
 * TODO : a utiliser pour evmanager
 * TODO : gerer correctement les protectde
 * 
 * Variables and constants in RAM (global, static), used 28500 / 80192 bytes (35%)

║   SEGMENT  BYTES    DESCRIPTION
╠══ DATA     1504     initialized variables
╠══ RODATA   1004     constants       
╚══ BSS      25992    zeroed variables
. Instruction RAM (IRAM_ATTR, ICACHE_RAM_ATTR), used 60331 / 65536 bytes (92%)
║   SEGMENT  BYTES    DESCRIPTION
╠══ ICACHE   32768    reserved space for flash instruction cache
╚══ IRAM     27563    code in IRAM    
. Code in flash (default, ICACHE_FLASH_ATTR), used 247752 / 1048576 bytes (23%)
║   SEGMENT  BYTES    DESCRIPTION
╚══ IROM     247752   code in flash   

 * 
 * 
 * 
 * 
 ***********************/


#pragma once


// Base pour un node de la liste
template<typename T>
class BItem {
public:
  BItem() : _next(nullptr) {}
  //T* next(T* aItem) {return(aItem->_next);}
  //protected:
  T* _next;
};

// Base pour une classe list
template<typename T>
class BList {

public:
  BList() : _first(nullptr) {}



  void _add(T* self) {
    T* tmp = _first;
    _first = self;
    self->_next = tmp;
  }

  void _remove(T* self) {
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
  }

  T* _first;
};
