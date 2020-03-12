#ifndef _DELAY_H_
#define _DELAY_H_
#include <stdint.h>
#include <stm8s.h>

// https://stackoverflow.com/questions/47429311/interrupt-stm8s-issue-with-sdcc-compiler
// need to include interrupt handler in main.
void timer_isr_handler() __interrupt(TIM4_ISR);


// precision delay functions based in TIM4
void initialize_system_clock(void);
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);


// some helpers for using the tim4 delay counter directly
void reset_tim4_counter();
uint32_t read_tim4_counter();

#endif // _DELAY_H_
