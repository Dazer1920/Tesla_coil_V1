#include "Serial.h"

extern bool isBluetoothConfig;
extern bool isBluetoothConfigurate;

void processFrames(uint8_t *frame);

uint32_t currentBaudRate = 0;

void initUsart() {
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  RCC->AHBENR |= RCC_AHBENR_DMAEN;
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  GPIOA->MODER |= GPIO_MODER_MODER10_1 | GPIO_MODER_MODER9_1;
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR10 | GPIO_OSPEEDR_OSPEEDR9;
  GPIOA->AFR[1] |= (1 << (2 * 4)) | (1 << (1 * 4));
  
  USART1->CR1 |= USART_CR1_UE;
  USART1->CR1 &= ~USART_CR1_M;
  USART1->CR2 &= ~USART_CR2_STOP;
  USART1->CR1 |= USART_CR1_RE;
  USART1->CR1 |= USART_CR1_TE;
  
  USART1->CR1 |= USART_CR1_RXNEIE;
  USART1->CR3 |= USART_CR3_DMAT;
  
  NVIC_SetPriority(USART1_IRQn, 1);
  NVIC_EnableIRQ(USART1_IRQn);
  
  DMA1_Channel2->CCR &= ~DMA_CCR_MSIZE;
  DMA1_Channel2->CCR &= ~DMA_CCR_PSIZE;
  DMA1_Channel2->CCR &= ~DMA_CCR_PL;
  DMA1_Channel2->CCR &= ~DMA_CCR_CIRC;
  
  DMA1_Channel2->CCR |= DMA_CCR_MINC;
  DMA1_Channel2->CCR &= ~DMA_CCR_PINC;
  DMA1_Channel2->CCR |= DMA_CCR_DIR;
  DMA1_Channel2->CCR |= DMA_CCR_TCIE;

  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 3);
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

void setBaudrateUsart(uint32_t baud) {
  currentBaudRate = baud;
  USART1->BRR = (SystemCoreClock + baud / 2) / baud;
}

volatile bool UsartBusy = false;

void UsartWriteDMA(uint8_t *data, uint16_t len) {
  UsartBusy = true;
  DMA1_Channel2->CCR &= ~DMA_CCR_EN;
  
  DMA1_Channel2->CNDTR = len;
  DMA1_Channel2->CPAR = (uint32_t) (&USART1->TDR);
  DMA1_Channel2->CMAR = (uint32_t) data;
  
  DMA1_Channel2->CCR |= DMA_CCR_EN;
}

void DMA1_Channel2_3_IRQHandler() {
  if(DMA1->ISR & DMA_ISR_TCIF2) {
    DMA1->IFCR |= DMA_IFCR_CTCIF2;
    UsartBusy = false;
  }
}

uint8_t txBuff[50], txSum, countTransFrames = 0;
void TransmitFrame(uint8_t *frame) {
  txSum = 0;
  
  for(uint8_t i = 0; i < 4; i++) {
    txBuff[i + (countTransFrames * 5)] = frame[i];
    txSum += frame[i];
  }
  txBuff[4 + (countTransFrames * 5)] = ((~(txSum + 1)) & 0x007f);
  countTransFrames = (countTransFrames > 9)? countTransFrames : countTransFrames + 1;
}

void processTxFrames() {
  if(UsartBusy) return;
  
  if(countTransFrames > 0) {
    UsartWriteDMA(txBuff, countTransFrames * 5);
    countTransFrames = 0;
  }
}

uint8_t sum, countBuff, buff[16], rxData;
void USART1_IRQHandler() {
  if(USART1->ISR & USART_ISR_RXNE) {
    USART1->ISR &= ~USART_ISR_RXNE;

    rxData = USART1->RDR;
    
    if(isBluetoothConfig == true) {
      if(rxData > 127) {
        sum = 0;
        countBuff = 0;
      } else {
        if(countBuff == SIZE_RX_BUFF && rxData == ((~(sum + 1)) & 0x007f)) {
          processFrames(buff);
        }
      }
      
      if(countBuff < SIZE_RX_BUFF) {
        buff[countBuff] = rxData;
        countBuff++;
        sum += rxData;
      }
    } else {
      if(isBluetoothConfigurate == false && countBuff >= strlen(CHECK_CONFIG_COMMAND_ANSWER)) {
        countBuff = 0;
        memset(buff, 0, SIZE_RX_BUFF);
      } else 
      if(isBluetoothConfigurate == true && countBuff >= strlen(CONFIGURATE_COMMAND_ANSWER)) {
        countBuff = 0;
        memset(buff, 0, SIZE_RX_BUFF);
      }
      
      buff[countBuff] = rxData;
      countBuff++;
      
      if(isBluetoothConfigurate == false && strstr((const char*) buff, CHECK_CONFIG_COMMAND_ANSWER) != NULL) {
        countBuff = 0;
        memset(buff, 0, SIZE_RX_BUFF);
        setBluetoothEvent(ANSWER_CHECK_CONFIG);
      } else 
      if(isBluetoothConfigurate == true && strstr((const char*) buff, CONFIGURATE_COMMAND_ANSWER) != NULL) {
        countBuff = 0;
        memset(buff, 0, SIZE_RX_BUFF);
        setBluetoothEvent(ANSWER_CONFIGURARE);
      }
    }
  }
}

void processFrames(uint8_t *frame) {
  bool fault = false;
  
  switch(frame[0]) {
  default: fault = true; break;
  
  case RX_IDENTIFICATION_KEY1: setKey1(frame[1]); break;
  case RX_IDENTIFICATION_KEY2: setKey2(frame[1]); break;
  case RX_CHANGE_CONNECTION: setBluetoothEvent(CONNECTED); break;
  case RX_DISCONNECT: setBluetoothEvent(DISCONNECTED); break;

  case RX_SET_PULSE_WHITH: setPW(frame); break;
  case RX_SET_FREQUENCY: setFrequency(frame); break;
  case RX_SET_MODULATER: setModulater(frame); break;
  case RX_SET_PULSE: setPulse(frame); break;
  case RX_SET_ENABLE: setEnbale(frame); break;
  
  case RX_SET_START_PLAYER: setStartMidiPlayer(); break;
  case RX_SET_STOP_PLAYER: setStopMidiPlayer(); break;
  case RX_SET_RESET_PLAYER: resetMidiPlayer(); break;
  case RX_SET_CURRENT_TICK: setCurrentTick(frame); break;
  case RX_SET_TEMPO_PLAYER: setTempo(frame); break;
  case RX_SET_RESOLUTION_PLAYER: setResolution(frame); break;
  case RX_SET_MIDI_EVENT_PLAYER: setMidiEvent(frame); break;
  case RX_SET_MIDI_EVENT_TICK_PLAYER: setMidiEventTick(frame); break;
  case RX_SET_MAX_TICKS_PLAYER: setMaxTick(frame); break;
  case RX_SET_VOLUME: setVolume(frame); break;
  case RX_SET_USE_VELOCITY: setUseVelocity(frame); break;
  case RX_SET_PORTAMENTO_TIME: setPortamentoTime(frame); break;
  case RX_MAX_SIZE: setEndRequest(); break;
  case RX_SET_ATTACK: setAttack(frame); break;
  case RX_SET_DECAY: setDecay(frame); break;
  case RX_SET_SUSTAIN: setSustain(frame); break;
  case RX_SET_RELEASE: setRelease(frame); break;

  }
  
  if(fault == false) cleareCounterBluetoothEventTimer();
}