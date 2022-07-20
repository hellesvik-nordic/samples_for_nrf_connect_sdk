/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>

static void config_nvmc(uint32_t val)
{
    while (!NRF_NVMC_S->READY);
    NRF_NVMC_S->CONFIG = val;
    while (!NRF_NVMC_S->READY);
}


void main(void)
{
    uint32_t read_val;

    read_val= NRF_CTRLAP_S->ERASEPROTECT.LOCK;
    printk(" NRF_CTRLAP_S->ERASEPROTECT.LOCK: 0x%08x\n",read_val);

    config_nvmc(NVMC_CONFIG_WEN_Wen);
    NRF_CTRLAP_S->ERASEPROTECT.DISABLE = 0x12345678;
    config_nvmc(NVMC_CONFIG_WEN_Ren);
    /* NVIC_SystemReset(); */

    read_val= NRF_CTRLAP_S->ERASEPROTECT.DISABLE;
    printk(" NRF_CTRLAP_S->ERASEPROTECT.DISABLE: 0x%08x\n",read_val);

	printk("Hello World! %s\n", CONFIG_BOARD);
}
