/*
 *
 * Copyright (c) 2011 Paul Stoffregen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>

// PB4 OC1B = DIM pin (high = LED power on, low = LED power off)
// PB3 OC1A = Red disable
// PB2 OC0A = Green disable
// PD5 OC0B = Blue disable
// PD4      = RS485 TXEN

const uint8_t my_addr = 3;

void init(void);
uint8_t uart_getchar(void);
void uart_putchar(uint8_t c);

uint8_t buf[4];
uint8_t count=0;

void got_char(uint8_t n)
{
	uint8_t r, g, b, max;

	if ((count & 1) == 0) {
		buf[count >> 1] = (n << 4);
	} else {
		buf[count >> 1] |= n;
	}
	count++;
	if (count >= 8) {
		if (buf[0] == my_addr) {
			r = buf[1];
			g = buf[2];
			b = buf[3];
			max = 0;
			if (r > max) max = r;
			if (g > max) max = g;
			if (b > max) max = b;
			r ^= 0xFF;
			g ^= 0xFF;
			b ^= 0xFF;
			max ^= 0xFF;
			if (max != 0xFF) {
				OCR0B = r;
				OCR0A = g;
				OCR1AL = b;
				OCR1BL = max;
			} else {
				OCR0B = 0;
				OCR0A = 0;
				OCR1AL = 0;
				OCR1BL = max;
			}
		}
		count = 0;
	}
}

int main(void)
{
	uint8_t c;

	init();

	while (1) {
		c = uart_getchar();
		if (c >= '0' && c <= '9') {
			got_char(c - '0');
		} else if (c >= 'A' && c <= 'F') {
			got_char(c - 'A' + 10);
		} else if (c >= 'a' && c <= 'f') {
			got_char(c - 'a' + 10);
		} else {
			count = 0;
		}
	}
}

uint8_t uart_getchar(void)
{
	while (!(UCSRA & (1<<RXC))) ;
	return UDR;
}

void uart_putchar(uint8_t c)
{
	UCSRA = 0;
	UDR = c;
	while (!(UCSRA & (1<<TXC))) ;
}

void init(void)
{
	PORTB = 0x00;
	DDRB  = 0xFF;
	PORTD = 0x03;
	DDRD  = 0xFE;
	// timer0 off
	TCCR0A = 0;
	TCCR0B = 0;
	TCNT0 = 0;
	// timer1 off
	TCCR1A = 0;
	TCCR1B = 0; 
	TCCR1C = 0; 
	TCNT1L = 0;
	OCR0A = 0;	// green	invert
	OCR0B = 0;	// red		invert
	OCR1AL = 0;	// blue		invert
	OCR1BL = 0xFF;	// all		invert
	// timer0 and timer1 on, 8 bit PWM mode
	TCCR0A = 0xA3;
	TCCR1A = 0xB1;
	TCCR0B = 0x03;
	TCCR1B = 0x0B;
	// uart
	UBRRH = 0;
	UBRRL = 3;
	UCSRA = 0;
	UCSRB = 0x18;
	UCSRC = 0x06;
}

