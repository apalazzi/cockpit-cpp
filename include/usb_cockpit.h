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

#ifndef __USB_COCKPIT_H
#define __USB_COCKPIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include <common.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>
#include <libopencm3/usb/cdc.h>

#include <ringbuffer.h>

extern ringbuffer usb_read_buffer;

struct hid_function {
    struct usb_hid_descriptor hid_descriptor;
    struct {
        uint8_t bReportDescriptorType;
        uint16_t wDescriptorLength;
    } __attribute__((packed)) hid_report;
} __attribute__((packed));

struct cdcacm_function {
    struct usb_cdc_header_descriptor header;
    struct usb_cdc_call_management_descriptor call_mgmt;
    struct usb_cdc_acm_descriptor acm;
    struct usb_cdc_union_descriptor cdc_union;
} __attribute__((packed));

extern struct usb_device_descriptor dev_descr;
extern const struct usb_config_descriptor config;
extern const struct hid_function hf_;
extern const struct usb_endpoint_descriptor hid_endpoint;
extern const struct usb_interface_descriptor hid_iface;
extern const struct usb_interface ifaces[];
extern const char* const usb_strings[];

int hid_control_request(usbd_device *dev,
                        struct usb_setup_data *req,
                        uint8_t **buf,
                        uint16_t *len,
                        void (**complete)(usbd_device *,
                                          struct usb_setup_data *));
void hid_set_config(usbd_device *dev, uint16_t wValue);
void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue);
void tim2_setup(void);

#ifdef __cplusplus
}
# endif /* extern C */
#endif /* __USB_COCKPIT_H */
