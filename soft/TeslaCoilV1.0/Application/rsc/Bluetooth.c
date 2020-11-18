#include "Bluetooth.h"

#define NUMBER_AT_COMMANDS 2

uint8_t ATCommands[][25] = { // config commands
  {"AT+NAMETeslaCoil"},
  //{"AT+ENABLEIND0\r\n"},
  {"AT+BAUD7"}
};

#define NUMBER_BAUDS 15

uint32_t baudsTable[] = {
  57600,
  600,
  1200,
  2400,
  4800,
  9600,
  14400,
  19200,
  38400,
  56000,
  115200,
  230400,
  460800,
  921600,
  1382400
};

Enable isConfiguredBt = OFF, isConfig = OFF;
BluetoothConfigEvent bluetoothConfigEvent = NO_EVENT;
uint8_t countRequest = 0, countBaud = 0, countCommands;

uint32_t countBtEventTimer = 0;
BluetoothConfigEvent timerEvent = NO_EVENT;

void stopEventTimer() {
  countBtEventTimer = 0;
  timerEvent = NO_EVENT;
}

void cleareVaues() {
  stopEventTimer();
  countRequest = 0;
  countBaud = 0;
}

void setEventTimer(uint32_t ms, BluetoothConfigEvent event) {
  countBtEventTimer = ms;
  timerEvent = event;
}

void processBluetoothEventTimer() {
  if(timerEvent != NO_EVENT) {
    if(--countBtEventTimer == 0) {
      bluetoothConfigEvent = timerEvent;
      timerEvent = NO_EVENT;
    }
  }
}

void setBtEvent(BluetoothConfigEvent event) {
  bluetoothConfigEvent = event;
}

void startConfigBluetooth() {
  setEventTimer(START_TIMEOUT, CHECK_BAUD);
}

void BluetoothCheckBaudrate() {
  if(countBaud >= NUMBER_BAUDS) {
    cleareVaues();
    noBluetooth();
    return;
  }
  
  isConfig = OFF;
  setBaudSerial(baudsTable[countBaud]);
  
  SerialWriteString(CHECK_AT_COMMAND);
  
  setEventTimer(TIMEOUT_BETWEN_REQUESTS, CHECK_BAUD);
  
  if(countRequest >= NUMBER_REQUEST_IN_CHECK_BAUD) {
    countRequest = 0;
    countBaud++;
  }
  countRequest++;
}

void answerCheckBt() {
  cleareVaues();
  if(huart1.Init.BaudRate == BAUDRATE_DEVICE) bluetoothConfigEvent = BT_CONFIGURED; 
  else bluetoothConfigEvent = CONFIGURATE_BT;
}

void configurateBt() {
  if(countCommands >= NUMBER_AT_COMMANDS) {
    countCommands = 0;
    cleareVaues();
    setReboot();
    return;
  }
  
  if(countRequest >= NUMBER_REQUEST_IN_CONFIG) {
    countCommands = 0;
    cleareVaues();
    noBluetooth();
    return;
  }
  countRequest++;
  
  isConfig = ON;
  SerialWriteString(ATCommands[countCommands]);
  
  setEventTimer(TIMEOUT_BETWEN_REQUESTS, CONFIGURATE_BT);
}

void answerConfigBt() {
  countRequest = 0;
  countCommands++;
  stopEventTimer();
  bluetoothConfigEvent = CONFIGURATE_BT;
}

void BluetoothConfigured() {
  cleareVaues();
  isConfiguredBt = ON;
  startApp();
}

void processBluetooth() {
  switch(bluetoothConfigEvent) {
  default: break;
  case NO_EVENT: break;
  
  case CHECK_BAUD: bluetoothConfigEvent = NO_EVENT; BluetoothCheckBaudrate(); break;
  case BT_CONFIGURED: bluetoothConfigEvent = NO_EVENT; BluetoothConfigured(); break;
  case BT_ANSWER_CHECK: bluetoothConfigEvent = NO_EVENT; answerCheckBt(); break;
  case BT_ANSWER_CONFIG: bluetoothConfigEvent = NO_EVENT; answerConfigBt(); break;
  case CONFIGURATE_BT: bluetoothConfigEvent = NO_EVENT; configurateBt(); break;
  }
}