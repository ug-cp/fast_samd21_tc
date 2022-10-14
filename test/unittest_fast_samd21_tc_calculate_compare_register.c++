// https://github.com/Arduino-CI/arduino_ci
#include <ArduinoUnitTests.h>

#include <Arduino.h>

#ifndef SystemCoreClock
#if defined(ARDUINO_ARCH_SAMD)
// arduino_ci does not provide platform specific information
// therefore we have to define it here on our own
#define SystemCoreClock 48000000
#endif
#endif

#include <fast_samd21_tc_calculate_compare_register.h>

unittest(fast_samd21_tc_calculate_compare_register) {
  assertEqual((uint32_t) 1,
	      _fast_samd21_tc_calculate_compare_register(0.021, 1));
  assertEqual((uint32_t) 4,
	      _fast_samd21_tc_calculate_compare_register(0.1, 1));
  assertEqual((uint32_t) 48,
	      _fast_samd21_tc_calculate_compare_register(1, 1));
  assertEqual((uint32_t) 479,
	      _fast_samd21_tc_calculate_compare_register(10, 1));
  assertEqual((uint32_t) 144,
	      _fast_samd21_tc_calculate_compare_register(6, 2));
  assertEqual((uint32_t) 184,
	      _fast_samd21_tc_calculate_compare_register(23, 6));
  uint16_t prescaler;
  uint32_t compare_register;
  assertEqual((uint8_t) 0,
	      fast_samd21_tc_calculate_compare_register(6,
							&prescaler,
							&compare_register));
  assertEqual(1, prescaler);
  assertEqual(288, compare_register);
  assertEqual((uint8_t) 0,
	      fast_samd21_tc_calculate_compare_register(23,
							&prescaler,
							&compare_register));
  assertEqual(1, prescaler);
  assertEqual(1104, compare_register);
  assertEqual((uint8_t) 0,
	      fast_samd21_tc_calculate_compare_register(2000,
							&prescaler,
							&compare_register));
  assertEqual(2, prescaler);
  assertEqual(48000, compare_register);
  assertEqual((uint8_t) 0,
	      fast_samd21_tc_calculate_compare_register(4000,
							&prescaler,
							&compare_register));
  assertEqual(4, prescaler);
  assertEqual(48000, compare_register);
  assertEqual((uint8_t) 0,
	      fast_samd21_tc_calculate_compare_register(10000,
							&prescaler,
							&compare_register));
  assertEqual(8, prescaler);
  assertEqual(60000, compare_register);
  assertEqual((uint8_t) 0,
	      fast_samd21_tc_calculate_compare_register(100000UL,
							&prescaler,
							&compare_register));
  assertEqual(256, prescaler);
  assertEqual(18750, compare_register);
  assertEqual((uint8_t) 0,
	      fast_samd21_tc_calculate_compare_register(1398090UL,
							&prescaler,
							&compare_register));
  assertEqual(1024, prescaler);
  assertEqual(65535UL, compare_register);
  assertEqual((uint8_t) 3,
	      fast_samd21_tc_calculate_compare_register(1398102UL,
							&prescaler,
							&compare_register));
}

unittest_main()
