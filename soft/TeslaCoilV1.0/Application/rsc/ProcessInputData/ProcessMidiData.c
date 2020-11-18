#include "ProcessMidiData.h"

void SetCurrentTick(uint8_t *data) {
  extern uint32_t countTicks;
  extern uint8_t countProcesedMidiEvents;
  extern uint8_t countBuffMidiEvents;

  HAL_TIM_Base_Stop_IT(&htim1);
  OffAllTimers();
  
  countTicks = (data[2] << 14) | (data[3] << 7) | data[4];
  countProcesedMidiEvents = 0;
  countBuffMidiEvents = 0;
  
  uint8_t frame[] = {TX_MIDI_PLAYER_KEY, TX_START_PLAYER};
  SerialWriteFrame(frame, 2);
}

void SetTempo(uint8_t *data) {
  uint16_t val = data[2] << 7;
  val |= data[3];
  
  extern float Tempo;
  Tempo = ((float) val) / 10.0f;
  CalculateSystemTick();
}

void SetResolution(uint8_t *data) {
  extern uint16_t Resolution;
  
  Resolution = (data[2] << 7) | data[3];  
  CalculateSystemTick();
}

void SetMidiEvent(uint8_t *data) {
  extern uint8_t countBuffMidiEvents;
  extern uint32_t MidiEventTicks[SIZE_PLAYER_BUFF];
  extern uint8_t MidiEvents[SIZE_PLAYER_BUFF][3];
  
  MidiEvents[countBuffMidiEvents][0] = data[2] + 0x80;
  MidiEvents[countBuffMidiEvents][1] = data[3];
  MidiEvents[countBuffMidiEvents][2] = data[4];

  MidiEventTicks[countBuffMidiEvents] = (data[5] << 14) | (data[6] << 7) | data[7];
  
  countBuffMidiEvents = (countBuffMidiEvents < SIZE_PLAYER_BUFF - 1)? countBuffMidiEvents + 1 : countBuffMidiEvents;
}

void setMaxTicks(uint8_t *data) {
  extern uint32_t MaxTicksFile;
  MaxTicksFile = (data[2] << 14) | (data[3] << 7) | data[4];
}

void readMidiData(uint8_t *data) {
  switch(data[1]) {
  default: break;
  
  case RX_SET_START_PLAYER: setStartPlayer(); break;
  case RX_SET_STOP_PLAYER: setStopPlayer(); break;
  case RX_SET_RESET_PLAYER: setResetPlayer(); break;
  case RX_SET_CURRENT_TICK: SetCurrentTick(data); break;
  case RX_SET_TEMPO_PLAYER: SetTempo(data); break;
  case RX_SET_RESOLUTION_PLAYER: SetResolution(data); break;
  case RX_SET_MIDI_EVENT_PLAYER: SetMidiEvent(data); break;
  case RX_SET_MAX_TICKS_PLAYER: setMaxTicks(data); break;
  }
}