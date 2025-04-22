/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include "bluetooth_smp.h"

int main(void)
{
  start_smp_bluetooth_adverts();
	printk("Change this to see it change.\n");

  return 0;
}
