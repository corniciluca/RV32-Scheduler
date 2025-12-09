#include "common.h"
#define UART0_BASE 0x10000000

void uart_putc(char c) {
    volatile uint8_t *uart = (uint8_t *)UART0_BASE;
    *uart = c;
}

void uart_wr(char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

char *strcpy(char *dst, const char *src) {
    char *ret = dst;
    while ((*dst++ = *src++));
    return ret;
}