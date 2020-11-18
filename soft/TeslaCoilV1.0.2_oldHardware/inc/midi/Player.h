#ifndef _PLAYER_H
#define _PLAYER_H

#include "System.h"
#include "Timers.h"
#include "Serial.h"
#include "ProcessMidiData.h"
#include "ProcessMidiEvents.h"

#define DEFAULT_BPM 120.0f

#define SIZE_MIDI_EVENTS_BUFF 146

#define ENABLE_TIMER_PLAYER TIM1->DIER |= TIM_DIER_UIE
#define DISABLE_TIMER_PLAYER TIM1->DIER &= ~TIM_DIER_UIE

void initTimerPlayer();
void PlayerBegen();
void calculateTimeTick();
void trackEnd();

void processPlayer();

void resetTimerRequestsPlayer();
void setRequestBuffPlayer();
void processRequestsPlayer();

#endif // _PLAYER_H