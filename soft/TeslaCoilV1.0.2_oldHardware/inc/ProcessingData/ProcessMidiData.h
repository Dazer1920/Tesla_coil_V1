#ifndef _PROCESS_MIDI_DATA_H
#define _PROCESS_MIDI_DATA_H

#include "System.h"
#include "Timers.h"
#include "Player.h"
#include "Serial.h"
#include "ProcessMidiEvents.h"
#include "Util.h"
#include "MidiControllers.h"

#define RX_MIDI_PLAYER_KEY 0x01
#define RX_MIDI_PLAYER_MSK (0x80 | (RX_MIDI_PLAYER_KEY << 5))

#define RX_SET_START_PLAYER RX_MIDI_PLAYER_MSK | 0x01
#define RX_SET_STOP_PLAYER RX_MIDI_PLAYER_MSK | 0x02
#define RX_SET_RESET_PLAYER RX_MIDI_PLAYER_MSK | 0x03
#define RX_SET_CURRENT_TICK RX_MIDI_PLAYER_MSK | 0x04
#define RX_SET_TEMPO_PLAYER RX_MIDI_PLAYER_MSK | 0x05
#define RX_SET_RESOLUTION_PLAYER RX_MIDI_PLAYER_MSK | 0x06
#define RX_SET_MIDI_EVENT_PLAYER RX_MIDI_PLAYER_MSK | 0x07
#define RX_SET_MIDI_EVENT_TICK_PLAYER RX_MIDI_PLAYER_MSK | 0x08
#define RX_SET_MAX_TICKS_PLAYER RX_MIDI_PLAYER_MSK | 0x09
#define RX_SET_BEGIN_PLAYER RX_MIDI_PLAYER_MSK | 0x0a
#define RX_SET_VOLUME RX_MIDI_PLAYER_MSK | 0x0b
#define RX_SET_USE_VELOCITY RX_MIDI_PLAYER_MSK | 0x0c
#define RX_SET_PORTAMENTO_TIME RX_MIDI_PLAYER_MSK | 0x0d
#define RX_SET_PORTAMENTO_TYPE RX_MIDI_PLAYER_MSK | 0x0e
#define RX_MAX_SIZE RX_MIDI_PLAYER_MSK | 0x0f
#define RX_SET_ATTACK RX_MIDI_PLAYER_MSK | 0x10
#define RX_SET_DECAY RX_MIDI_PLAYER_MSK | 0x11
#define RX_SET_SUSTAIN RX_MIDI_PLAYER_MSK | 0x12
#define RX_SET_RELEASE RX_MIDI_PLAYER_MSK | 0x13


#define TX_MIDI_PLAYER_KEY 0x01
#define TX_MIDI_PLAYER_MSK (0x80 | (TX_MIDI_PLAYER_KEY << 5))

#define TX_SET_BEGIN_PLAYER TX_MIDI_PLAYER_MSK | 0x01
#define TX_SET_RESET_PLAYER TX_MIDI_PLAYER_MSK | 0x02
#define TX_LOAD_HALF_BUFFER_PLAYER TX_MIDI_PLAYER_MSK | 0x03
#define TX_START_PLAYER TX_MIDI_PLAYER_MSK | 0x04
#define TX_LOST_PACKEGE_PLAYER TX_MIDI_PLAYER_MSK | 0x05

void setStartMidiPlayer();
void setStopMidiPlayer();
void resetMidiPlayer();

void setCurrentTick(uint8_t *buff);
void setTempo(uint8_t *buff);
void setResolution(uint8_t *buff);
void setMidiEvent(uint8_t *buff);
void setMidiEventTick(uint8_t *buff);
void setMaxTick(uint8_t *buff);
void setVolume(uint8_t *buff);
void setUseVelocity(uint8_t *buff);
void setPortamentoTime(uint8_t *buff);
void setPortamentoType(uint8_t *buff);
void setEndRequest();
void setAttack(uint8_t *buff);
void setDecay(uint8_t *buff);
void setSustain(uint8_t *buff);
void setRelease(uint8_t *buff);

#endif // _PROCESS_MIDI_DATA_H