#include "Player.h"

uint16_t Resolution;
float Tempo = DEFAULT_BPM;

uint32_t MidiEventTicks[SIZE_PLAYER_BUFF];
uint8_t MidiEvents[SIZE_PLAYER_BUFF][3];

uint32_t countTicks = 0;
uint32_t MaxTicksFile = 0;
uint8_t countProcesedMidiEvents = 0;
uint8_t countBuffMidiEvents = 0;

void OffAllTimers() {
  HAL_TIM_PWM_Stop(&htim14, TIM_CHANNEL_1);
  offAllNotes();
}

void PlayerBegin() {
  HAL_TIM_Base_Stop_IT(&htim1);
  OffAllTimers();
  
  Tempo = DEFAULT_BPM;
  countTicks = 0;
  MaxTicksFile = 0;
  countBuffMidiEvents = 0;
  countProcesedMidiEvents = 0;
  
  for(uint8_t i = 0; i < SIZE_PLAYER_BUFF; i++) {
    MidiEventTicks[i] = 0;
    
    MidiEvents[i][0] = 0;
    MidiEvents[i][1] = 0;
    MidiEvents[i][2] = 0;
  }
  
  uint8_t frame[] = {TX_MIDI_PLAYER_KEY, TX_SET_BEGIN_PLAYER};
  SerialWriteFrame(frame, 2);
}

void CalculateSystemTick() {
  uint32_t delta = (uint32_t)(60000000.0f / (Tempo * ((float) Resolution)));
  htim1.Instance->ARR = delta - 1;
}

void setStartPlayer() {
  HAL_TIM_Base_Start_IT(&htim1);
}

void setStopPlayer() {
  HAL_TIM_Base_Stop_IT(&htim1);
  OffAllTimers();
}

void setResetPlayer() {
  HAL_TIM_Base_Stop_IT(&htim1);
  PlayerBegin();
}

void setEndPlaing() {
  HAL_TIM_Base_Stop_IT(&htim1);
  OffAllTimers();
  
  uint8_t frame[] = {TX_MIDI_PLAYER_KEY, TX_SET_RESET_PLAYER};
  SerialWriteFrame(frame, 2);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if(htim->Instance == htim1.Instance) {
    if(countTicks < MaxTicksFile) {
      for(uint8_t i = 0; i < SIZE_PLAYER_BUFF; i++) {
        if(MidiEventTicks[i] == countTicks) {
          processMidiEvents(MidiEvents[i]);
          
          if(countProcesedMidiEvents == (SIZE_PLAYER_BUFF / 2)) {
            countBuffMidiEvents = 0;
            
            uint8_t frame[] = {TX_MIDI_PLAYER_KEY, TX_LOAD_HALF_BUFFER_PLAYER};
            SerialWriteFrame(frame, 2);
          } else 
          if(countProcesedMidiEvents == SIZE_PLAYER_BUFF) {
            countProcesedMidiEvents = 0;
            countBuffMidiEvents = SIZE_PLAYER_BUFF / 2;
            
            uint8_t frame[] = {TX_MIDI_PLAYER_KEY, TX_LOAD_HALF_BUFFER_PLAYER};
            SerialWriteFrame(frame, 2);
          }
          
          countProcesedMidiEvents++;
          
          break;
        }
      }
      
      countTicks++;
    } else {
      setEndPlaing();
    }
  }
}