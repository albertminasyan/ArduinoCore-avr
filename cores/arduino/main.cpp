/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include <dbg.h>

// Declared weak in Arduino.h to allow user redefinitions.
int atexit(void (* /*func*/ )()) { return 0; }

// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

void setupUSB() __attribute__((weak));
void setupUSB() { }


uint8_t mcusr_copy __attribute__((section(".noinit")));

void early(void) __attribute__((naked, used, section(".init0")));
void early(void) {

	init_usart();
	UCSR0A = _BV(TXC0);		// reset transmit completed bit = 1

	dbg_putc('\x0a');
	dbg_putc('\x0d');
	dbg_putc('#');
	dbg_putc('#');
	dbg_putc('\x0a');
	dbg_putc('\x0d');

	dbg_flush();
}

extern int __heap_start, *__brkval;
int freeRam() { int v; return (int)&v - (__brkval ? (int)__brkval : (int)&__heap_start); }



int main(void)
{
	Serial.begin(9600);
	Serial.print("FREE RAM: "); Serial.println(freeRam());
	Serial.print("MCUSR=0x"); Serial.println(mcusr_copy, HEX);
	Serial.flush();

	DBG();

	init();

	UCSR0B = _BV(TXEN0);
	DBG();

	initVariant();

	DBG();

#if defined(USBCON)
	USBDevice.attach();
#endif
	
	setup();

	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
	}

	return 0;
}

