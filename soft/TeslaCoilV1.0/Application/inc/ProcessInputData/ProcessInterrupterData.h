#ifndef _PROCESS_INTERRUPTER_DATA_H
#define _PROCESS_INTERRUPTER_DATA_H

#include "System.h"
#include "Connection.h"

#define RX_INTERRUPTER_DATA_KEY 32

#define RX_SET_PULSE_WHITH 55
#define RX_SET_FREQUENCY 32
#define RX_SET_MODULATER 56
#define RX_SET_PULSE 43
#define RX_SET_ENABLE 23

#define MAX_PULSE 75
#define MAX_FREQ 400

void readInterrupterData(uint8_t *data);
void processModulaterInterrupter();
void OffInterrupter();

#endif // _PROCESS_INTERRUPTER_DATA_H