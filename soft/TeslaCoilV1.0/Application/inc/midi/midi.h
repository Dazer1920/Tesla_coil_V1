#ifndef _MIDI_H
#define _MIDI_H

#include "System.h"
#include "Player.h"

#define MIDI_NOTE_ON 0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CONTROLLER 0xb0
#define MIDI_PITCH_BEND 0xe0
#define MIDI_METTA_EVENT 0xf0

#define METTA_EVENT_END_TRACK 5
#define METTA_EVENT_SET_TEMPO 3

void processMidiEvents(uint8_t *event);

void processMettaEvents(uint8_t *event);

void EnfOfTrackHandler();
void SetTempoHandler(uint8_t *event);

void NoteOnHandler(uint8_t *event);
void NoteOffHandler(uint8_t *event);
void ControllerHandler(uint8_t *event);
void PitchBendHandler(uint8_t *event);

void offAllNotes();

#endif // _MIDI_H