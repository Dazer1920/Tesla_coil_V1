#include "System.h"
#include "Serial.h"
#include "BluetoothConfig.h"
#include "Led.h"
#include "Protection.h"
#include "Timers.h"
#include "ProcessInterrupterData.h"
#include "Player.h"
#include "MidiControllers.h"
#include "ProcessMidiEvents.h"

extern boolean isError;

int main() {
  initClock();
  initGPIO();
  initSystemTimer();
  initUsart();
  initTimers();
  initTimerPlayer();
  initProtection();
  
  __enable_irq();
  
  while(isError == false) {
    processBluetooth();
    processProtection();
    processPlayer();
    processTxFrames();

    if(TIM3->SR & TIM_SR_UIF) {
      TIM3->SR &= ~TIM_SR_UIF;
      
      processBluetoothEventTimer();
      processLed();
      processModulater();
      processMidiControllers();
      processRequestsPlayer();
      processMidiNotes();
    }
  }
  
  SystemError();
}
