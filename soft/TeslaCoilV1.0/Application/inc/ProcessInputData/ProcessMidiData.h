#ifndef _PROCESS_MIDI_DATA_H
#define _PROCESS_MIDI_DATA_H

#include "System.h"
#include "Player.h"
#include "Serial.h"

#define RX_MIDI_PLAYER_KEY 64

#define RX_SET_START_PLAYER 35
#define RX_SET_STOP_PLAYER 34
#define RX_SET_RESET_PLAYER 33
#define RX_SET_CURRENT_TICK 54
#define RX_SET_TEMPO_PLAYER 10
#define RX_SET_RESOLUTION_PLAYER 13
#define RX_SET_MIDI_EVENT_PLAYER 12
#define RX_SET_MAX_TICKS_PLAYER 11
#define RX_SET_BEGIN_PLAYER 15

void readMidiData(uint8_t *data);

#endif // _PROCESS_MIDI_DATA_H