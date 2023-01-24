#include "ProcessMidiData.h"

extern uint16_t Resolution;
extern float Tempo;

extern uint32_t MidiEventTicks[SIZE_MIDI_EVENTS_BUFF];
extern uint8_t MidiEvents[SIZE_MIDI_EVENTS_BUFF][3];

extern uint32_t countTicks;
extern uint32_t MaxTicksFile;
extern uint8_t countProcesedMidiEvents;
extern uint8_t countBuffMidiEvents;

extern uint16_t AttackVal, DecayVal, SustainVal, ReleaseVal;

void setStartMidiPlayer() {
  ResetAllTimers();
  offAllNotes();
  ENABLE_TIMER_PLAYER;
}

void setStopMidiPlayer() {
  DISABLE_TIMER_PLAYER;
  ResetAllTimers();
  offAllNotes();
}

void resetMidiPlayer() {
  DISABLE_TIMER_PLAYER;
  ResetAllTimers();
  offAllNotes();
  initControllers();
  
  PlayerBegen();
}

void setCurrentTick(uint8_t *buff) {
  DISABLE_TIMER_PLAYER;
  ResetAllTimers();
  
  countTicks = (buff[1] << 14) | (buff[2] << 7) | buff[3];
  countProcesedMidiEvents = 0;
  countBuffMidiEvents = 0;
  
  uint8_t frame[] = {TX_START_PLAYER, 0, 0, 0};
  TransmitFrame(frame);
}

void setTempo(uint8_t *buff) {
  uint16_t val = (buff[1] << 7) | buff[2];
  Tempo = ((float) val) / 10.0f;
  calculateTimeTick();
}

void setResolution(uint8_t *buff) {
  Resolution = (buff[1] << 7) | buff[2];
  calculateTimeTick();
}

void setMidiEvent(uint8_t *buff) {
  resetTimerRequestsPlayer();
  MidiEvents[countBuffMidiEvents][0] = buff[1] + 0x80;
  MidiEvents[countBuffMidiEvents][1] = buff[2];
  MidiEvents[countBuffMidiEvents][2] = buff[3];
}

void setMidiEventTick(uint8_t *buff) {
  MidiEventTicks[countBuffMidiEvents] = (buff[1] << 14) | (buff[2] << 7) | buff[3];
  countBuffMidiEvents = (countBuffMidiEvents < SIZE_MIDI_EVENTS_BUFF - 1)? countBuffMidiEvents + 1 : countBuffMidiEvents;
}

void setMaxTick(uint8_t *buff) {
  MaxTicksFile = (buff[1] << 14) | (buff[2] << 7) | buff[3];
}

void setVolume(uint8_t *buff) {
  extern uint8_t Volume;
  Volume = buff[1];
}

void setUseVelocity(uint8_t *buff) {
  extern bool isUseVelocity;
  isUseVelocity = (buff[1] > 0)? true : false;
}

void setPortamentoTime(uint8_t *buff) {
  extern uint16_t portamentoTime;
  portamentoTime = (MAX_TIME_PORTAMENTO * buff[1]) / 100;
}

void setPortamentoType(uint8_t *buff) {
  extern bool portamentoType;
  portamentoType = (buff[1] > 0)? true : false;
}

void setEndRequest() {
  resetTimerRequestsPlayer();
}

void setAttack(uint8_t *buff) {
  AttackVal = (buff[1] * 600) / 100;
}

void setDecay(uint8_t *buff) {
  DecayVal = (buff[1] * 600) / 100;
}

void setSustain(uint8_t *buff) {
  SustainVal = buff[1];
}

void setRelease(uint8_t *buff) {
  ReleaseVal = (buff[1] * 600) / 100;
}