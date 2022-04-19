/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <dk_buttons_and_leds.h>

void main(void)
{
    printk("Starting sample.\n");
    dk_leds_init();
    for(;;){
        k_sleep(K_SECONDS(1));
        dk_set_led_on(1);
        k_sleep(K_SECONDS(1));
        dk_set_led_off(1);

    }
}
