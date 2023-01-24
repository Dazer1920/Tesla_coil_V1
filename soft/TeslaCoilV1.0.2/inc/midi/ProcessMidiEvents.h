#ifndef _PROCESS_MIDI_EVENTS_H
#define _PROCESS_MIDI_EVENTS_H

#include "System.h"
#include "Player.h"
#include "Timers.h"
#include "Util.h"

#define NUMBER_NOTES 8
#define NUMBER_VOICES 3

#define MAX_NOTE 100
#define MIN_NOTE 20

typedef struct {
  uint8_t channel;
  uint8_t note;
  uint8_t velocity;
  bool isPlaing;
  uint32_t milis;
} MidiNote;

typedef struct {
  uint8_t volume;
  int16_t pitchBend;
} MidiController;

#define MIDI_NOTE_ON 0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CONTROLLER 0xb0
#define MIDI_PITCH_BEND 0xe0
#define MIDI_METTA_EVENT 0xf0

#define METTA_EVENT_END_TRACK 5
#define METTA_EVENT_SET_TEMPO 3

void processMidiEvents(uint8_t *event);
void processMettaEvents(uint8_t *event);

void SetTempoHandler(uint8_t *event);

void offAllNotes();
void initControllers();

void NoteOnHandler(uint8_t *event);
void NoteOffHandler(uint8_t *event);
void ControllerHandler(uint8_t *event);
void PitchBendHandler(uint8_t *event);

void processMidiNotes();

#endif // _PROCESS_MIDI_EVENTS_H