/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define RETURN68_ADDR   0xf83d4 // Found from searching build/zephyr/zephyr.map for "return68"

void main(void)
{
	printk("Hello world from app\n");

    void* function_ptr = (void*)(RETURN68_ADDR+1);

    uint8_t ret = ((uint8_t (*)(void*))function_ptr)(NULL);

    printk("Returned %d\n",ret);
}
