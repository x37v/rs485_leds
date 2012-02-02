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

#include <inttypes.h>
#include "hsv_to_rgb.h"
#include "pauls_code.h"

#ifndef MY_ADDRESS
#define MY_ADDRESS 1
#endif

const uint8_t broadcast_address = 0;
const uint8_t my_addr = MY_ADDRESS;

int main(void) {
   //init our pwm and uart inputs
   init_pwm_and_uart();

   while (1) {
      uint8_t hsv[3];
      uint8_t rgb[3];
      uint8_t address;

      //loop until we get an address packet, top bit is set
      while(((address = uart_getchar()) & 0x80) == 0);

      address &= 0x7F;

      //if the first value matches our address or the broadcast address, work with it
      if (address == my_addr || address == broadcast_address) {

         //grab our colors
         for (uint8_t i = 0; i < 3; i++)
            hsv[i] = (uart_getchar() << 1); //shift up because we're only getting 7 bit values

         //convert to rgb and draw
         hsv_to_rgb(rgb, hsv);
         draw_rgb(rgb[0], rgb[1], rgb[2]);
      }
   }
}

