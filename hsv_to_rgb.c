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
   //we lose some precision here, can we keep it somehow?
   uint16_t chroma = ((uint16_t)hsv_in[1] * (uint16_t)hsv_in[2]) >> 8;

   //x = chroma * (1 - abs((h_prime % 2) - 1));
   //approximation in fixed point
   uint16_t h_prime = ((((uint16_t)hsv_in[0]) << 8) / 85) << 1;
   uint16_t tmp = h_prime % (((uint16_t)2) << 8);
   const uint16_t one = (1 << 8);

   //abs(tmp - one)
   if (tmp >= one)
      tmp -= one;
   else
      tmp = one - tmp;

   //x = chroma * (1 - abs(h_prime % 2 - 1))
   tmp = one - tmp;
   uint16_t x = (chroma * tmp) >> 8;

   //if hue is defined
   if (hsv_in[1] != 0) {
      int16_t m;
      uint16_t rgb1[3] = {0, 0, 0};
      switch(h_prime >> 8) {
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

      for (uint8_t i = 0; i < 3; i++) {
         rgb_out[i] = rgb1[i] + m;
      }
   } else {
      for (uint8_t i = 0; i < 3; i++) {
         rgb_out[i] = hsv_in[2];
      }
   }
}

