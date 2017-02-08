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

    Mostly borrowed from libopencm3 examples
 */

#include <usb_cockpit.h>

extern ringbuffer usb_read_buffer = {.begin=NULL, .end=NULL};

struct usb_device_descriptor dev_descr = {
        .bLength = USB_DT_DEVICE_SIZE, .bDescriptorType = USB_DT_DEVICE,
        .bcdUSB = 0x0200, .bDeviceClass = 0xEF, .bDeviceSubClass = 2,
        .bDeviceProtocol = 1, .bMaxPacketSize0 = 64, .idVendor = 0x1209,
        .idProduct = 0xc0c0, .bcdDevice = 0x0200, .iManufacturer = 1,
        .iProduct = 2, .iSerialNumber = 3, .bNumConfigurations = 1, };

const uint8_t hid_report_descriptor[] =  {
                                          0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
                                          0x15, 0x00,                    // LOGICAL_MINIMUM (0)
                                          0x09, 0x05,                    // USAGE (Gamepad)
                                          0xa1, 0x01,                    // COLLECTION (Application)
                                          0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
                                          0x09, 0x01,                    //   USAGE (Pointer)
                                          0xa1, 0x00,                    //   COLLECTION (Physical)
                                          0x09, 0x30,                    //     USAGE (X)
                                          0x09, 0x31,                    //     USAGE (Y)
                                          0x09, 0x32,                    //     USAGE (Z)
                                          0x09, 0x33,                    //     USAGE (Rx)
                                          0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
                                          0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
                                          0x75, 0x08,                    //     REPORT_SIZE (8)
                                          0x95, 0x04,                    //     REPORT_COUNT (4)
                                          0x81, 0x02,                    //     INPUT (Data,Var,Abs)
                                          0xc0,                          //   END_COLLECTION
                                          0x05, 0x02,                    //   USAGE_PAGE (Simulation Controls)
                                          0x09, 0xbb,                    //   USAGE (Throttle)
                                          0x09, 0xba,                    //   USAGE (Rudder)
                                          0x15, 0x81,                    //   LOGICAL_MINIMUM (-127)
                                          0x25, 0x7f,                    //   LOGICAL_MAXIMUM (127)
                                          0x75, 0x08,                    //   REPORT_SIZE (8)
                                          0x95, 0x02,                    //   REPORT_COUNT (3)
                                          0x81, 0x02,                    //   INPUT (Data,Var,Abs)
                                          0x05, 0x09,                    //   USAGE_PAGE (Button)
                                          0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
                                          0x29, 0x20,                    //   USAGE_MAXIMUM (Button 32)
                                          0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
                                          0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
                                          0x75, 0x01,                    //   REPORT_SIZE (1)
                                          0x95, 0x20,                    //   REPORT_COUNT (32)
                                          0x55, 0x00,                    //   UNIT_EXPONENT (0)
                                          0x65, 0x00,                    //   UNIT (None)
                                          0x81, 0x02,                    //   INPUT (Data,Var,Abs)
                                          0xc0                           // END_COLLECTION
};

const struct hid_function hf_ = {
        .hid_descriptor = { .bLength = sizeof(hf_),
                            .bDescriptorType = USB_DT_HID,
                            .bcdHID = 0x0100, .bCountryCode = 0,
                            .bNumDescriptors = 1, },
        .hid_report = {
                .bReportDescriptorType = USB_DT_REPORT,
                .wDescriptorLength = sizeof(hid_report_descriptor), } };

const struct usb_endpoint_descriptor hid_endpoint = {
        .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT, .bEndpointAddress = 0x81,
        .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
        .wMaxPacketSize = 10, .bInterval = 20, };

const struct usb_interface_descriptor hid_iface = {
        .bLength = USB_DT_INTERFACE_SIZE,
        .bDescriptorType = USB_DT_INTERFACE, .bInterfaceNumber = 2,
        .bAlternateSetting = 0, .bNumEndpoints = 1,
        .bInterfaceClass = USB_CLASS_HID, .bInterfaceSubClass = 1, /* boot */
        .bInterfaceProtocol = 0, .iInterface = 0,
        .endpoint = &hid_endpoint, .extra = &hf_,
        .extralen = sizeof(hf_), };

const struct usb_endpoint_descriptor cdcacm_data_endp[] = {
        { .bLength = USB_DT_ENDPOINT_SIZE,
          .bDescriptorType = USB_DT_ENDPOINT, .bEndpointAddress = 0x02,
          .bmAttributes = USB_ENDPOINT_ATTR_BULK, .wMaxPacketSize = 64,
          .bInterval = 1, },
        { .bLength = USB_DT_ENDPOINT_SIZE,
          .bDescriptorType = USB_DT_ENDPOINT, .bEndpointAddress = 0x83,
          .bmAttributes = USB_ENDPOINT_ATTR_BULK, .wMaxPacketSize = 64,
          .bInterval = 1, } };

