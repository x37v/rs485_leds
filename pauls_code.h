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

#ifndef PAULS_CODE_H
#define PAULS_CODE_H

#include <inttypes.h>

// PB4 OC1B = DIM pin (high = LED power on, low = LED power off)
// PB3 OC1A = Red disable
// PB2 OC0A = Green disable
// PD5 OC0B = Blue disable
// PD4      = RS485 TXEN

//setup the uart [communicating with the rs485 chip] and the pwm led driving
void init_pwm_and_uart(void);

//gets a character from the uart [rs485 input]
uint8_t uart_getchar(void);

//draw rgb values [0..255] via PWM
void draw_rgb(uint8_t r, uint8_t g, uint8_t b);

//grabs data from the uart, converts from ascii to numeric representation: ie "a0" -> 0xa0
//and fills size bytes in buf with that data, drops anything that is not in 0..9 A..F or a..f
void ascii_uart_to_uint_buffer(uint8_t * buf, uint8_t size);

#endif
