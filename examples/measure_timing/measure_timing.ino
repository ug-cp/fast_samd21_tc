/*
  fast_samd21_tc5 example

  This examples uses micros() to measure the timing.
  If you have a small TC5_Handler routine, you can also use a timer
  with 4 us = 4e-6 s. But micros() needs too long here.

  You can do a much more precise measurement using an oscilloscope.

  Author: Daniel Mohr
  Date: 2022-09-02
*/

#include <fast_samd21_tc5.h>

/* serial interface */
#define SERIAL_BAUD_RATE 115200 // baud = bits per second
#define SERIAL_TIMEOUT 1000 // timeout in 1e-6 seconds

#define LED_PIN 0

unsigned long microstime_old = 0;
unsigned long microstime_delta = 0;

void TC5_Handler(void) {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  static unsigned long mymicros;
  mymicros = micros();
  microstime_delta = mymicros - microstime_old;
  microstime_old = mymicros;
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}

void setup() {
  // set serial communication:
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.setTimeout(SERIAL_TIMEOUT);
  pinMode(LED_PIN, OUTPUT);
  fast_samd21_tc5_configure(100000); // starts the timer/trigger with 0.1 s
}

void loop() {
  Serial.println("\nloop starts");
  static unsigned long mtime;
  fast_samd21_tc5_configure(100000); // starts the timer/trigger with 100 ms
  Serial.println("timer runs with 100 ms");
  for (unsigned short i = 0; i < 10; i++) {
    delay(1000);
    mtime = millis();
    Serial.print(" mtime: ");
    Serial.print(mtime);
    Serial.print(" microstime_delta: ");
    Serial.println(microstime_delta);
  }
  fast_samd21_tc5_configure(10000); // starts the timer/trigger with 10 ms
  Serial.println("timer runs with 10 ms");
  for (unsigned short i = 0; i < 10; i++) {
    delay(100);
    mtime = millis();
    Serial.print(" mtime: ");
    Serial.print(mtime);
    Serial.print(" microstime_delta: ");
    Serial.println(microstime_delta);
  }
  fast_samd21_tc5_configure(1000); // starts the timer/trigger with 1 ms
  Serial.println("timer runs with 1 ms");
  for (unsigned short i = 0; i < 10; i++) {
    delay(10);
    mtime = millis();
    Serial.print(" mtime: ");
    Serial.print(mtime);
    Serial.print(" microstime_delta: ");
    Serial.println(microstime_delta);
  }
  fast_samd21_tc5_configure(100); // starts the timer/trigger with 100 us
  Serial.println("timer runs with 100 us");
  for (unsigned short i = 0; i < 10; i++) {
    delay(10);
    mtime = millis();
    Serial.print(" mtime: ");
    Serial.print(mtime);
    Serial.print(" microstime_delta: ");
    Serial.println(microstime_delta);
  }
  fast_samd21_tc5_configure(10); // starts the timer/trigger with 10 us
  Serial.println("timer runs with 10 us");
  for (unsigned short i = 0; i < 10; i++) {
    delay(10);
    mtime = millis();
    Serial.print(" mtime: ");
    Serial.print(mtime);
    Serial.print(" microstime_delta: ");
    Serial.println(microstime_delta);
  }
}
