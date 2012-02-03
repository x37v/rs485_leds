/*
 *		Copyright (c) 2012 Alex Norman.  All rights reserved.
 *
 *		This file is part of rs485_leds
 *		
 *		rs485_leds is free software: you can redistribute it and/or modify it
 *		under the terms of the GNU General Public License as published by the
 *		Free Software Foundation, either version 3 of the License, or (at your
 *		option) any later version.
 *		
 *		rs485_leds is distributed in the hope that it will be useful, but
 *		WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *		Public License for more details.
 *		
 *		You should have received a copy of the GNU General Public License along
 *		with rs485_leds.  If not, see <http://www.gnu.org/licenses/>.
 */

//used info from http://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV 31 Jan 2012

#include "hsv_to_rgb.h"
#include <stdlib.h>
#include <stdio.h>

#define HSV_DIV_FACTOR 43

void hsv_to_rgb(uint8_t rgb_out[3], uint8_t hsv_in[3]) {
   uint8_t i = 0;
   uint16_t chroma = (uint16_t)hsv_in[1] * (uint16_t)hsv_in[2];
   uint16_t x;
   int16_t m;
   uint16_t rgb1[3] = {0, 0, 0};

   chroma = chroma >> 8;

   //x = chroma * (1 - abs((h_prime % 2) - 1));

   float tmp;
   tmp = (float)hsv_in[0] / 42.5;
   uint8_t h_prime_index = tmp;
   //h_prime mod 2
   while(tmp > 2)
      tmp -= 2;
   //minus 1
   tmp -= 1.0;
   //abs
   if (tmp < 0.0)
      tmp = -tmp;
   x = (uint16_t)((float)chroma * (1.0 - tmp));

   //if hue is defined
   if (hsv_in[1] != 0) {
      switch(h_prime_index) {
         case 0:
            rgb1[0] = chroma;
            rgb1[1] = x;
            rgb1[2] = 0;
            break;
         case 1:
            rgb1[0] = x;
            rgb1[1] = chroma;
            rgb1[2] = 0;
            break;
         case 2:
            rgb1[0] = 0;
            rgb1[1] = chroma;
            rgb1[2] = x;
            break;
         case 3:
            rgb1[0] = 0;
            rgb1[1] = x;
            rgb1[2] = chroma;
            break;
         case 4:
            rgb1[0] = x;
            rgb1[1] = 0;
            rgb1[2] = chroma;
            break;
         default:
         case 5:
            rgb1[0] = chroma;
            rgb1[1] = 0;
            rgb1[2] = x;
            break;
      }
      m = hsv_in[2] - chroma;

      //printf("x c m %d %d %d\n", x, chroma, m);

      for (i = 0; i < 3; i++) {
         rgb_out[i] = rgb1[i] + m;
      }
   } else {
      for (i = 0; i < 3; i++) {
         rgb_out[i] = hsv_in[2];
      }
   }
}

