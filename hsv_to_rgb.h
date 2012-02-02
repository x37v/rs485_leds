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

#ifndef HSV_TO_RGB_H
#define HSV_TO_RGB_H

#include <inttypes.h>

void hsv_to_rgb(uint8_t rgb_out[3], uint8_t hsv_in[3]);

#endif
