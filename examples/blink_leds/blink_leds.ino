/*
  fast_samd21_tc5 example

  It blinks one LED/pin for a while and then blinks another one.
  Then is pauses and starts again.

  Author: Daniel Mohr
  Date: 2022-09-02
*/

#include <fast_samd21_tc5.h>

#define LED_PIN0 0
#define LED_PIN1 1

void (* callback)();

void blink1(void) {
  digitalWrite(LED_PIN0, !digitalRead(LED_PIN0));
}

void blink2(void) {
  digitalWrite(LED_PIN1, !digitalRead(LED_PIN1));
}

void TC5_Handler(void) {
  callback();
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}

void setup() {
  pinMode(LED_PIN0, OUTPUT);
  pinMode(LED_PIN1, OUTPUT);
  callback = blink1;
  fast_samd21_tc5_configure(100000); // starts the timer/trigger with 0.1 s
}

void loop() {
  callback = blink1;
  digitalWrite(LED_PIN1, LOW);
  delay(3000);
  callback = blink2;
  digitalWrite(LED_PIN0, LOW);
  delay(3000);
  fast_samd21_tc5_disable();
  delay(3000);
  fast_samd21_tc5_start();
  delay(3000);
}
