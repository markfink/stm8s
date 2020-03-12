#include <stdint.h>
#include <stm8s.h>


// works ok!

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


uint16_t delay_counter;

void timer_isr_handler() __interrupt(TIM4_ISR) {
  if (delay_counter)
    delay_counter--;
  TIM4_SR &= ~(1 << TIM4_SR_UIF); // clear interrupt flag
}


void tim4_setup(void)
{
  TIM4_PSCR = 0b00000000; //  Prescaler = 1
  // 16MHz * 0.5-10^-6 = 80
  // PSCR * ARR = 80
  // PSCR: 2^0 = 1
  // set auto reload register
  TIM4_ARR = 80; 

  TIM4_IER |= (1 << TIM4_IER_UIE); // Enable Update Interrupt
  TIM4_CR1 |= (1 << TIM4_CR1_CEN); // Enable TIM2
}


void delay_us(uint16_t us)
{
  delay_counter = us/5;
  tim4_setup();  // 5us ~ 1 count
  while (delay_counter) {}
}


void delay_ms(signed int ms)
{
  while(ms--)
    delay_us(1000);
}


#define LED_PIN 5

void main() {
  initialize_system_clock();
  enable_interrupts();

  PB_DDR |= (1 << LED_PIN);
  //PB_CR1 |= (1 << LED_PIN); //  push-pull
  PB_CR1 &= ~(1 << LED_PIN); //  open drain

  while (1) {
    PB_ODR ^= (1 << LED_PIN); // toggle output
    delay_ms(300);
  }
}
