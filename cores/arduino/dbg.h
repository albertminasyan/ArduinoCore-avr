/*
 * dbg.h
 *
 *  Created on: 22 Oct 2026
 *      Author: Albert Minasyan albert.minasyan@gmail.com
 */

#ifndef HARDWARE_ARDUINO_AVR_CORES_ARDUINO_DBG_H_
#define HARDWARE_ARDUINO_AVR_CORES_ARDUINO_DBG_H_



#ifndef DBG_H
#define DBG_H
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#define BAUD 9600UL
#define UBRR_VAL ((F_CPU / (16UL * BAUD)) - 1)

static inline void dbg_putc(char c)
{
    while (!(UCSR0A & _BV(UDRE0)));
    UDR0 = c;
}

static inline void init_usart(void)
{
    UCSR0A = 0;                                 /* normal speed, no multiprocessor */
    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t)UBRR_VAL;
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);         /* 8 data bits, no parity, 1 stop bit */
    UCSR0B = _BV(TXEN0);                        /* transmitter only, no interrupts */
}

static inline void dbg_flush(void)
{
    while (!(UCSR0A & _BV(TXC0)));
}

static inline void dbg_at(const char *file_P, uint16_t line, const char *func)
{
	// clear TXC0 (write 1), keep U2X0/MPCM0 unchanged, don't touch other flags
	UCSR0A = (UCSR0A & (_BV(U2X0) | _BV(MPCM0))) | _BV(TXC0);

	const char *base = file_P;
    char c;
    for (const char *p = file_P; (c = pgm_read_byte(p)); p++)
        if (c == '/') base = p + 1;

    while ((c = pgm_read_byte(base++))) dbg_putc(c);
    dbg_putc(':');

    char b[5]; uint8_t i = 0;
    do { b[i++] = '0' + line % 10; line /= 10; } while (line);
    while (i) dbg_putc(b[--i]);

    dbg_putc(' ');
    while (*func) dbg_putc(*func++);
    dbg_putc('\r'); dbg_putc('\n');

    // wait until the last byte has fully left the shift register
    if (UCSR0B & _BV(TXEN0))
        while (!(UCSR0A & _BV(TXC0)));
}

#define DBG() do { \
    static const char f_[] PROGMEM = __FILE__; \
    dbg_at(f_, __LINE__, __func__); } while (0)

#endif



#endif /* HARDWARE_ARDUINO_AVR_CORES_ARDUINO_DBG_H_ */
