#include "Serial.h"

extern Enable isConfiguredBt, isConfig;

uint8_t dataUsart1;

void initSerial() {
  if(HAL_UART_Receive_IT(&huart1, &dataUsart1, 1) != HAL_OK) {
    Error_Handler();
  }
}

void setBaudSerial(uint32_t baud) {
  huart1.Init.BaudRate = baud;
  if(HAL_UART_Init(&huart1) != HAL_OK) {
    Error_Handler();
  }
  
  if(HAL_UART_Receive_IT(&huart1, &dataUsart1, 1) != HAL_OK) {
    Error_Handler();
  }
}

void SerialWriteString(uint8_t *string) {
  if(HAL_UART_Transmit_DMA(&huart1, string, strlen((char*)string)) == HAL_OK) setStatusLed(BT_CHECK_BAUD_LED);
  else Error_Handler();
}

Enable TriggerRx = OFF;
uint8_t RxBuff[SIZE_RX_BUFF], countRxBuff, checkSumRx, RxSum;
uint8_t TxBuff[SIZE_TX_BUFF], checkSumTx;

void SerialWriteFrame(uint8_t *frame, uint8_t len) {
  checkSumTx = 0;
  
  for(uint8_t i = 0; i < len; i++) {
    checkSumTx += frame[i];
    TxBuff[i + 2] = frame[i];
  }
  
  TxBuff[0] = START_BYTE;
  TxBuff[1] = ((~(checkSumTx + 1)) & 0x007f);
  TxBuff[len + 2] = STOP_BYTE;
  
  if(HAL_UART_Transmit_DMA(&huart1, TxBuff, len + 3) != HAL_OK) Error_Handler();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if(huart->Instance == huart1.Instance) {
    if(isConfiguredBt == ON) {
      if(dataUsart1 > 0x7f) {
        if(dataUsart1 == START_BYTE) {
          RxSum = 0;
          countRxBuff = 0;
          checkSumRx = 0;
          TriggerRx = ON;
        } else if(dataUsart1 == STOP_BYTE) {
          TriggerRx = OFF;
          if(checkSumRx == ((~(RxSum + 1)) & 0x007f)) {
            switch(RxBuff[0]) {
            default: break;
            
            case RX_SYSTEM_DATA_KEY: readSystemData(RxBuff); break;
            case RX_INTERRUPTER_DATA_KEY: readInterrupterData(RxBuff); break;
            case RX_MIDI_PLAYER_KEY: readMidiData(RxBuff); break;
            }
          }
        }
      } else {
        if(TriggerRx == ON) {
          if(countRxBuff == 0) checkSumRx = dataUsart1;
          else {
            RxBuff[countRxBuff - 1] = dataUsart1;
            RxSum += dataUsart1;
          }
          countRxBuff = (countRxBuff < SIZE_RX_BUFF)? countRxBuff + 1 : countRxBuff;
        }
      }
    } else {
      if(countRxBuff >= SIZE_RX_BUFF) {
        countRxBuff = 0;
        cleareBuff(RxBuff, SIZE_RX_BUFF);
      }
      
      RxBuff[countRxBuff] = dataUsart1;
      countRxBuff++;
      
      if(strstr((const char*) RxBuff, CHECK_ANSWER_BT) != NULL && isConfig == OFF) {
        countRxBuff = 0;
        cleareBuff(RxBuff, SIZE_RX_BUFF);
        setBtEvent(BT_ANSWER_CHECK);
      } else {
        if(strstr((const char*) RxBuff, ANSWER_CONFIG) != NULL && isConfig == ON) {
          countRxBuff = 0;
          cleareBuff(RxBuff, SIZE_RX_BUFF);
          setBtEvent(BT_ANSWER_CONFIG);
        }
      }
    }
    
    HAL_UART_Receive_IT(huart, &dataUsart1, 1);
  }
}