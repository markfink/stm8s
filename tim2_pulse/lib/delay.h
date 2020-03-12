#ifndef DELAY_H
#define DELAY_H

/*
inline void delay_ms(uint32_t ms) {
    //for (uint32_t i = 0; i < ((F_CPU / 18 / 1000UL) * ms); i++) {
    for (uint32_t i = 0; i < ((F_CPU / 12000UL) * ms); i++) {
        __asm__("nop");
    }
}

inline void delay_us(uint32_t us) {
    for (uint32_t i = 0; i < ((F_CPU / 18) * us); i++) {
        __asm__("nop");
    }
}
*/

#endif /* DELAY_H */