const struct usb_interface_descriptor cdcacm_data_iface = {
        .bLength = USB_DT_INTERFACE_SIZE,
        .bDescriptorType = USB_DT_INTERFACE, .bInterfaceNumber = 1,
        .bAlternateSetting = 0, .bNumEndpoints = 2,
        .bInterfaceClass = USB_CLASS_DATA, .bInterfaceSubClass = 0,
        .bInterfaceProtocol = 0, .iInterface = 0,
        .endpoint = cdcacm_data_endp, };

const struct usb_endpoint_descriptor cdcacm_comm_endpoint = {
        .bLength = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType = USB_DT_ENDPOINT, .bEndpointAddress = 0x84,
        .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
        .wMaxPacketSize = 16, // FIXME
        .bInterval = 255, // FIXME
};

struct cdcacm_function cdcacm_functional_descriptors = {
        .header = {
                .bFunctionLength = sizeof(struct usb_cdc_header_descriptor),
                .bDescriptorType = CS_INTERFACE,
                .bDescriptorSubtype = USB_CDC_TYPE_HEADER,
                .bcdCDC = 0x0110, },
        .call_mgmt = {
                .bFunctionLength = sizeof(struct usb_cdc_call_management_descriptor),
                .bDescriptorType = CS_INTERFACE,
                .bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,
                .bmCapabilities = 0, .bDataInterface = 1, },
        .acm = {
                .bFunctionLength = sizeof(struct usb_cdc_acm_descriptor),
                .bDescriptorType = CS_INTERFACE,
                .bDescriptorSubtype = USB_CDC_TYPE_ACM,
                .bmCapabilities = 0, },
        .cdc_union = {
                .bFunctionLength = sizeof(struct usb_cdc_union_descriptor),
                .bDescriptorType = CS_INTERFACE,
                .bDescriptorSubtype = USB_CDC_TYPE_UNION,
                .bControlInterface = 0, .bSubordinateInterface0 = 1, } };

const struct usb_interface_descriptor cdcacm_comm_iface = {
        .bLength = USB_DT_INTERFACE_SIZE,
        .bDescriptorType = USB_DT_INTERFACE, .bInterfaceNumber = 0,
        .bAlternateSetting = 0, .bNumEndpoints = 1,
        .bInterfaceClass = USB_CLASS_CDC,
        .bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,
        .bInterfaceProtocol = USB_CDC_PROTOCOL_AT, .iInterface = 0,
        .endpoint = &cdcacm_comm_endpoint,
        .extra = &cdcacm_functional_descriptors,
        .extralen = sizeof(cdcacm_functional_descriptors) };

const struct usb_iface_assoc_descriptor iad_iface = {
        .bLength = USB_DT_INTERFACE_ASSOCIATION_SIZE,
        .bDescriptorType = USB_DT_INTERFACE_ASSOCIATION,
        .bFirstInterface = 0, .bInterfaceCount = 2,
        .bFunctionClass = USB_CLASS_CDC,
        .bFunctionSubClass = USB_CDC_SUBCLASS_ACM,
        .bFunctionProtocol = USB_CDC_PROTOCOL_AT, .iFunction = 0, // ??
};

const struct usb_interface ifaces[] = {
        { .num_altsetting = 1,
          .iface_assoc = &iad_iface,
          .altsetting = &cdcacm_comm_iface,
        }, {
         .num_altsetting = 1, // cdc acm data
         .altsetting = &cdcacm_data_iface,
        }, { .num_altsetting = 1, // hid
           .altsetting = &hid_iface, },

};

const struct usb_config_descriptor config = {
        .bLength = USB_DT_CONFIGURATION_SIZE,
        .bDescriptorType = USB_DT_CONFIGURATION, .wTotalLength = 0,
        .bNumInterfaces = 3, .bConfigurationValue = 1,
        .iConfiguration = 0, .bmAttributes = 0xC0, .bMaxPower = 0x32,
        .interface = ifaces, };

const char* const usb_strings[] = {
        "Andrea Palazzi <palazziandrea@yahoo.it>",
        "Cockpit button rack", "0001", };

