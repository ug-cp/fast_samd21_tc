/*
  tc5timerinterrupt example

  It blinks one LED/pin for a while and then blinks another one.
  Then is pauses and starts again.

  Author: Daniel Mohr
  Date: 2022-09-02
*/

#include <tc5timerinterrupt.h>

#define LED_PIN1 0
#define LED_PIN2 1

void (* callback)();

void blink1(void) {
  digitalWrite(LED_PIN1, !digitalRead(LED_PIN1));
}

void blink2(void) {
  digitalWrite(LED_PIN2, !digitalRead(LED_PIN2));
}

void TC5_Handler(void) {
  callback();
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}

void setup() {
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  callback = blink1;
  tc5timerinterrupt_configure(100000); // starts the timer/trigger with 0.1 s
}

void loop() {
  callback = blink1;
  digitalWrite(LED_PIN2, LOW);
  delay(3000);
  callback = blink2;
  digitalWrite(LED_PIN1, LOW);
  delay(3000);
  tc5timerinterrupt_disable();
  delay(3000);
  tc5timerinterrupt_start();
  delay(3000);
}
