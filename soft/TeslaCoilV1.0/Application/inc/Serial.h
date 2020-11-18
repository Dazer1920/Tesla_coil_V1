#ifndef _SERIAL_H
#define _SERIAL_H

#include "System.h"
#include "Bluetooth.h"
#include "LED.h"
#include "Util.h"
#include "ProcessSystemData.h"
#include "ProcessInterrupterData.h"
#include "ProcessMidiData.h"
#include "Protection.h"

#include <string.h>

#define START_BYTE 0x94
#define STOP_BYTE 0xA3

#define SIZE_RX_BUFF 16
#define SIZE_TX_BUFF 16

#define BAUDRATE_DEVICE 57600

void initSerial();
void setBaudSerial(uint32_t baud);

void SerialWriteString(uint8_t *string);

void SerialWriteFrame(uint8_t *frame, uint8_t len);

#endif // _SERIAL_H