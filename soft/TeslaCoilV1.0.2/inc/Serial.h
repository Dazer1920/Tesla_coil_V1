#ifndef _SERIAL_H
#define _SERIAL_H

#include "System.h"
#include "BluetoothConfig.h"
#include "stdio.h"
#include "Util.h"
#include "string.h"
#include "ProcessSystemData.h"
#include "ProcessInterrupterData.h"
#include "ProcessMidiData.h"

#define CURRENT_BAUDRATE 57600

#define SIZE_RX_BUFF 4
#define SIZE_TX_BUFF 5

void initUsart();

void setBaudrateUsart(uint32_t baud);

void UsartWriteDMA(uint8_t *data, uint16_t len);
void UsartWrite(uint8_t data);
void TransmitFrame(uint8_t *frame);

void processTxFrames();


#endif // _SERIAL_H