#include <stdint.h>
#include <stm8s.h>


// Default system clock will be 2MHz.
// We set a 128  prescaler, so tick will be 64 micro appart
// We want to generate a overflow interrupt every second using the timer
// this happens when the counter reaches 15625 (1S/64uS)
const uint16_t reload_value = 15625;

#define LED_PIN 5


/*
void timer_isr() __interrupt(TIM2_OVF_ISR) {
  PB_ODR ^= (1 << LED_PIN); // toggle output
  TIM2_SR1 &= ~(1 << TIM2_SR1_UIF); // clear interrupt flag
}


void main() {
  enable_interrupts();

  PB_DDR |= (1 << LED_PIN);
  PB_CR1 |= (1 << LED_PIN); //  push-pull

  TIM2_PSCR = 0b00000111; //  Prescaler = 128
  // set auto reload register
  // We need to put MSB and LSB in separate 8 bit registers
  // Also, we have to put value in ARRH first
  TIM2_ARRH = reload_value >> 8;
  TIM2_ARRL = reload_value & 0x00FF;

  TIM2_IER |= (1 << TIM2_IER_UIE); // Enable Update Interrupt
  TIM2_CR1 |= (1 << TIM2_CR1_CEN); // Enable TIM2

  while (1) {
    // do nothing
  }
}
*/


// works fine with LED_PIN 5
int main() {
  // Default clock is HSI/8 = 2MHz

  PB_DDR |= (1 << LED_PIN);
  PB_CR1 |= (1 << LED_PIN); // pushpull

  TIM2_PSCR = 0b00000111; //  Prescaler = 128
  // Generate an update event so prescaler value will be loaded
  TIM2_EGR |= (1 << TIM2_EGR_UG);
  TIM2_CR1 |= (1 << TIM2_CR1_CEN); // Enable TIM2

  while (1) {
    if ( ( ((uint16_t)TIM2_CNTRH << 8) + (uint16_t)TIM2_CNTRL ) >= 15625 ) {
      // Reset counter to 0
      TIM2_CNTRH = 0;
      TIM2_CNTRL = 0;

      // toggle output
      PB_ODR ^= (1 << LED_PIN);
    }
  }
}
