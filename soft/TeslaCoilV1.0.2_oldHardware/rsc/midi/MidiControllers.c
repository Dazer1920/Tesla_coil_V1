#include "MidiControllers.h"

extern uint8_t Volume;
extern uint16_t portamentoTime;
extern boolean portamentoType;

extern Timer Timers[NUMBER_VOICES];
extern MidiNote notes[NUMBER_NOTES];

uint16_t AttackVal = 0, DecayVal = 0, SustainVal = 100, ReleaseVal = 0;

void processPortamento(Timer *timer) {  
  if(timer->periodArr != timer->period && timer->countTime > 0) {
    if(timer->periodArr > timer->period) {
      timer->periodArr -= timer->countTime;
        
      if(timer->periodArr <= timer->period) {
        timer->periodArr = timer->period;
      }
        
    } else 
    if(timer->periodArr < timer->period) {
      timer->periodArr += timer->countTime;
      
      if(timer->periodArr >= timer->period) {
        timer->periodArr = timer->period;
      }
    }
  }
}

void processADSR(Timer *timer) {
  if(timer->statusAdsr == ATTACK) {
    if(timer->countPulse >= timer->pulse) {
      if(DecayVal > 0) {
        timer->countPulse = timer->pulse;
        
        timer->pulseSustain = (float) (timer->pulse * SustainVal) / 100;
        timer->countPulseUint = (((float) timer->pulse) - timer->pulseSustain) / ((float) DecayVal);
        timer->statusAdsr = DECAY;
      } else {
        timer->pulseSustain = (float) (timer->pulse * SustainVal) / 100;
        timer->countPulse = timer->pulseSustain;
        timer->statusAdsr = SUSTAIN;
      }
      
    } else timer->countPulse +=  timer->countPulseUint;
  } else 
  if(timer->statusAdsr == DECAY) {
    if((timer->countPulse - timer->pulseSustain) <= timer->countPulseUint) {
      timer->countPulse = timer->pulseSustain;
      timer->statusAdsr = SUSTAIN;
    } else timer->countPulse -=  timer->countPulseUint;
  } else 
  if(timer->statusAdsr == RELEASE) {
    if(timer->countPulse <= timer->countPulseUint) {
      timer->statusAdsr = ADSR_END;
      timer->enable = false;
      timer->countPulse = 0.0f;
    } else timer->countPulse -=  timer->countPulseUint;
  }
}

void processMidiControllers() {
  for(uint8_t i = 0; i < NUMBER_VOICES; i++) {
    if(Timers[i].enable == true) {
      processPortamento(&Timers[i]);
      processADSR(&Timers[i]);
    }
  }
}