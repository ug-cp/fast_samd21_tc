/*
  Author: Daniel Mohr
  Date: 2022-09-19

  This header file is used by 'fast_samd21_tc3.h', 'fast_samd21_tc4.h' and 
  'fast_samd21_tc5.h'.
*/

#ifndef fast_samd21_tc_calculate_compare_register_h
#define fast_samd21_tc_calculate_compare_register_h

static inline uint32_t
fast_samd21_tc_calculate_compare_register(uint32_t us,
					  uint16_t prescaler) {
  return (uint32_t) (((double) us) * 1e-6 *
                     (((double) SystemCoreClock) / ((double) prescaler)));
}

#endif /* fast_samd21_tc_calculate_compare_register_h */
