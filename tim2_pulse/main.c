#include <stdint.h>
#include <stm8s.h>

#define LED_PIN 5


//  Setup the system clock to run at 16MHz using the internal oscillator.
void initialize_system_clock()
{
  CLK_ICKR = 0;                       //  Reset the Internal Clock Register.
  CLK_ICKR |= (1 << CLK_ICKR_HSIEN);  //  Enable the HSI.
  CLK_ECKR = 0;                       //  Disable the external clock.
  while((CLK_ICKR & CLK_ICKR_HSIRDY) == 0 );  //  Wait for the HSI to become ready
  CLK_CKDIVR = 0;                     //  Ensure the clocks are running at full speed.
  CLK_PCKENR1 = 0xff;                 //  Enable all peripheral clocks.
  CLK_PCKENR2 = 0xff;                 //  Ditto.
  CLK_CCOR = 0;                       //  Turn off CCO.
  CLK_HSITRIMR = 0;                   //  Turn off any HSIU trimming.
  CLK_SWIMCCR = 0;                    //  Set SWIM to run at clock / 2.
  CLK_SWR = 0xe1;                     //  Use HSI as the clock source.
  CLK_SWCR = 0;                       //  Reset the clock switch control register.
  CLK_SWCR |= (1 << CLK_SWCR_SWEN);   //  Enable switching.
  while((CLK_SWCR & CLK_SWCR_SWBSY) != 0 );  //  Pause until the clock switch is complete.
}


//  TIM2 Overflow handler.
void TIM2_UPD_OVF_IRQHandler(void) __interrupt(TIM2_OVF_ISR)
{
  // toggle the output port so we can observe the overflow interrupt
  PB_ODR ^= (1 << LED_PIN);
  //  Reset the interrupt otherwise it will fire again straight away.
  TIM2_SR1 &= ~(1 << TIM2_SR1_UIF); // Clear interrupt flag
}


//  Setup the port
void setup_output_port(void)
{
  PB_DDR |= (1 << LED_PIN); // PB5 is now output
  PB_CR1 &= ~(1 << LED_PIN); // PB5 is now open drain
}


//  Setup Timer 2 to generate a 20 Hz interrupt based upon a 16 MHz timer.
void setup_tim2(void)
{
  TIM2_PSCR = 0x03;       //  Prescaler = 8.
  TIM2_ARRH = 0xc3;       //  High byte of 50,000.
  TIM2_ARRL = 0x50;       //  Low byte of 50,000.
  TIM2_IER |= (1 << TIM2_IER_UIE); // Enable Update Interrupt
  TIM2_CR1 |= (1 << TIM2_CR1_CEN); // Enable TIM2
}


void main(void)
{
  disable_interrupts()
  initialize_system_clock();
  setup_output_port();
  setup_tim2();

  enable_interrupts()
  while (1)
  {
    // wait for interrupt
  }
}
