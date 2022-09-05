/*
  tc5timerinterrupt example

  3 LEDs/pins a switched on and off by 2 interrupts with frequencies relatively prime.
  Each interrupt handles their own LED/pin. And both handle the third LED/pin.

  Author: Daniel Mohr
  Date: 2022-09-05
*/

#include <fast_samd21_tc.h>

#define LED_PIN1 0
#define LED_PIN2 1
#define LED_PIN3 2

void TC3_Handler(void) {
  digitalWrite(LED_PIN1, !digitalRead(LED_PIN1));
  digitalWrite(LED_PIN3, !digitalRead(LED_PIN3));
  TC3->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}

void TC5_Handler(void) {
  digitalWrite(LED_PIN2, !digitalRead(LED_PIN2));
  digitalWrite(LED_PIN3, !digitalRead(LED_PIN3));
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}

void setup() {
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  tc3timerinterrupt_configure(101000); // starts the timer/trigger with 0.101 s
  tc5timerinterrupt_configure(100000); // starts the timer/trigger with 0.1 s
}

void loop() {
}