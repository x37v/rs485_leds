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
 * This code was originally written by Paul Stoffregen, modularized by Alex Norman 2012
 */

#include "pauls_code.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

void init_pwm_and_uart(void) {
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
   OCR0A = 0;  // green  invert
   OCR0B = 0;  // red    invert
   OCR1AL = 0;  // blue    invert
   OCR1BL = 0xFF;  // all    invert
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

uint8_t uart_getchar(void) {
   while (!(UCSRA & (1<<RXC))) ;
   return UDR;
}

void draw_rgb(uint8_t r, uint8_t g, uint8_t b) {
   uint8_t max = 0;
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

void ascii_uart_to_uint_buffer(uint8_t * buf, uint8_t size) {
   uint8_t count = 0;
   while (count < (size << 1)) {
      uint8_t c = uart_getchar();
      if (c >= '0' && c <= '9') {
         c = c - '0';
      } else if (c >= 'A' && c <= 'F') {
         c = c - 'A' + 10;
      } else if (c >= 'a' && c <= 'f') {
         c = c - 'a' + 10;
      } else {
         continue;
      }
      if ((count & 1) == 0) {
         buf[count >> 1] = (c << 4);
      } else {
         buf[count >> 1] |= c;
      }
      count++;
   }
}
