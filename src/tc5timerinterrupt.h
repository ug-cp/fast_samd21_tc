/*
  Author: Daniel Mohr
  Date: 2022-09-01

  This header file allows using the TC5_Handler routine triggered by
  the TC5 timer on SAMD21 (e. g. Arduino MKRZERO).

  You have to provide the TC5_Handler routine, e. g.:

  void TC5_Handler(void) {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
  }

  To set up the trigger you only need to configure and to start, e. g.:

  void setup() {
  tc5timerinterrupt_configure(8); // 8 us = 8e-6 s
  tc5timerinterrupt_start();
  }

  You can stop, start and reconfigure the trigger, e. g.:

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

  The TC5_Handler routine can call different functions. Since a function call
  needs some time, you have to implement this on your own.
  But here is an example:

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
*/

static inline uint32_t calculate_compare_register(uint32_t us,
    uint16_t prescaler) {
  return (uint32_t) (((double) us) * 1e-6 *
                     (((double) SystemCoreClock) / ((double) prescaler)));
}

static void tc5timerinterrupt_reset() {
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
  while (TC5->COUNT16.CTRLA.bit.SWRST);
}

/*
  Starts the timer.
*/
void tc5timerinterrupt_start() {
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
}

/*
  Pauses the timer.
*/
void tc5timerinterrupt_disable() {
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
}

/*
  Stops the timer. You should start with tc5timerinterrupt_configure again.
*/
void tc5timerinterrupt_stop() {
  tc5timerinterrupt_disable();
  tc5timerinterrupt_reset();
}

/*
  Configures the timer interrupt.

  The parameter us is the time in 1e-6 s (= 1 us).

  us has to be smaller than 1398090: 0 << us <= 1398090

  You can just reconfigure by call this routine again.

  Returns an error code:

  0: no error
  1: us == 0 and this is too small
  2: us > 1398090 and this is too large
  3: no combination of prescaler and compare register value found
*/
uint8_t tc5timerinterrupt_configure(uint32_t us) {
  if (us == 0)
    return 1;
  if (us > 1398090)
    return 2;
  // find prescaler and compare register value
  // try TC_CTRLA_PRESCALER_DIV1
  uint16_t prescaler = 1;
  uint32_t compare_register = calculate_compare_register(us, prescaler);
  if (compare_register > UINT16_MAX) {
    // try TC_CTRLA_PRESCALER_DIV2
    prescaler = 2;
    compare_register = calculate_compare_register(us, prescaler);
    if (compare_register > UINT16_MAX) {
      // try TC_CTRLA_PRESCALER_DIV4
      prescaler = 4;
      compare_register = calculate_compare_register(us, prescaler);
      if (compare_register > UINT16_MAX) {
        // try TC_CTRLA_PRESCALER_DIV8
        prescaler = 8;
        compare_register = calculate_compare_register(us, prescaler);
        if (compare_register > UINT16_MAX) {
          // try TC_CTRLA_PRESCALER_DIV16
          prescaler = 16;
          compare_register = calculate_compare_register(us, prescaler);
          if (compare_register > UINT16_MAX) {
            // try TC_CTRLA_PRESCALER_DIV64
            prescaler = 64;
            compare_register = calculate_compare_register(us, prescaler);
            if (compare_register > UINT16_MAX) {
              // try TC_CTRLA_PRESCALER_DIV256
              prescaler = 256;
              compare_register = calculate_compare_register(us, prescaler);
              if (compare_register > UINT16_MAX) {
                // try TC_CTRLA_PRESCALER_DIV1024
                prescaler = 1024;
                compare_register = calculate_compare_register(us, prescaler);
              }
            }
          }
        }
      }
    }
  }
  if (compare_register > UINT16_MAX)
    return 3;

  // select clock generator TC5
  GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN |
                                  GCLK_CLKCTRL_GEN_GCLK0 |
                                  GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;
  while (GCLK->STATUS.bit.SYNCBUSY);

  tc5timerinterrupt_reset();

  // set 16 bit mode and set waveform 'match frequency'
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16 | TC_CTRLA_WAVEGEN_MFRQ;

  // prescaler
  switch (prescaler) {
    case 1:
      TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;
      break;
    case 2:
      TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV2 | TC_CTRLA_ENABLE;
      break;
    case 4:
      TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV4 | TC_CTRLA_ENABLE;
      break;
    case 8:
      TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV8 | TC_CTRLA_ENABLE;
      break;
    case 16:
      TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV16 | TC_CTRLA_ENABLE;
      break;
    case 64:
      TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV64 | TC_CTRLA_ENABLE;
      break;
    case 256:
      TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV256 | TC_CTRLA_ENABLE;
      break;
    case 1024:
      TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024 | TC_CTRLA_ENABLE;
      break;
  }

  // set compare register value
  TC5->COUNT16.CC[0].reg = (uint16_t) compare_register;

  while (TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);

  NVIC_DisableIRQ(TC5_IRQn);
  NVIC_ClearPendingIRQ(TC5_IRQn);
  NVIC_SetPriority(TC5_IRQn, 0);
  NVIC_EnableIRQ(TC5_IRQn);

  // enable interrupt
  TC5->COUNT16.INTENSET.bit.MC0 = 1;
  while (TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);

  return 0;
}
