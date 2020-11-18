#ifndef _PLAYER_H
#define _PLAYER_H

#include "System.h"
#include "Midi.h"
#include "Serial.h"

#define TX_MIDI_PLAYER_KEY 64

#define TX_SET_BEGIN_PLAYER 15
#define TX_SET_RESET_PLAYER 33
#define TX_LOAD_HALF_BUFFER_PLAYER 23
#define TX_START_PLAYER 43

#define DEFAULT_BPM 120.0f

#define SIZE_PLAYER_BUFF 146 // ~1kB

void OffAllTimers();

void CalculateSystemTick();

void setStartPlayer();
void setStopPlayer();
void setResetPlayer();
void setEndPlaing();

#endif // _PLAYER_H