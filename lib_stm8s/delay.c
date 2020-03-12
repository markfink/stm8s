
#include <stdint.h>
#include <stm8s.h>
#include <utils.h>

uint32_t tim4_counter;  // 6h when counting seconds on 5us intervals


void timer_isr_handler() __interrupt(TIM4_ISR) {
  tim4_counter++;
  clr_bit(TIM4_SR, TIM4_SR_UIF); // clear interrupt flag
}


void tim4_setup(void)
{
  // TODO we could use different prescalers for bigger delays
  TIM4_PSCR = 0b00000000; //  Prescaler = 1
  // 16MHz * 0.5-10^-6 = 80
  // PSCR * ARR = 80
  // PSCR: 2^0 = 1
  // set auto reload register
  TIM4_ARR = 80; 

  set_bit(TIM4_IER, TIM4_IER_UIE); // Enable Update Interrupt
  set_bit(TIM4_CR1, TIM4_CR1_CEN); // Enable TIM2
}


void reset_tim4_counter(void)
{
  tim4_counter = 0;
  tim4_setup();  // 5us ~ 1 count
}


uint32_t read_tim4_counter()
{
  return tim4_counter;
}


void delay_us(uint16_t us)
{
  uint16_t count_to = us/5;
  reset_tim4_counter();
  while (tim4_counter <= count_to);
}


void delay_ms(uint16_t ms)
{
  while(ms--)
    delay_us(1000);
}


//  Setup the system clock to run at 16MHz using the internal oscillator.
void initialize_system_clock(void)
{
  CLK_ICKR = 0;                       //  Reset the Internal Clock Register.
  set_bit(CLK_ICKR, CLK_ICKR_HSIEN);  //  Enable the HSI.
  CLK_ECKR = 0;                       //  Disable the external clock.
  while(get_bit(CLK_ICKR, CLK_ICKR_HSIRDY) == 0 );  //  Wait for the HSI to become ready
  CLK_CKDIVR = 0;                     //  Ensure the clocks are running at full speed.
  CLK_PCKENR1 = 0xff;                 //  Enable all peripheral clocks.
  CLK_PCKENR2 = 0xff;                 //  Ditto.
  CLK_CCOR = 0;                       //  Turn off CCO.
  CLK_HSITRIMR = 0;                   //  Turn off any HSIU trimming.
  CLK_SWIMCCR = 0;                    //  Set SWIM to run at clock / 2.
  CLK_SWR = 0xe1;                     //  Use HSI as the clock source.
  CLK_SWCR = 0;                       //  Reset the clock switch control register.
  set_bit(CLK_SWCR, CLK_SWCR_SWEN);   //  Enable switching.
  while(get_bit(CLK_SWCR, CLK_SWCR_SWBSY) != 0 );  //  Pause until the clock switch is complete.
}
