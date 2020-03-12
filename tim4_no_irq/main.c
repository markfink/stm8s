#include <stdint.h>
#include <stm8s.h>


// Default system clock will be 2MHz.
// We set a 128  prescaler, so tick will be 64 micro appart
// We want to generate a overflow interrupt every second using the timer
// this happens when the counter reaches 15625 (1S/64uS)
const uint16_t reload_value = 15625;

#define LED_PIN 5


void timer_isr() __interrupt(TIM4_ISR) {
  PB_ODR ^= (1 << LED_PIN); // toggle output
  TIM4_SR &= ~(1 << TIM4_SR_UIF); // clear interrupt flag
}



//  Setup the system clock to run at 16MHz using the internal oscillator.
void initialize_system_clock()
{
  CLK_ICKR = 0;                       //  Reset the Internal Clock Register.
  CLK_ICKR |= (1 << CLK_ICKR_HSIEN);  //  Enable the HSI.
  CLK_ECKR = 0;                       //  Disable the external clock.
  while((CLK_ICKR & CLK_ICKR_HSIRDY) == 0 );  //  Wait for the HSI to become ready
  CLK_CKDIVR = 0;                     //  Ensure the clocks are running at full speed.
  //CLK_PCKENR1 = 0xff;                 //  Enable all peripheral clocks.
  //CLK_PCKENR2 = 0xff;                 //  Ditto.
  CLK_CCOR = 0;                       //  Turn off CCO.
  CLK_HSITRIMR = 0;                   //  Turn off any HSIU trimming.
  CLK_SWIMCCR = 0;                    //  Set SWIM to run at clock / 2.
  CLK_SWR = 0xe1;                     //  Use HSI as the clock source.
  CLK_SWCR = 0;                       //  Reset the clock switch control register.
  CLK_SWCR |= (1 << CLK_SWCR_SWEN);   //  Enable switching.
  while((CLK_SWCR & CLK_SWCR_SWBSY) != 0 );  //  Pause until the clock switch is complete.
}


void main() {
  //initialize_system_clock();
  enable_interrupts();

  PB_DDR |= (1 << LED_PIN);
  PB_CR1 |= (1 << LED_PIN); //  push-pull

  //TIM4_PSCR = 0b00000111; //  Prescaler = 128  - 16ms
  TIM4_PSCR = 0b00000000; //  Prescaler = 128  - 16ms
  // set auto reload register
  // We need to put MSB and LSB in separate 8 bit registers
  // Also, we have to put value in ARRH first
  TIM4_ARR = 100;

  TIM4_IER |= (1 << TIM4_IER_UIE); // Enable Update Interrupt
  TIM4_CR1 |= (1 << TIM4_CR1_CEN); // Enable TIM2

  while (1) {
    // do nothing
  }
}


/*
// works fine with LED_PIN 5
int main() {
  // Default clock is HSI/8 = 2MHz

  PB_DDR |= (1 << LED_PIN);
  PB_CR1 |= (1 << LED_PIN); // pushpull

  TIM4_PSCR = 0b00000111; //  Prescaler = 128  - 16ms
  //TIM4_PSCR = 0b00000000; //  Prescaler = 1
  // Generate an update event so prescaler value will be loaded
  TIM4_EGR |= (1 << TIM4_EGR_UG);
  TIM4_CR1 |= (1 << TIM4_CR1_CEN); // Enable TIM4

  while (1) {
    if ( TIM4_CNTR == 255) {
      // Reset counter to 0
      TIM4_CNTR = 0;

      // toggle output
      PB_ODR ^= (1 << LED_PIN);
    }
  }
}
*/
