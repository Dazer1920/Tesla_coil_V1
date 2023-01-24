#include "Util.h"

uint8_t Volume = 100;
uint16_t portamentoTime = 0;
bool isUseVelocity = false, portamentoType = false;

const uint16_t frequencyMidiNote[] = {
  8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 21, 22, 23, 24, 26, 27, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 
  58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139,147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 
  311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 830, 880, 932, 988, 1046, 1108, 1174, 1244, 1318, 
  1397, 1480, 1568, 1661, 1760, 1864, 1975, 2092, 2217, 2349, 2488, 2636, 2793, 2959, 3135, 3322, 3519, 3728, 3950, 4185, 4434, 4697, 4977, 
  5272, 5586, 5918, 6270, 6643, 7038, 7456, 7900, 8369, 8867, 9394, 9953, 10545, 11171, 11836, 12540
};

uint16_t convertVoltageToAdc(float voltage, float r1, float r2) {
  float dividet = r2 / (r1 + r2);
  uint16_t val = (uint16_t) (((voltage * dividet) / 3.3f) * 4096.0f);
  return val;
}

uint16_t convertTemperatureToAdc(float val) {
  val += 273.15f;
  val = (3988.0 * 298.15) / val;
  val -= 3988.0;
  val /= 298.15;

  val = pow(2.718281828, val);
  val *= 10000.0;

  val  /= 9710.0;
  val += 1.0;

  uint16_t adc = (uint16_t) (4096.0 / val);

  adc = 4095 - adc;
  return adc;
}

uint16_t filterData(uint16_t data, FilterData *f) {
  f->data = (f->A * f->data + data) >> f->K;
  return f->data;
}

uint32_t convertData(uint8_t key) {
  uint32_t data = ((~key) + key);
  return data % 10;
}

uint8_t calculateCheckSum(uint8_t *buff, uint8_t len) {
  uint8_t sum = 0;
  for(uint8_t i = 0; i < len; i++) {
    sum += buff[i];
  }
  
  return ((~(sum + 1)) & 0x007f);
}

uint32_t abs(int32_t a) {
  if(a > 0) return a;
  else return -a;
}

int16_t map(int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

extern MidiNote notes[NUMBER_NOTES];
extern MidiController MidiControllers[16];
extern Timer Timers[NUMBER_VOICES];

float calculatePitchBend(int16_t p) {
  float fbend_amount = ((float) p) / ((float) (0x2000));
  return (1.0f - 0.1 * fbend_amount);
}

void setPitchBendTimer(uint8_t i) {
  uint32_t per = (uint32_t) ((1000000UL / frequencyMidiNote[notes[i].note]) * 
    calculatePitchBend(MidiControllers[notes[i].channel].pitchBend));
  setPeriodTimer(per, i);
}

void playNotes() {  
  for(uint8_t i = 0; i < NUMBER_VOICES; i++) {
    if(notes[i].isPlaing == false) {
      setEnableTimer(false, i, true);
    } else {
      uint16_t pw = map(notes[i].note, MIN_NOTE, MAX_NOTE, PULSE_AT_MIN_NOTE, PULSE_AT_MAX_NOTE);
      if(isUseVelocity == true) {
        pw = (pw * notes[i].velocity) / 127;
        pw = (pw * MidiControllers[notes[i].channel].volume) / 127;
      }
      
      setPulseTimer(pw, i);
      setPitchBendTimer(i);
      setEnableTimer(true, i, true);
    }
  }
}