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
 *		You should have received a copy of the GNU General Public License auint16_t
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
//
//addressed packets
//0: address [or broadcast]
//1: command [see below]
//2: h
//3: s
//4: v
//5: timeMSB [if applicable]
//6: timeLSB [if applicable]

uint16_t time_ms = 0;

typedef enum { ACTION_NONE, ACTION_ADDRESSED, ACTION_CLOCK } action_t;
typedef enum {
   COMMAND_HSV_IMMEDIATE = 0,
   COMMAND_HSV_FROM_BLACK = 1,
   COMMAND_HSV_FROM_CURRENT = 2
} command_t;

void draw_hsv(uint8_t hsv[3]);

//dist is 0..255 where 255 is right at target and 0 is right at init
void interp_hsv(uint8_t hsv_out[3], uint8_t hsv_init[3], uint8_t hsv_target[3], uint16_t current_loc, uint16_t target_loc);

void process_command(command_t command, uint16_t *time_ms, uint16_t *target_ms, uint8_t hsv[3], uint8_t hsv_target[3], uint8_t hsv_initial[3]);

int main(void) {
   //init our pwm and uart inputs
   init_pwm_and_uart();
   draw_rgb(0,0,16);
   time_ms = 0;
   uint16_t target_ms = 0;
   uint8_t hsv_target[3] = {0, 0, 0};
   uint8_t hsv_initial[3] = {0, 0, 0};
   uint8_t hsv[3];
   uint8_t start_packet;

   while (1) {
      action_t action = ACTION_NONE;

      //loop until we get an start_packet packet, top bit is set
      while(((start_packet = uart_getchar()) & 0x80) == 0);

      //figure out what type of packet we got is
      switch (start_packet & 0xE0) {
         case 0x80: //individual light
            if ((start_packet & 0x1F) == MY_ADDRESS)
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

         command_t command = uart_getchar();
         process_command(command, &time_ms, &target_ms, hsv, hsv_target, hsv_initial);
         
      } else {
         time_ms += (start_packet & 0x1f);
         if (target_ms != 0) {
            if (time_ms < target_ms) {
               //react to clock
               interp_hsv(hsv, hsv_initial, hsv_target, time_ms, target_ms);
               draw_hsv(hsv);
            } else {
               //set it to zero so we don't draw anymore
               target_ms = 0;
               memcpy(hsv, hsv_target, 3 * sizeof(uint8_t));
               draw_hsv(hsv);
            }
         }
      }
   }
}

void grab_hsv(uint8_t hsv[3]) {
   //grab our colors, 7 bit
   for (uint8_t i = 0; i < 3; i++)
      hsv[i] = (uart_getchar() << 1); //shift up because we're only getting 7 bit values
}

void process_command(command_t command, uint16_t *time_ms, uint16_t *target_ms, uint8_t hsv[3], uint8_t hsv_target[3], uint8_t hsv_initial[3]) {
   //XXX may move with new command types
   grab_hsv(hsv_target);

   switch(command) {
      case COMMAND_HSV_FROM_CURRENT:
         //grab our target time
         *target_ms = ((uint16_t)uart_getchar() << 7) | uart_getchar();
         if (*target_ms != 0) {
            memcpy(hsv_initial, hsv, 3 * sizeof(uint8_t));
            draw_hsv(hsv_initial);
            break;
         }
         //if target_ms == 0, let it drop through
      case COMMAND_HSV_IMMEDIATE:
         *target_ms = 0;
         memcpy(hsv_initial, hsv_target, 3 * sizeof(uint8_t));
         memcpy(hsv, hsv_target, 3 * sizeof(uint8_t));
         draw_hsv(hsv);
         break;
      case COMMAND_HSV_FROM_BLACK:
         //grab our target time
         *target_ms = ((uint16_t)uart_getchar() << 7) | uart_getchar();
         //set initial hue and saturation, but value to zero
         hsv_initial[0] = hsv_target[0];
         hsv_initial[1] = hsv_target[1];
         if (*target_ms == 0)
            hsv_initial[2] = hsv_target[2];
         else
            hsv_initial[2] = 0;
         memcpy(hsv, hsv_initial, 3 * sizeof(uint8_t));
         draw_hsv(hsv);
         break;
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

void interp_hsv(uint8_t hsv_out[3], uint8_t hsv_init[3], uint8_t hsv_target[3], uint16_t current_loc, uint16_t target_loc) {
   if (current_loc == 0) {
      memcpy(hsv_out, hsv_init, 3 * sizeof(uint8_t));
      return;
   } else if (current_loc >= target_loc) {
      memcpy(hsv_out, hsv_target, 3 * sizeof(uint8_t));
      return;
   }
   //32 bit divide.. ugg, can we fix this?
   uint8_t dist = (((uint32_t)current_loc << 18) / (uint32_t)0x3fff) >> 10;
   for (uint8_t i = 0; i < 3; i++)
      hsv_out[i] = linear_interp(dist, hsv_init[i], hsv_target[i]);
}
