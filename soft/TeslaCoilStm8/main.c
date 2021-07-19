#include "System.h"
#include "Led.h"
#include <Timer.h>

int main() {
  initClock();
  initGpio();
  initSystemTimer();
  initTimer();

  enableInterrupts();

  SetPulseWhith(100);
  setFreq(1000);
  EnablePulse(1);

  setStateLed(LED_BLINK);

  while(1) {

    if(TIM4->SR1 & TIM4_SR1_UIF) {
      TIM4->SR1 &= ~TIM4_SR1_UIF;

      processLed();
    }
  }
}
