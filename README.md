# tc5timerinterrupt -- TC5 timer library for SAMD21 (e. g. Arduino MKRZERO)

This library allows using the TC5_Handler routine triggered by
the TC5 timer on SAMD21 (e. g. Arduino MKRZERO).

It is possible to trigger very fast (a few microsends, e. g. 8 us = 8e-6 s).

## Tested

It was tested on SAMD21 (Arduino MKRZERO).

## Usage

You have to provide the TC5_Handler routine, e. g.:

```
void TC5_Handler(void) {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
}
```

To set up the trigger you only need to configure and to start, e. g.:

```
void setup() {
  tc5timerinterrupt_configure(8); // 8 us = 8e-6 s
  tc5timerinterrupt_start();
}
```

You can stop, start and reconfigure the trigger, e. g.:

```
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

```
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

## Examples

In the folder `examples` you can find some examples.
