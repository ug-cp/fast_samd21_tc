/*
  tc5timerinterrupt example

  This example shows how it works together with the Ethernet library.

  Author: Daniel Mohr
  Date: 2022-09-02
*/

#include <SPI.h>
#include <Ethernet.h>
#include <tc5timerinterrupt.h>

/* serial interface */
#define SERIAL_BAUD_RATE 115200 // baud = bits per second
#define SERIAL_TIMEOUT 1000 // timeout in 1e-6 seconds

#define LED_PIN 0

void TC5_Handler(void) {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}

void setup() {
  // set serial communication:
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.setTimeout(SERIAL_TIMEOUT);
  pinMode(LED_PIN, OUTPUT);
  tc5timerinterrupt_configure(10); // starts the timer/trigger with 10e-6 s
}

void loop() {
  auto link = Ethernet.linkStatus();
  Serial.print("Ethernet link status: ");
  switch (link) {
    case Unknown:
      tc5timerinterrupt_disable();
      Serial.println("Unknown");
      break;
    case LinkON:
      tc5timerinterrupt_configure(50000);
      Serial.println("ON");
      break;
    case LinkOFF:
      tc5timerinterrupt_configure(1000000);
      Serial.println("OFF");
      break;
  }
  delay(1000);
}