int hid_control_request(
        usbd_device *dev, struct usb_setup_data *req, uint8_t **buf,
        uint16_t *len,
        void (**complete)(usbd_device *, struct usb_setup_data *))
{
    (void) complete;
    (void) dev;

    if ((req->bmRequestType != 0x81) || (req->bRequest
            != USB_REQ_GET_DESCRIPTOR)
        || (req->wValue != 0x2200))
        return 0;

    /* Handle the HID report descriptor. */
    *buf = (uint8_t *) hid_report_descriptor;
    *len = sizeof(hid_report_descriptor);

    return 1;
}

void hid_set_config(usbd_device *dev, uint16_t wValue)
{
    (void) wValue;
    (void) dev;

    usbd_ep_setup(dev, 0x81,
                  USB_ENDPOINT_ATTR_INTERRUPT,
                  64,
                  NULL);

    usbd_register_control_callback(
            dev,
            USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
            USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
            hid_control_request);

    tim2_setup();
}

void tim2_setup(void)
{
    /* Enable TIM2 clock. */
    rcc_periph_clock_enable(RCC_TIM2);

    /* Enable TIM2 interrupt. */
    nvic_enable_irq(NVIC_TIM2_IRQ);

    /* Reset TIM2 peripheral to defaults. */
    rcc_periph_reset_pulse(RST_TIM2);

    /* Timer global mode:
     * - No divider
     * - Alignment edge
     * - Direction up
     * (These are actually default values after reset above, so this call
     * is strictly unnecessary, but demos the api for alternative settings)
     */
    timer_set_mode(TIM2,
                   TIM_CR1_CKD_CK_INT,
                   TIM_CR1_CMS_EDGE,
                   TIM_CR1_DIR_UP);

    /*
     * Please take note that the clock source for STM32 timers
     * might not be the raw APB1/APB2 clocks.  In various conditions they
     * are doubled.  See the Reference Manual for full details!
     * In our case, TIM2 on APB1 is running at double frequency
     */

    uint32_t target_tick = 120000; // scale down to 120kHz
    timer_set_prescaler(TIM2,
                        (rcc_apb1_frequency * 2 / target_tick) - 1);

    /* Disable preload. */
    timer_disable_preload(TIM2);
//    timer_continuous_mode(TIM2);

    uint32_t target_freq = 50; /* target frequency for TIM2 */
    timer_set_period(TIM2, (target_tick / target_freq) - 1);

    /* Set the initial output compare value for OC1. */
    /*    timer_set_oc_value(TIM2, TIM_OC1, 1); */
    /* Counter enable. */
    timer_enable_counter(TIM2);

    /* Enable Channel 1 compare interrupt to recalculate compare values */
    timer_enable_irq(TIM2,
    TIM_DIER_CC1IE);
}

////////////////////////
//
// cdcacm
//
////////////////////////

int cdcacm_control_request(
        usbd_device *usbd_dev,
        struct usb_setup_data *req,
        uint8_t **buf,
        uint16_t *len,
        void (**complete)(usbd_device *usbd_dev,
                          struct usb_setup_data *req))
{
    (void) complete;
    (void) buf;
    (void) usbd_dev;

    switch (req->bRequest) {
    case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
        /*
         * This Linux cdc_acm driver requires this to be implemented
         * even though it's optional in the CDC spec, and we don't
         * advertise it in the ACM functional descriptor.
         */
//        char local_buf[10];
//        struct usb_cdc_notification *notif = (void *)local_buf;
        /* We echo signals back to host as notification. */
//        notif->bmRequestType = 0xA1;
//        notif->bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
//        notif->wValue = 0;
//        notif->wIndex = 0;
//        notif->wLength = 2;
//        local_buf[8] = req->wValue & 3;
//        local_buf[9] = 0;
        // usbd_ep_write_packet(0x83, buf, 10);
        return 1;
    }
    case USB_CDC_REQ_SET_LINE_CODING:
        if (*len < sizeof(struct usb_cdc_line_coding)) {
            return 0;
        }
        return 1;
    }
    return 0;
}

static void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
    (void) ep;

    char buf[64];
    int len = usbd_ep_read_packet(usbd_dev, 0x02, buf, 64);

    if (len) {
        len=write(&usb_read_buffer, buf, len);
    }
}

void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
    (void) wValue;
    init_ringbuffer(&usb_read_buffer);

    usbd_ep_setup(usbd_dev, 0x02, USB_ENDPOINT_ATTR_BULK, 64,
                  cdcacm_data_rx_cb);
    usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_BULK, 64,
                  NULL);
    usbd_ep_setup(usbd_dev, 0x84, USB_ENDPOINT_ATTR_INTERRUPT, 16,
                  NULL);

    usbd_register_control_callback(
            usbd_dev,
            USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
            USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
            cdcacm_control_request);
}
