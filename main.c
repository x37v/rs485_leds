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

//top 3 bits:
//0b100 == address packet [individual light] [the rest of the byte contains the light address]
//0b101 == reserved [unused]
//0b110 == clock packet [all lights] [the rest of the byte contains clock offset in milliseconds]
//0b111 == broadcast packet [all lights] [the rest of the byte is ignored]

const uint8_t broadcast_address = 0;
const uint8_t my_addr = MY_ADDRESS;
unsigned long time_ms = 0;

typedef enum { ACTION_NONE, ACTION_ADDRESSED, ACTION_CLOCK } action_t;

int main(void) {
   //init our pwm and uart inputs
   init_pwm_and_uart();
   time_ms = 0;

   while (1) {
      uint8_t hsv[3];
      uint8_t rgb[3];
      uint8_t start_packet;
      action_t action = ACTION_NONE;

      //loop until we get an start_packet packet, top bit is set
      while(((start_packet = uart_getchar()) & 0x80) == 0);

      //figure out what type of packet we got is
      switch (start_packet & 0xE0) {
         case 0x80: //individual light
            if ((start_packet & 0x1F) == my_addr)
               action = ACTION_ADDRESSED;
            break;
         case 0xE0: //broadcast packet, all lights
            action = ACTION_ADDRESSED;
            break;
            break;
         case 0xC0: //clock packet, all lights
            action = ACTION_CLOCK;
            break;
         default: break;
      }

      if (action == ACTION_ADDRESSED) {
         //grab our colors, 7 bit
         for (uint8_t i = 0; i < 3; i++)
            hsv[i] = (uart_getchar() << 1); //shift up because we're only getting 7 bit values

         //convert to rgb and draw
         hsv_to_rgb(rgb, hsv);
         draw_rgb(rgb[0], rgb[1], rgb[2]);
      } else {
         time_ms += (start_packet & 0x1f);
         //XXX react to clock
      }
   }
}

