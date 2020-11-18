#include "ProcessMidiEvents.h"

MidiNote notes[NUMBER_NOTES];
MidiController MidiControllers[16];

void processMidiNotes() {
  for(uint8_t i = 0; i < NUMBER_NOTES; i++) {
    if(notes[i].isPlaing == true) {
      notes[i].milis++;
    }
  }
}

void processMidiEvents(uint8_t *event) {
  switch(event[0] & 0xf0) {
  default: break;
    
  case MIDI_NOTE_ON: NoteOnHandler(event); break;
  case MIDI_NOTE_OFF: NoteOffHandler(event); break;
  case MIDI_CONTROLLER: ControllerHandler(event); break;
  case MIDI_PITCH_BEND: PitchBendHandler(event); break;
  case MIDI_METTA_EVENT: processMettaEvents(event); break;
  }
}

void processMettaEvents(uint8_t *event) {
  switch(event[0] & 0x0f) {
  default: break;
    
  case METTA_EVENT_END_TRACK: trackEnd(); break;
  case METTA_EVENT_SET_TEMPO: SetTempoHandler(event); break;
  }
}

void SetTempoHandler(uint8_t *event) {
  uint16_t val = (event[1] << 7) | event[2];
  
  extern float Tempo;
  Tempo = ((float) val) / 10.0f;
  calculateTimeTick();
}

void initControllers() {
  for(uint8_t i = 0; i < 16; i++) {
    MidiControllers[i].pitchBend = 0;
    MidiControllers[i].volume = 127;
  }
}

void offAllNotes() {
  for(uint8_t i = 0; i < NUMBER_NOTES; i++) {
    notes[i].isPlaing = false;
  }
}

void NoteOnHandler(uint8_t *event) {
  OUT_PORT->ODR &= ~(1 << OUT_PIN);
  
  if(!event[2]) {
    NoteOffHandler(event);
    return;
  }
  
  boolean isPlaing = false;
  for(uint8_t i = 0; i < NUMBER_NOTES; i++) {
    if(notes[i].note == event[1] && notes[i].isPlaing == true) {
      isPlaing = true;
      break;
    }
    
    if(event[1] < MIN_NOTE || event[1] > MAX_NOTE) {
      isPlaing = true;
      break;
    }
  }
  if(isPlaing == true) return; 
  
  
  for(uint8_t i = 0; i < NUMBER_NOTES; i++) {
    if(notes[i].isPlaing == false) {
      notes[i].isPlaing = true;
      
      notes[i].channel = event[0] & 0x0f;
      notes[i].note = event[1];
      notes[i].velocity = event[2];
      
      if(i < NUMBER_VOICES) playNotes();
      
      break;
    }
  }
  
  for(uint8_t i = 0; i < NUMBER_NOTES; i++) {
    if(notes[i].milis >= TIME_OUT_ON_NOTE) {
      uint8_t event[] = {notes[i].channel | (0x80), notes[i].note, notes[i].velocity};
      NoteOffHandler(event);
    }
  }
}

void NoteOffHandler(uint8_t *event) {
  for(uint8_t i = 0; i < NUMBER_NOTES; i++) {
    if(notes[i].note == event[1] && notes[i].channel == (event[0] & 0x0f)) {
      notes[i].isPlaing = false;
      notes[i].milis = 0;
      
      if(i < NUMBER_VOICES) {
        playNotes();
        
        for(uint8_t j = NUMBER_VOICES; j < NUMBER_NOTES; j++) {
          if(notes[j].isPlaing == true) {
            notes[i].channel = notes[j].channel;
            notes[i].note = notes[j].note;
            notes[i].velocity = notes[j].velocity;
            notes[j].isPlaing = false;
            
            notes[i].isPlaing = true;
            playNotes();
            
            break;
          }
        }
      }
      
      break;
    }
  }
}

void ControllerHandler(uint8_t *event) {
  if(event[1] == 0x78 || event[1] == 0x79 || event[1] == 0x7B || event[1] == 0x7C) {
    ResetAllTimers();
  }
  
  if(event[1] == 0x07) {
    MidiControllers[event[0] & 0x0f].volume = event[2];
    playNotes();
  }
}

void PitchBendHandler(uint8_t *event) {
  MidiControllers[event[0] & 0x0f].pitchBend = ((event[1] & 0x7f) | ((event[2] & 0x7f) << 7)) - 8192;
  
  for(uint8_t i = 0; i < NUMBER_VOICES; i++) {
    if(notes[i].channel == (event[0] & 0x0f) && notes[i].isPlaing == true) {
      playNotes();
    }
  }
}