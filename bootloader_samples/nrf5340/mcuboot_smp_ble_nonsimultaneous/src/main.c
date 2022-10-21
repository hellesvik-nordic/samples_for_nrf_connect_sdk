/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include "img_mgmt/img_mgmt.h"
#include "bluetooth_smp.h"

void main(void)
{
    img_mgmt_register_group();
    start_smp_bluetooth();
	printk("Change this to see it change.\n");
}
