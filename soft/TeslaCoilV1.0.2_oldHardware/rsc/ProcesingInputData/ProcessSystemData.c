#include "ProcessSystemData.h"

void setKey1(uint8_t key) {
  if(key == RX_IDENTIFICATION_KEY1_VAL) {
    uint8_t frame[] = {TX_IDENTIFICATION_KEY1, TX_IDENTIFICATION_KEY1_VAL, 0, 0};
    TransmitFrame(frame);
  }
}

void setKey2(uint8_t key) {
  if(key == RX_IDENTIFICATION_KEY2_VAL) {
    uint8_t frame[] = {TX_IDENTIFICATION_KEY2, TX_IDENTIFICATION_KEY2_VAL, 0, 0};
    TransmitFrame(frame);
  }
}