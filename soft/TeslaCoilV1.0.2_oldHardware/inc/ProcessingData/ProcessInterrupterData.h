#ifndef _PROCESS_INTERRUPTER_DATA_H
#define _PROCESS_INTERRUPTER_DATA_H

#include "System.h"
#include "Timers.h"

#define RX_INTERRUPTER_DATA_KEY 0x02
#define RX_INTERRUPTER_DATA_MSK (0x80 | (RX_INTERRUPTER_DATA_KEY << 5))

#define RX_SET_PULSE_WHITH RX_INTERRUPTER_DATA_MSK | 0x01
#define RX_SET_FREQUENCY RX_INTERRUPTER_DATA_MSK | 0x02
#define RX_SET_MODULATER RX_INTERRUPTER_DATA_MSK | 0x03
#define RX_SET_PULSE RX_INTERRUPTER_DATA_MSK | 0x04
#define RX_SET_ENABLE RX_INTERRUPTER_DATA_MSK | 0x05

#define MAX_PULSE 75
#define MAX_FREQ 400

void offInterrupter();

void setEnbale(uint8_t *buff);
void setPulse(uint8_t *buff);
void setPW(uint8_t *buff);
void setFrequency(uint8_t *buff);
void setModulater(uint8_t *buff);

void processModulater();

#endif // _PROCESS_INTERRUPTER_DATA_H