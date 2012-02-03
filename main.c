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
#include <string.h>
#include "hsv_to_rgb.h"
#include "pauls_code.h"

#ifndef MY_ADDRESS
#define MY_ADDRESS 0
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

void draw_hsv(uint8_t hsv[3]);

//dist is 0..255 where 255 is right at target and 0 is right at init
void interp_hsv(uint8_t hsv_out[3], uint8_t hsv_init[3], uint8_t hsv_target[3], uint8_t dist);

int main(void) {
   //init our pwm and uart inputs
   init_pwm_and_uart();
   draw_rgb(0,0,16);
   time_ms = 0;
   unsigned long target_ms = 0;
   uint8_t hsv_target[3] = {0, 0, 0};
   uint8_t hsv_initial[3] = {0, 0, 0};
   uint8_t hsv[3];
   uint8_t start_packet;
   uint16_t time_mul = 0;

   while (1) {
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
         //reset time_ms
         time_ms = 0;
         
         //grab our colors, 7 bit
         for (uint8_t i = 0; i < 3; i++)
            hsv_target[i] = (uart_getchar() << 1); //shift up because we're only getting 7 bit values
         //grab our target time
         target_ms = ((uint16_t)uart_getchar() << 7) | uart_getchar();

         if (target_ms == 0) {
            //our initial is our target, copy and draw
            memcpy(hsv_initial, hsv_target, 3 * sizeof(uint8_t));
            memcpy(hsv, hsv_target, 3 * sizeof(uint8_t));
            draw_hsv(hsv);

            //XXX use different multipler depending on size of target_ms?
            time_mul = (255 << 4) / target_ms;
         } else {
            memcpy(hsv_initial, hsv, 3 * sizeof(uint8_t));
            draw_hsv(hsv_initial);
         }

      } else {
         time_ms += (start_packet & 0x1f);
         if (time_ms < target_ms) {
            //react to clock
            //XXX use different multipler depending on size of target_ms?
            uint8_t dist = (time_mul * time_ms) >> 4;
            interp_hsv(hsv, hsv_initial, hsv_target, dist);
            draw_hsv(hsv);
         } else {
            memcpy(hsv, hsv_target, 3 * sizeof(uint8_t));
            draw_hsv(hsv);
         }
      }
   }
}

void draw_hsv(uint8_t hsv[3]) {
   uint8_t rgb[3];
   hsv_to_rgb(rgb, hsv);
   draw_rgb(rgb[0], rgb[1], rgb[2]);
}

uint8_t linear_interp(uint8_t x, uint16_t y0, uint16_t y1) {
   return (y0 * (255 - x) + y1 * x) / 255;
}

void interp_hsv(uint8_t hsv_out[3], uint8_t hsv_init[3], uint8_t hsv_target[3], uint8_t dist) {
   if (dist == 0) {
      memcpy(hsv_out, hsv_init, 3 * sizeof(uint8_t));
      return;
   } else if (dist == 255) {
      memcpy(hsv_out, hsv_target, 3 * sizeof(uint8_t));
      return;
   }
   for (uint8_t i = 0; i < 3; i++)
      hsv_out[i] = linear_interp(dist, hsv_init[i], hsv_target[i]);
}
