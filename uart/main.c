#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stm8s.h>
#include <uart.h>
#include <delay.h>


// brief:
// Mode: 8-N-1, flow-control: none.
// uart 115200 baud on following pins:
// * PD5 -> TX
// * PD6 -> RX

// use console:
// $ picocom -b 115200 --imap lfcrlf /dev/ttyUSB0 
// to exit: CTRL + A + X 


// Redirect stdout to UART
int putchar(int c) {
    uart_write(c);
    return 0;
}

// Redirect stdin to UART
int getchar() {
    return uart_read();
}

void main() {
    uint8_t counter = 0;
    enable_interrupts();  // for delays to work
    uart_init();

    while (1) {
        printf("Test, %d\n", counter++);
        delay_ms(500);
    }
}
