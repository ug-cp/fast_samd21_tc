/*
  Author: Daniel Mohr
  Date: 2022-09-02

  This header file allows using the TC3_Handler routine triggered by
  the TC3 timer on SAMD21 (e. g. Arduino MKRZERO).

  You have to provide the TC3_Handler routine, e. g.:

  void TC3_Handler(void) {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  TC3->COUNT16.INTFLAG.bit.MC0 = 1; // clears the interrupt
  }


Fast switching a pin (using [blink_led.ino](examples/blink_led/blink_led.ino))
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

*/

static inline uint32_t calculate_compare_register(uint32_t us,
    uint16_t prescaler) {
  return (uint32_t) (((double) us) * 1e-6 *
                     (((double) SystemCoreClock) / ((double) prescaler)));
}

static void tc3timerinterrupt_reset() {
  TC3->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
  while (TC3->COUNT16.CTRLA.bit.SWRST);
}

/*
  Starts the timer.
*/
void tc3timerinterrupt_start() {
  TC3->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
}

/*
  Pauses the timer.
*/
void tc3timerinterrupt_disable() {
  TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);
}

/*
  Stops the timer. You should start with tc3timerinterrupt_configure again.
*/
void tc3timerinterrupt_stop() {
  tc3timerinterrupt_disable();
  tc3timerinterrupt_reset();
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
uint8_t tc3timerinterrupt_configure(uint32_t us) {
  if (us == 0)
    return 1;
  if (us > 1398090)
    return 2;
  // find prescaler and compare register value
  // try TC_CTRLA_PRESCALER_DIV1
  uint16_t prescaler = 16;
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

  // select clock generator TC3
  GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN |
                                  GCLK_CLKCTRL_GEN_GCLK0 |
                                  GCLK_CLKCTRL_ID(GCM_TCC2_TC3)) ;
  while (GCLK->STATUS.bit.SYNCBUSY);

  tc3timerinterrupt_reset();

  // set 16 bit mode and set waveform 'match frequency'
  TC3->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16 | TC_CTRLA_WAVEGEN_MFRQ;

  // prescaler
  switch (prescaler) {
    case 1:
      TC3->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;
      break;
    case 2:
      TC3->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV2 | TC_CTRLA_ENABLE;
      break;
    case 4:
      TC3->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV4 | TC_CTRLA_ENABLE;
      break;
    case 8:
      TC3->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV8 | TC_CTRLA_ENABLE;
      break;
    case 16:
      TC3->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV16 | TC_CTRLA_ENABLE;
      break;
    case 64:
      TC3->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV64 | TC_CTRLA_ENABLE;
      break;
    case 256:
      TC3->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV256 | TC_CTRLA_ENABLE;
      break;
    case 1024:
      TC3->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024 | TC_CTRLA_ENABLE;
      break;
  }

  // set compare register value
  TC3->COUNT16.CC[0].reg = (uint16_t) compare_register;

  while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);

  NVIC_DisableIRQ(TC3_IRQn);
  NVIC_ClearPendingIRQ(TC3_IRQn);
  NVIC_SetPriority(TC3_IRQn, 0);
  NVIC_EnableIRQ(TC3_IRQn);
  //return 0;

  // enable interrupt
  TC3->COUNT16.INTENSET.bit.MC0 = 1;
  while (TC3->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY);

  return 0;
}
