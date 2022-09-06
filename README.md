---
title: fast_samd21_tc -- TC5 or TC3 timer library for SAMD21 microcontrollers
author: Daniel Mohr
date: 2022-09-06
license: BSD 3-Clause License
home: [gitlab.com/ug-cp/fast_samd21_tc](https://gitlab.com/ug-cp/fast_samd21_tc)
mirror: [github.com/ug-cp/fast_samd21_tc](https://github.com/ug-cp/fast_samd21_tc)
---

[[_TOC_]]

# fast_samd21_tc -- TC5 or TC3 timer library for SAMD21 microcontrollers

This library allows using the TC5_Handler or the TC3_Handler routine
triggered by the Timer/Counter TC5 or the Timer/Counter TC3 on SAMD21
(e. g. Arduino MKRZERO). The SAMD21 is also called SAM D21.

It is possible to trigger very fast (a few microseconds, e. g. 4 us = 4e-6 s).

home: [gitlab.com/ug-cp/fast_samd21_tc](https://gitlab.com/ug-cp/fast_samd21_tc)
mirror: [github.com/ug-cp/fast_samd21_tc](https://github.com/ug-cp/fast_samd21_tc)

In contrast to
[SAMD_TimerInterrupt](https://www.arduino.cc/reference/en/libraries/samd_timerinterrupt/)
or
[TimerInterrupt_Generic](https://www.arduino.cc/reference/en/libraries/timerinterrupt_generic/)
this library fast_samd21_tc does not interfere with the
[Ethernet library](https://www.arduino.cc/reference/en/libraries/ethernet/)
on an [Arduino MKR Zero](https://docs.arduino.cc/hardware/mkr-zero) using
[MKR ETH Shield](https://docs.arduino.cc/hardware/mkr-eth-shield).

## Install

To install this library use `Add .ZIP Library ...` in your Arduino library
to add the zip archive `fast_samd21_tc-master.zip` you can download from
this repository.


Or just copy the content of this repository to your Arduino libraries, e. g.:

```shell
unzip -d ~/Arduino/libraries/ ~/Downloads/fast_samd21_tc-master.zip
mv ~/Arduino/libraries/fast_samd21_tc-master ~/Arduino/libraries/fast_samd21_tc
```

## Usage

You can include the whole library `#include <fast_samd21_tc.h>` and choose
what you use.
Or you can include the TC5 specific part `#include <tc5timerinterrupt.h>` or
the TC3 specific part `#include <tc3timerinterrupt.h>`.

We will use here only the TC5 specific part `#include <tc5timerinterrupt.h>`.
For TC3 or all look at the [examples](examples).

You have to provide the TC5_Handler routine, e. g.:

```c
void TC5_Handler(void) {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}
```

To set up the trigger you only need to configure and to start, e. g.:

```c
#include <tc5timerinterrupt.h>
void setup() {
  tc5timerinterrupt_configure(8); // 8 us = 8e-6 s
  tc5timerinterrupt_start();
}
```

You can stop, start, disable and reconfigure the trigger, e. g.:

```c
void loop() {
  tc5timerinterrupt_stop();
  delay(1000);
  tc5timerinterrupt_configure(16);
  tc5timerinterrupt_start();
  delay(1000);
  tc5timerinterrupt_disable();
  delay(1000);
  tc5timerinterrupt_start();
  delay(1000);
  tc5timerinterrupt_configure(32);
  delay(1000);
}
```

The TC5_Handler routine can call different functions. Since a function call
needs some time, you have to implement this on your own.
But here is an example:

```c
#include <tc5timerinterrupt.h>
void blink1(void) {
  digitalWrite(1, !digitalRead(1));
}
void blink2(void) {
  digitalWrite(2, !digitalRead(2));
}
void (* callback)() = blink1;
void TC5_Handler(void) {
  callback();
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}
void setup() {
  tc5timerinterrupt_configure(1000000); // 1 s
  tc5timerinterrupt_start();
}
void loop() {
  callback = blink1;
  delay(1000);
  callback = blink2;
  delay(1000);
}
```

## Tested

It was tested on SAMD21 (Arduino MKR1000, Arduino MKR Zero and
Arduino MKR WiFi 1010).
But should work on other Arduino MKR or SAMD21 boards as well.

Fast switching a pin
(using [blink_led_tc5.ino](examples/blink_led_tc5/blink_led_tc5.ino))
and measure the period T with an oscilloscope gives something like
(values in 1 us = 1e-6 s):

| set interval | expected T | mean T | min. T | max. T | std T | 
| ------ | ------ | ------ | ------ | ------ | ------ |
| 2 | 4 | 8.12 | 8.07 | 8.14 | 0.024 |
| 4 | 8 | 8.1 | 8.03 | 8.11 | 0.032 |
| 8 | 16 | 16.07 | 16.03 | 16.15 | 0.049 |
| 16 | 32 | 32.02 | 31.97 | 32.17 | 0.011 |
| 32 | 64 | 64.02 | 64.00 | 64.04 | 0.035 |
| 64 | 128 | 128.01 | 127.35 | 128.06 | 0.090 |
| 128 | 256 | 256.05 | 255.95 | 256.05 | 0.0023 |
| 256 | 512 | 511.9 | 511.80 | 512.00 | 0.144 |

So, we can really fast blink with a period of 8 us = 8e-6 s (125 kHz).
This is an interrupt every 4 us = 4e-6 s (250 kHz).

Fast switching a pin
(using [blink_led_tc3.ino](examples/blink_led_tc3/blink_led_tc3.ino))
and measure the period T with an oscilloscope gives something like
(values in 1 us = 1e-6 s):

| set interval | expected T | mean T | min. T | max. T | std T | 
| ------ | ------ | ------ | ------ | ------ | ------ |
| 4 | 8 | 8.73 | 8.66 | 8.78 | 0.032 |
| 8 | 16 | 16.69 | 16.66 | 16.91 | 0.063 |
| 16 | 32 | 32.65 | 32.65 | 32.79 | 0.015 |
| 32 | 64 | 64.67 | 64.65 | 64.67 | 0.013 |
| 64 | 128 | 128.65 | 128.64 | 128.68 | 0.008 |
| 128 | 256 | 256.52 | 256.45 | 256.55 | 0.043 |
| 256 | 512 | 512.64 | 512.50 | 512.70 | 0.153 |

Sorry, I do not know why this measurement is not as good as for TC5.

## Examples

In the folder [examples](examples) you can find some examples.

## License, Copyright: BSD 3-Clause License

Copyright (c) 2022, Daniel Mohr and University of Greifswald, Institute of Physics, Colloidal (dusty) plasma group

All rights reserved.

see: [LICENSE](LICENSE)
