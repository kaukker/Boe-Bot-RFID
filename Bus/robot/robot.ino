#include "Movement.h"
#include "Rfid.h"
#include "Nrf.h"
#include "RobotMode.h"
#include "Protocol.h"
#include "EEPROM_Settings.h"
#include <avr/io.h>
#include <avr/wdt.h>

uint8_t resetCause __attribute__((section(".noinit")));

void getResetCause(void) __attribute__((naked)) __attribute__((section(".init3")));
void getResetCause(void)
{
  resetCause = MCUSR;
  MCUSR = 0;
  wdt_disable();
}

static bool ButtonPressed()
{
  static bool lastState = LOW;

  bool currentState = digitalRead(BUTTON_PIN);

  if (currentState == HIGH && lastState == LOW)
  {
    lastState = currentState;
    return true;
  }

  lastState = currentState;
  return false;
}

void setupWDT() 
{
  cli();                 
  wdt_reset();           

  WDTCSR |= (1 << WDCE) | (1 << WDE);

  WDTCSR = (1 << WDIE) | (1 << WDP2) | (1 << WDP1)| (1 << WDP0);

  sei();                 
}

ISR(WDT_vect) 
{
  RobotSendEvent(EV_FAULT, 0);
  sendFault(FAULT_INTERNAL);
  resetArduino();
}

void resetArduino()
{
  wdt_disable();            
  wdt_enable(WDTO_15MS); 
  while (1) {}           
}

void setup()
{
  SetUpDataEEPROM();
  MovementSetup();
  RFIDSetup();
  RobotModeInit();
  RobotNrfSetup();
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  if (resetCause & (1 << EXTRF) | (1 << WDRF))
  {
    RobotSendEvent(EV_RESET, 0);
  }

  setupWDT();
}

void loop()
{
  wdt_reset();

  UpdateMotor();
  RobotNrfUpdate();  
  RobotModeUpdate();
  if (ButtonPressed())
  {
    RobotSendEvent(EV_BUTTON_PRESSED, 0);
    RobotModeSet(MODE_RUN);
  }
}