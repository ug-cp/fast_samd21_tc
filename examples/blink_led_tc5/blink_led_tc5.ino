/*
  tc5timerinterrupt example

  This is a basic example. It blinks one LED/pin.

  Author: Daniel Mohr
  Date: 2022-09-02
*/

#include <tc5timerinterrupt.h>

#define LED_PIN 0

void TC5_Handler(void) {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  tc5timerinterrupt_configure(100000); // starts the timer/trigger with 0.1 s
}

void loop() {
}
