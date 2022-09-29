/*
 *
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include "img_mgmt/img_mgmt.h"

void main(void)
{
    img_mgmt_register_group();
	printk("Change this to see change.\n");
    while(1){
        k_sleep(K_SECONDS(1));
    }
}
