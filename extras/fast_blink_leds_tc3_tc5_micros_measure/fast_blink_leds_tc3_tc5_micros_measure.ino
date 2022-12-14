/*
  fast_samd21_tc5 example

  2 LEDs/pins a switched on and off by 2 interrupts.
  Each interrupt handles their own LED/pin.
  Further, using micros() we try to get a measurement.

  Author: Daniel Mohr
  Date: 2022-09-19
*/

#include <fast_samd21_tc.h>

#define LED_PIN0 0
#define LED_PIN1 1

#define REGTYPE uint32_t
volatile REGTYPE *trigger_output_register0 =
  portOutputRegister(digitalPinToPort(LED_PIN0));;
const REGTYPE trigger_pin_mask0 = digitalPinToBitMask(LED_PIN0);
const REGTYPE not_trigger_pin_mask0 = ~trigger_pin_mask0;

volatile REGTYPE *trigger_output_register1 =
  portOutputRegister(digitalPinToPort(LED_PIN1));;
const REGTYPE trigger_pin_mask1 = digitalPinToBitMask(LED_PIN1);
const REGTYPE not_trigger_pin_mask1 = ~trigger_pin_mask1;

volatile unsigned long duration[] = {0, 0};

void TC3_Handler(void) {
  static uint16_t it = 0;
  static unsigned long start_microstime = 0;
  static unsigned long stop_microstime = 0;
  *trigger_output_register0 =
    (*trigger_output_register0 & not_trigger_pin_mask0) |
    ~(*trigger_output_register0 & trigger_pin_mask0);
  if (it++ == 0) {
    stop_microstime = micros();
    duration[0] = stop_microstime - start_microstime;
    start_microstime = stop_microstime;
  }
  TC3->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}

void TC5_Handler(void) {
  static uint16_t it = 0;
  static unsigned long start_microstime = 0;
  static unsigned long stop_microstime = 0;
  *trigger_output_register1 =
    (*trigger_output_register1 & not_trigger_pin_mask1) |
    ~(*trigger_output_register1 & trigger_pin_mask1);
  if (it++ == 0) {
    stop_microstime = micros();
    duration[1] = stop_microstime - start_microstime;
    start_microstime = stop_microstime;
  }
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}

void setup() {
  pinMode(LED_PIN0, OUTPUT);
  pinMode(LED_PIN1, OUTPUT);
  fast_samd21_tc3_configure(7); // starts the timer/trigger with 7 us
  fast_samd21_tc5_configure(7); // starts the timer/trigger with 7 us
}

void loop() {
  while (duration[0] == 0) {
    delay(1);
  };
  Serial.print("duration0: ");
  Serial.print(((double) duration[0]) / 65536UL);
  Serial.print(" us; ");
  duration[0] = 0;
  while (duration[1] == 0) {
    delay(1);
  };
  Serial.print("duration1: ");
  Serial.print(((double) duration[1]) / 65536UL);
  Serial.println(" us");
  duration[1] = 0;
}
