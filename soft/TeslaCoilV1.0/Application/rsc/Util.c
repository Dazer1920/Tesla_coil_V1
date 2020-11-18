#include "Util.h"

void cleareBuff(uint8_t *buff, uint8_t size) {
  for(uint8_t i = 0; i < size; i++) {
    buff[i] = 0;
  }
}