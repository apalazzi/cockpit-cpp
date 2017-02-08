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

#include "main.hpp"

using namespace std;
using namespace halarm;

controls_t controls = { 0, 0, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, } };
usbd_device* udev;

/* Set up a timer to create 1m ticks. */
void systick_setup(void)
{
    /* clock rate / 1000 to get 1ms interrupt rate */
    systick_set_reload(72000);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();
    /* this done last */
    systick_interrupt_enable();
}

int main(void)
{
#ifdef DEBUG
    // disable write cache to debug imprecise hard fault
    volatile uint32_t* actlr = (uint32_t*) 0xE000E008;
    *actlr |= (1 << 1);
#endif

    int err;
    setup_clock();
    systick_setup();

    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOD);

    DigitalIO led = DigitalIO(PD15, gpio_mode_output, pull_mode_none);
    DigitalIO blink = DigitalIO(PD12, gpio_mode_output, pull_mode_none);
    DigitalIO pulse = DigitalIO(PD13, gpio_mode_output, pull_mode_none);
    DigitalIO btn_feedback = DigitalIO(PD14, gpio_mode_output, pull_mode_none);
    DigitalIO reset = DigitalIO(PA0, gpio_mode_input, pull_mode_pulldown); // onboard user button
    DigitalIO sw = DigitalIO(PA1, gpio_mode_input, pull_mode_pulldown);

    vector<DigitalIO> btn;
    btn.push_back(reset);

    led.set();
    blink.set();

    SwitchResetLed autopilot=SwitchResetLed(&sw, &reset, &led);

    // setup USB
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
                GPIO9 | GPIO11 | GPIO12);
    gpio_set_af(GPIOA, GPIO_AF10, GPIO9 | GPIO11 | GPIO12);

    // some kind of USB voodoo from libopencm3 example
    gpio_clear(GPIOA, GPIO12);
    sleep(500);

    /* Buffer to be used for USB control requests. */
    uint8_t usbd_control_buffer[128];
    udev = usbd_init(&otgfs_usb_driver, &dev_descr, &config,
            usb_strings, 3,
            usbd_control_buffer, sizeof(usbd_control_buffer));
    usbd_register_set_config_callback(udev, hid_set_config);
    usbd_register_set_config_callback(udev, cdcacm_set_config);

    char cmd[64];
    char what[64];
    char val[64];
    int len=0;

    int test=0;
    while (true) {
        string ans="";
        usbd_poll (udev);

        for (int i=0; i<btn.size(); i++) {
            controls.button[i]=btn[i].read();
        }
        if (controls.button[0]==true) {
            btn_feedback.set();
        } else {
            btn_feedback.reset();
        }

        if (autopilot.value()) {
            controls.button[9]=true;
            pulse.set();
        } else {
            controls.button[9]=false;
            pulse.reset();
        }

        len=read(&usb_read_buffer, cmd, sizeof(cmd), ' ');
        if (len==0) {
            continue;
        }
        read(&usb_read_buffer, what, sizeof(what), ' ');
        len = string("GET").compare(cmd);
        if (string("GET").compare(cmd)==0) {
            if (string("AUTOPILOT").compare(what)==0) {
                ans="AUTOPILOT ";
                switch (autopilot.status()) {
                case status_engaged:
                    ans+="ON";
                    break;
                case status_disengaged:
                case status_auto_disengaged:
                    ans+="OFF";the
                    break;
                default:
                    ans+="UNKNOWN";
                    break;
                }
            }
        } else if (string("SET").compare(cmd)==0) {
            read(&usb_read_buffer, val, sizeof(val), ';');
            if (string("AUTOPILOT").compare(what)==0) {
                if (string("OFF").compare(val)==0) {
                    autopilot.status(true);
                    ans="SET OK";
                } else {
                    ans="SET FAILED";
                }
            } else if (string("X").compare(what)==0) {
                // roll axis
                int ival=atoi(val);
                controls.x=ival;
                ans="SET OK";
            } else if (string("Y").compare(what)==0) {
                // pitch
                int ival=atoi(val);
                controls.y=ival;
                ans="SET OK";
            } else if (string("Z").compare(what)==0) {
                // yaw
                int ival=atoi(val);
                controls.z=ival;
                ans="SET OK";
            } else if (string("THROTTLE").compare(what)==0) {
                // throttle
                int ival=atoi(val);
                controls.throttle=ival;
                ans="SET OK";
            } else {
                ans="SET FAILED";
            }
        } else {
            // unknown command
            ans = "UNKNOWN COMMAND";
        }
        usbd_ep_write_packet(udev, 0x83, ans.c_str(), ans.size());
    }
}

void setup_clock()
{
#ifdef DEBUG
    setup_clock_discovery();
#else
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_set_usbpre(1);
#endif
}

#ifdef DEBUG
void setup_clock_discovery()
{
    struct rcc_clock_scale r;
    r.pllm = 4;
    r.plln = 72;
    r.pllp = 2;
    r.pllq = 3;
    r.pllr = 0;
    r.hpre = RCC_CFGR_HPRE_DIV_NONE;
    r.ppre1 = RCC_CFGR_PPRE_DIV_2;
    r.ppre2 = RCC_CFGR_PPRE_DIV_NONE;
    r.power_save = 1;
    r.flash_config = FLASH_ACR_ICE | FLASH_ACR_DCE | FLASH_ACR_LATENCY_3WS;
    r.ahb_frequency = 72000000;
    r.apb1_frequency = 36000000;
    r.apb2_frequency = 72000000;

    rcc_clock_setup_hse_3v3(&r);
}
#endif

void to_buffer(controls_t data, uint8_t* buffer)
{
    *buffer++ = data.x;
    *buffer++ = data.y;
    *buffer++ = data.z;
    *buffer++ = data.elevator;

    *buffer++ = data.throttle;
    *buffer++ = data.rudder;

    uint8_t bitfield;
    for (int i = 0; i < 4; i++) {
        bitfield = 0;
        for (int j=0; j<8; j++) {
            bitfield |= data.button[8*i+j] << i;
        }
        *buffer++ = bitfield;
    }
}

#ifdef DEBUG
int cnt = 0;
#endif

void tim2_isr(void)
{
#ifdef DEBUG
    if (cnt < 25) {
        gpio_clear(GPIOD,
        GPIO12);
    } else {
        gpio_set(GPIOD,
        GPIO12);
    }
    cnt++;
    if (cnt==50) {
        cnt=0;
    }

    if (controls.button[0] != 0) {
        gpio_set(GPIOD,
        GPIO12);
    }
#endif
    if (timer_get_flag(TIM2,
    TIM_SR_CC1IF)) {
        /* Clear compare interrupt flag. */
        timer_clear_flag(TIM2,
        TIM_SR_CC1IF);
    }

    uint8_t buf[64];
    to_buffer(controls, buf);
    usbd_ep_write_packet(udev, 0x81, buf, 10);
}
