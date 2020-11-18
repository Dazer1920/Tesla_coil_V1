#include "Player.h"

uint16_t Resolution;
float Tempo = DEFAULT_BPM;

uint32_t MidiEventTicks[SIZE_MIDI_EVENTS_BUFF];
uint8_t MidiEvents[SIZE_MIDI_EVENTS_BUFF][3];

uint32_t countTicks = 0;
uint32_t MaxTicksFile = 0;
uint8_t countProcesedMidiEvents = 0;
uint8_t countBuffMidiEvents = 0;

void PlayerBegen() {
  Tempo = DEFAULT_BPM;
  countTicks = 0;
  MaxTicksFile = 0;
  countBuffMidiEvents = 0;
  countProcesedMidiEvents = 0;
  
  for(uint8_t i = 0; i < SIZE_MIDI_EVENTS_BUFF; i++) {
    MidiEventTicks[i] = 0;
    
    MidiEvents[i][0] = 0;
    MidiEvents[i][1] = 0;
    MidiEvents[i][2] = 0;
  }
  
  uint8_t frame[] = {TX_SET_BEGIN_PLAYER, 0, 0, 0};
  TransmitFrame(frame);
}

void initTimerPlayer() {
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
  
  TIM1->ARR = 999UL;
  TIM1->PSC = 47;
  TIM1->CR1 |= TIM_CR1_CEN;
  
  NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 3);
  NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
}

void calculateTimeTick() {
  uint32_t delta = (uint32_t)(60000000.0f / (Tempo * ((float) Resolution)));
  TIM1->ARR = delta;
}

void trackEnd() {
  DISABLE_TIMER_PLAYER;
  ResetAllTimers();
  
  uint8_t frame[] = {TX_SET_RESET_PLAYER, 0, 0, 0};
  TransmitFrame(frame);
}

uint16_t countPlayTicks = 0, countTimerRequests = 0;
boolean isRequestBuffPlayer = false;
uint8_t countRequestsPlayer = 0;

void resetTimerRequestsPlayer() {
  isRequestBuffPlayer = false;
}

void setRequestBuffPlayer() {  
  if(isRequestBuffPlayer == true) return; 
  isRequestBuffPlayer = true;
}

void processPlayer() {
  if(countPlayTicks > 0) {
    if(countTicks < MaxTicksFile) {
      for(uint8_t i = 0; i < SIZE_MIDI_EVENTS_BUFF; i++) {
        if(MidiEventTicks[i] == countTicks) {
          processMidiEvents(MidiEvents[i]);
                      
          if(countProcesedMidiEvents == (SIZE_MIDI_EVENTS_BUFF / 2)) {
            countBuffMidiEvents = 0;
            setRequestBuffPlayer();
          } else 
          if(countProcesedMidiEvents == SIZE_MIDI_EVENTS_BUFF) {
            countProcesedMidiEvents = 0;
            countBuffMidiEvents = SIZE_MIDI_EVENTS_BUFF / 2;
            setRequestBuffPlayer();
          }
          
          countProcesedMidiEvents++;
        }
      }
      
      countTicks++;
    } else {
      trackEnd();
    }
    
    countPlayTicks--;
  }
}

void TIM1_BRK_UP_TRG_COM_IRQHandler() {
  if(TIM1->SR & TIM_SR_UIF) {
    TIM1->SR &= ~TIM_SR_UIF;
    countPlayTicks++;
  }
}

void processRequestsPlayer() {
  if(isRequestBuffPlayer == true) {
    if(countTimerRequests == 0) {
      countTimerRequests = 700;
      
      if(countRequestsPlayer > 3) {
        resetTimerRequestsPlayer();
        
        trackEnd();
        uint8_t frame[] = {TX_LOST_PACKEGE_PLAYER, 0, 0, 0};
        TransmitFrame(frame);
        return;
      }
      
      countRequestsPlayer++;
      
      uint8_t frame[] = {TX_LOAD_HALF_BUFFER_PLAYER, 0, 0, 0};
      TransmitFrame(frame);
      
    } else countTimerRequests--;
  } else {
    countTimerRequests = 0;
    countRequestsPlayer = 0;
  }
}