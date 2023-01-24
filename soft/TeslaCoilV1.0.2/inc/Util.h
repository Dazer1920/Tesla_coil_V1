#ifndef _UTIL_H
#define _UTIL_H

#include "System.h"
#include "ProcessMidiEvents.h"
#include "Timers.h"
#include "Math.h"

#define PULSE_AT_MAX_NOTE 7
#define PULSE_AT_MIN_NOTE 100

#define TIME_OUT_ON_NOTE 2000 // in ms

typedef struct {
  uint16_t data;
  uint8_t K;
  uint8_t A;
} FilterData;

int16_t map(int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max);
uint32_t abs(int32_t a);
uint32_t convertData(uint8_t key);
uint16_t convertVoltageToAdc(float voltage, float r1, float r2);
uint16_t convertTemperatureToAdc(float val);

uint16_t filterData(uint16_t data, FilterData *f);

void playNotes();
void setPitchBendTimer(uint8_t i);

#endif // _UTIL_H