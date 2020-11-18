#include "BluetoothConfig.h"

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

uint8_t countRequests = 1, countBauds = 0, countCommands = 0;

boolean isBluetoothConfig = false;
boolean isBluetoothConfigurate = false;

BluetoothEvents CurrentEvent = START_CONFIG;

uint16_t countTimeOutBluetoothEventTimer, timeOutBluetoothEventTimer;
BluetoothEvents eventTimer;

void SetBluetoothEventTimer(BluetoothEvents event, uint16_t timeOut) {
  timeOutBluetoothEventTimer = timeOut;
  countTimeOutBluetoothEventTimer = 0;
  eventTimer = event;
}

void processBluetoothEventTimer() {
  if(eventTimer != NO_EVENT) {
    if(countTimeOutBluetoothEventTimer >= timeOutBluetoothEventTimer) {
      CurrentEvent = eventTimer;
      eventTimer = NO_EVENT;
    } else countTimeOutBluetoothEventTimer++;
  }
}

void StopBluetoothEventTimer() {
  eventTimer = NO_EVENT;
  countTimeOutBluetoothEventTimer = 0;
  timeOutBluetoothEventTimer = 0;
}

void cleareCounterBluetoothEventTimer() {
  countTimeOutBluetoothEventTimer = 0;
}

void setBluetoothEvent(BluetoothEvents event) {
  CurrentEvent = event;
}

void startConfig() {
  SetBluetoothEventTimer(CHECK_CONFIG, START_TIMEOUT);
}

void checkConfig() {
  if(countBauds >= NUMBER_BAUDS && countRequests >= NUMBER_REQUESTS) {
    CurrentEvent = ERROR_CONFIG;
    return;
  }
  
  isBluetoothConfigurate = false;
  isBluetoothConfig = false;
  
  setBaudrateUsart(baudsTable[countBauds]);
  UsartWriteDMA(CHECK_CONFIG_COMMAND_REQUEST, strlen(CHECK_CONFIG_COMMAND_REQUEST));
  
  SetBluetoothEventTimer(CHECK_CONFIG, RESPONSE_TIME);
  setStateLed(LED_CHECK_CONFIG);
  
  if(countRequests >= NUMBER_REQUESTS) {
    countRequests = 0;
    countBauds++;
  }
  countRequests++;
}

void answerCheckConfig() {
  StopBluetoothEventTimer();
  countRequests = 0;
  
  extern uint32_t currentBaudRate;

  if(currentBaudRate == CURRENT_BAUDRATE) setBluetoothEvent(CONFIG_END);
  else setBluetoothEvent(CONFIGURARE);
}

void cofigEnd() {
  StopBluetoothEventTimer();
  isBluetoothConfig = true;
  isBluetoothConfigurate = false;
  countRequests = 0;
  countBauds = 0;
  countCommands = 0;
  setStateLed(LED_CONFIG_END);
}

void configurate() {
  if(countCommands >= NUMBER_AT_COMMANDS) {
    NVIC_SystemReset();
  }
  
  if(countRequests >= NUMBER_REQUESTS) {
    CurrentEvent = ERROR_CONFIG;
    return;
  }
  countRequests++;
  
  isBluetoothConfigurate = true;
  isBluetoothConfig = false;
  
  UsartWriteDMA(ATCommands[countCommands], strlen((char *) ATCommands[countCommands]));
  SetBluetoothEventTimer(CONFIGURARE, RESPONSE_TIME_CONFIGURATE);
  setStateLed(LED_CONFIGURATE);
}

void answerConfigurate() {
  StopBluetoothEventTimer();
  countRequests = 0;
  
  countCommands++;
  setBluetoothEvent(CONFIGURARE);
}

void errorConfig() {
  extern boolean isError;
  isError = true;
}


uint8_t countChangeConnection;
boolean isConnected = false;

void Connected() {
  SetBluetoothEventTimer(CHECK_CONNECTION, 1500);
  TpansmitValuesProtection();
  isConnected = true;
  countChangeConnection = 0;
}

void Disconnected() {
  countChangeConnection = 0;
  StopBluetoothEventTimer();
  isConnected = false;
  
  setStopMidiPlayer();
  offInterrupter();
}

void ChangeConnection() {
  if(countChangeConnection >= 2) {
    StopBluetoothEventTimer();
    setBluetoothEvent(DISCONNECTED);
    setStateLed(LED_DISCONNECT);
    return;
  }
  
  countChangeConnection++;
  SetBluetoothEventTimer(CHECK_CONNECTION, 1500);
}

void processBluetooth() {
  switch(CurrentEvent) {
  default: break;
  
  case NO_EVENT: break;
  
  case START_CONFIG: CurrentEvent = NO_EVENT; startConfig(); break;
  case CHECK_CONFIG: CurrentEvent = NO_EVENT; checkConfig(); break;
  case ANSWER_CHECK_CONFIG: CurrentEvent = NO_EVENT; answerCheckConfig(); break;
  case CONFIG_END: CurrentEvent = NO_EVENT; cofigEnd(); break;
  case CONFIGURARE: CurrentEvent = NO_EVENT; configurate(); break;
  case ANSWER_CONFIGURARE: CurrentEvent = NO_EVENT; answerConfigurate(); break;
  case ERROR_CONFIG: CurrentEvent = NO_EVENT; errorConfig(); break;
  case CONNECTED: CurrentEvent = NO_EVENT; Connected(); break;
  case DISCONNECTED: CurrentEvent = NO_EVENT; Disconnected(); break;
  case CHECK_CONNECTION: CurrentEvent = NO_EVENT; ChangeConnection(); break;
  }
}