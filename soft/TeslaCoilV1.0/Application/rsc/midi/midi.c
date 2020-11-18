#include "Midi.h"

const uint16_t frequencyMidiNote[] = {
  8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 21, 22, 23, 24, 26, 27, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 
  58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139,147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 
  311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 830, 880, 932, 988, 1046, 1108, 1174, 1244, 1318, 
  1397, 1480, 1568, 1661, 1760, 1864, 1975, 2092, 2217, 2349, 2488, 2636, 2793, 2959, 3135, 3322, 3519, 3728, 3950, 4185, 4434, 4697, 4977, 
  5272, 5586, 5918, 6270, 6643, 7038, 7456, 7900, 8369, 8867, 9394, 9953, 10545, 11171, 11836, 12540
};

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
    
  case METTA_EVENT_END_TRACK: EnfOfTrackHandler(); break;
  case METTA_EVENT_SET_TEMPO: SetTempoHandler(event); break;
  }
}

void EnfOfTrackHandler() {
  setEndPlaing();
}

void SetTempoHandler(uint8_t *event) {
  uint16_t val = event[1] << 7;
  val |= event[2];
  
  extern float Tempo;
  Tempo = ((float) val) / 10.0f;
  CalculateSystemTick();
}

int16_t map(int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t notes[5], channels[5], velocity;
Enable PlaingNotes[5];

void startTimer() {
  htim14.Instance->ARR = (1000000 / frequencyMidiNote[notes[0]]) - 1;
  htim14.Instance->CCR1 = 70;
  HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
}

void offAllNotes() {
  for(uint8_t i = 0; i < 5; i++) {
    PlaingNotes[0] = OFF;
  }
}

void NoteOnHandler(uint8_t *event) {
  if(!event[2]) {
    NoteOffHandler(event);
    return;
  }
  
  if(PlaingNotes[0] == OFF) {
    notes[0] = event[1];
    channels[0] = (event[0] & 0x0f);
    PlaingNotes[0] = ON;
    
    startTimer();
  } else {
    for(uint8_t i = 0; i < 5; i++) {
      if(PlaingNotes[i] == OFF) {
        notes[i] = notes[0];
        channels[i] = channels[0];
        PlaingNotes[i] = ON;
        
        notes[0] = event[1];
        channels[0] = (event[0] & 0x0f);
        PlaingNotes[0] = ON;
        startTimer();
        
        break;
      }
    }
  }
  
  
}

void NoteOffHandler(uint8_t *event) {
  for(uint8_t i = 0; i < 5; i++) {
    if((event[0] & 0x0f) == channels[i] && event[1] == notes[i] && PlaingNotes[i] == ON) {
      PlaingNotes[i] = OFF;
      
      if(i == 0) {
        HAL_TIM_PWM_Stop(&htim14, TIM_CHANNEL_1);
        
        for(uint8_t j = 1; j < 5; j++) {
          if(PlaingNotes[j] == OFF) {
            notes[0] = notes[j];
            channels[0] = channels[j];
            PlaingNotes[0] = ON;
            PlaingNotes[j] = OFF;

            startTimer();
            
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
    HAL_TIM_PWM_Stop(&htim14, TIM_CHANNEL_1);
    offAllNotes();
  }
}

void PitchBendHandler(uint8_t *event) {
  
}