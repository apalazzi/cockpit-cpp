/*
    Copyright 2017 Andrea Palazzi <palazziandrea@yahoo.it>

    This file is part of ed-autopilot-cockpit.

    ed-autopilot-cockpit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ed-autopilot-cockpit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ed-autopilot-cockpit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __MAIN_H
#define __MAIN_H

#include <common.h>

#ifdef DEBUG
#include <board/stm32f4discovery.hpp>
#else
#include <board/stm32f103x.hpp>
#endif

#include <map>
#include <vector>
#include <cstdlib>
#include <string>

#include <libopencm3/cm3/common.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include "buttons.hpp"
#include "digitalIO.hpp"
#include <analogIO.hpp>

#include <handlers.h>
#include <usb_cockpit.h>
#include <ringbuffer.h>

void* __dso_handle;

typedef struct {
    int8_t x;
    int8_t y;
    int8_t z;
    int8_t elevator; // Ry in the hid descriptor

    int8_t throttle;
    int8_t rudder;

    uint8_t button[32];
} controls_t;


void setup_clock();
#ifdef DEBUG
void setup_clock_discovery();
#endif

#endif /* __MAIN_H */
