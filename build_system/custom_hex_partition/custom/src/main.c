/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>

uint8_t return68(){
    uint8_t ret = 68;
	printk("Hello world from child image built from hex\n");

    return ret;
}


// Include main to stop compiler from optimizing out return68
void main(void)
{
    return68();
}
