---
title: tc5timerinterrupt -- TC5 timer library for SAMD21 (e. g. Arduino MKR Zero)
author: Daniel Mohr
date: 2022-09-02
license: BSD 3-Clause License
---

[[_TOC_]]

# tc5timerinterrupt -- TC5 timer library for SAMD21 (e. g. Arduino MKR Zero)

This library allows using the TC5_Handler routine triggered by
the TC5 timer on SAMD21 (e. g. Arduino MKR Zero).

It is possible to trigger very fast (a few microsends, e. g. 8 us = 8e-6 s).

In contrast to
[SAMD_TimerInterrupt](https://www.arduino.cc/reference/en/libraries/samd_timerinterrupt/)
or
[TimerInterrupt_Generic](https://www.arduino.cc/reference/en/libraries/timerinterrupt_generic/)
this library tc5timerinterrupt does not interfere with the
[Ethernet library](https://www.arduino.cc/reference/en/libraries/ethernet/)
on an [Arduino MKR Zero](https://docs.arduino.cc/hardware/mkr-zero) using
[MKR ETH Shield](https://docs.arduino.cc/hardware/mkr-eth-shield).

## Install

To install this library just copy the content to your arduino libraries, e. g.:

```shell
unzip -d ~/Arduino/libraries/ ~/Downloads/tc5timerinterrupt-master.zip
mv ~/Arduino/libraries/tc5timerinterrupt-master ~/Arduino/libraries/tc5timerinterrupt
```

## Usage

You have to provide the TC5_Handler routine, e. g.:

```c
void TC5_Handler(void) {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}
```

To set up the trigger you only need to configure and to start, e. g.:

```c
void setup() {
  tc5timerinterrupt_configure(8); // 8 us = 8e-6 s
  tc5timerinterrupt_start();
}
```

You can stop, start and reconfigure the trigger, e. g.:

```c
void loop() {
  tc5timerinterrupt_stop();
  delay(1000);
  tc5timerinterrupt_configure(16);
  tc5timerinterrupt_start();
  delay(1000);
  tc5timerinterrupt_disable(16);
  delay(1000);
  tc5timerinterrupt_start();
  delay(1000);
}
```

The TC5_Handler routine can call different functions. Since a function call
needs some time, you have to implement this on your own.
But here is an example:

```c
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

It was tested on SAMD21 (Arduino MKR Zero).
But should work on other Arduino MKR or SAMD21 boards as well.

Fast switching a pin and measure the period T with an oscilloscope gives
something like (values in 1 us = 1e-6 s):

| set interval | expected T | mean T | min. T | max. T | std T | 
| ------ | ------ | ------ | ------ | ------ | ------ |
| 4 | 8 | 16.18 | 16.06 | 16.74 | 0.219 |
| 8 | 16 | 16.16 | 16.10 | 16.20 | 0.039 |
| 16 | 32 | 32.02 | 31.96 | 32.08 | 0.009 |
| 32 | 64 | 64.02 | 64.00 | 64.04 | 0.005 |
| 64 | 128 | 127.98 | 127.97 | 128.00 | 0.004 |
| 128 | 256 | 255.95 | 255.95 | 255.95 | 0.0001 |
| 256 | 512 | 511.91 | 511.82 | 511.91 | 0.023 |

## Examples

In the folder [examples](examples) you can find some examples.

## License, Copyright: BSD 3-Clause License

Copyright (c) 2022, Daniel Mohr and University of Greifswald, Institute of Physics, Colloidal (dusty) plasma group

All rights reserved.

see: [LICENSE](LICENSE)
