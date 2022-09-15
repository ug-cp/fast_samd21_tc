---
author: Daniel Mohr
date: 2022-09-15
license: BSD 3-Clause License
---

# fast_samd21_tc -- TC5 or TC3 timer library for SAMD21 microcontrollers

[[_TOC_]]

This library allows using the TC3_Handler, TC4_Handler and the TC5_Handler
routine triggered by the Timer/Counter TC3, TC4 or TC5 on SAMD21
(e. g. Arduino MKRZERO).

It is possible to trigger very fast (a few microseconds, e. g. 4 us or less).

Keep in mind, TC4 and TC5 are not completely independent.

* home: [gitlab.com/ug-cp/fast_samd21_tc](https://gitlab.com/ug-cp/fast_samd21_tc)
* mirror: [github.com/ug-cp/fast_samd21_tc](https://github.com/ug-cp/fast_samd21_tc)

* Arduino Library Manager: [arduino.cc/reference/en/libraries/fast_samd21_tc](https://www.arduino.cc/reference/en/libraries/fast_samd21_tc/)

In contrast to
[SAMD_TimerInterrupt](https://www.arduino.cc/reference/en/libraries/samd_timerinterrupt/)
or
[TimerInterrupt_Generic](https://www.arduino.cc/reference/en/libraries/timerinterrupt_generic/)
this library fast_samd21_tc does not interfere with the
[Ethernet library](https://www.arduino.cc/reference/en/libraries/ethernet/)
on an [Arduino MKR Zero](https://docs.arduino.cc/hardware/mkr-zero) using
[MKR ETH Shield](https://docs.arduino.cc/hardware/mkr-eth-shield).

## Install

To use/install this library, you can open the Library Manager in the
Arduino IDE and install it from there. This is the recommended way.

Or if you use [Arduino CLI](https://arduino.github.io/arduino-cli/) the
recommended way is `arduino-cli lib install fast_samd21_tc`

Or to install this library use `Add .ZIP Library ...` in your Arduino IDE
to add the zip archive `fast_samd21_tc-main.zip` you can download from
this repository. Or you can use archives from
[latest release](https://gitlab.com/ug-cp/fast_samd21_tc/-/releases/permalink/latest).

Or just copy the content of this repository to your Arduino libraries, e. g.:

```shell
git clone https://gitlab.com/ug-cp/fast_samd21_tc.git ~/Arduino/libraries/fast_samd21_tc
```

## Usage

You can include the whole library `#include <fast_samd21_tc.h>` and choose
what you use.

By defining `fast_samd21_no_tc3` you can skip this part of the library.
Defining `fast_samd21_no_tc4` or `fast_samd21_no_tc5` skips the
corresponding part.

Or you can include the TC3 specific part `#include <fast_samd21_tc3.h>`,
the TC4 specific part `#include <fast_samd21_tc5.h>` or
the TC5 specific part `#include <fast_samd21_tc5.h>`.

For example instead of `#include <fast_samd21_tc5.h>` you could also use:

```c
#define fast_samd21_no_tc3
#define fast_samd21_no_tc4
#include <fast_samd21_tc.h>
```

We will use here only the TC5 specific part `#include <fast_samd21_tc5.h>`.
For TC3, TC4 or all look at the [examples](examples).

You have to provide the TC5_Handler routine, e. g.:

```c
void TC5_Handler(void) {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}
```

To set up the trigger you only need to configure and to start, e. g.:

```c
#include <fast_samd21_tc5.h>
void setup() {
  fast_samd21_tc5_configure(8); // 8 us = 8e-6 s
  fast_samd21_tc5_start();
}
```

You can stop, start, disable and reconfigure the trigger, e. g.:

```c
void loop() {
  fast_samd21_tc5_stop();
  delay(1000);
  fast_samd21_tc5_configure(16);
  fast_samd21_tc5_start();
  delay(1000);
  fast_samd21_tc5_disable();
  delay(1000);
  fast_samd21_tc5_start();
  delay(1000);
  fast_samd21_tc5_configure(32);
  delay(1000);
}
```

The TC5_Handler routine can call different functions. Since a function call
needs some time, you have to implement this on your own.
But here is an example:

```c
#include <fast_samd21_tc5.h>
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
  fast_samd21_tc5_configure(1000000); // 1 s
  fast_samd21_tc5_start();
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

For faster switching the used `digitalWrite()` and `digitalRead()` are too
slow. To achieve faster switching we can use the registers directly. This
is done in the following measurements.

Fast switching a pin (using
[fast_blink_led_tc3.ino](examples/fast_blink_led_tc3/fast_blink_led_tc3.ino))
and measure the period T with an oscilloscope gives something like
(values in 1 us = 1e-6 s):

| set interval | expected T | mean T | min. T | max. T | std T |
| ------ | ------ | ------ | ------ | ------ | ------ |
| 1 | 2 | 2.69 | 2.66 | 2.83 | 0.054 |
| 2 | 4 | 4.68 | 4.65 | 4.85 | 0.060 |
| 4 | 8 | 8.67 | 8.65 | 9.02 | 0.097 |
| 8 | 16 | 16.66 | 16.66 | 16.57 | 0.003 |
| 16 | 32 | 32.63 | 32.01 | 32.91 | 0.165 |
| 32 | 64 | 64.64 | 64.46 | 64.66 | 0.014 |
| 64 | 128 | 128.64 | 128.60 | 128.70 | 0.023 |
| 128 | 256 | 256.6 | 256.55 | 256.65 | 0.222 |
| 256 | 512 | 512.45 | 512.45 | 512.63 | 0.033 |

Fast switching a pin (using
[fast_blink_led_tc5.ino](examples/fast_blink_led_tc5/fast_blink_led_tc5.ino))
and measure the period T with an oscilloscope gives something like
(values in 1 us = 1e-6 s):

| set interval | expected T | mean T | min. T | max. T | std T |
| ------ | ------ | ------ | ------ | ------ | ------ |
| 1 | 2 | 4.1 | 4.07 | 4.27 | 0.052 |
| 2 | 4 | 4.05 | 4.03 | 4.26 | 0.054 |
| 4 | 8 | 8.06 | 8.03 | 8.34 | 0.084 |
| 8 | 16 | 16.03 | 16.03 | 16.04 | 0.015 |
| 16 | 32 | 32.03 | 32.02 | 32.03 | 0.003 |
| 32 | 64 | 64.0 | 63.98 | 64.01 | 0.008 |
| 64 | 128 | 127.98 | 127.89 | 128.02 | 0.021 |
| 128 | 256 | 255.93 | 255.87 | 255.93 | 0.032 |
| 256 | 512 | 511.89 | 511.74 | 511.91 | 0.047 |

## Examples

In the folder [examples](examples) you can find some examples.

## License, Copyright: BSD 3-Clause License

Copyright (c) 2022, Daniel Mohr and
                    University of Greifswald, Institute of Physics,
                    Colloidal (dusty) plasma group

All rights reserved.

see: [LICENSE](LICENSE)
