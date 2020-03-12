#ifndef _UTILS_H_
#define _UTILS_H_

// helpers to improve readability for register manipulations
// idea from here: https://maker.pro/custom/tutorial/using-gpio-on-stm8-microcontrollers
#define set_bit(register, bit) (register |= (1 << bit))
#define get_bit(register, bit) (register & (1 << bit))
#define clr_bit(register, bit) (register &= ~(1 << bit))  // clear bit
#define tgl_bit(register, bit) (register ^= (1 << bit))   // toggle bit

#endif // _UTILS_H_
