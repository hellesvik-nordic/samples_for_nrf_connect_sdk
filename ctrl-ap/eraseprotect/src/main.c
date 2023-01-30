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

    printk("Start eraseprotect sample.\n");
    NRF_CTRLAP_S->ERASEPROTECT.DISABLE = 0x00000001;
    // Somehow, reading the value back does not work.
    // printk("NRF_CTRLAP_S->ERASEPROTECT.DISABLE: %08x\n",NRF_CTRLAP_S->ERASEPROTECT.DISABLE);

    if(NRF_UICR_S->ERASEPROTECT){
        config_nvmc(NVMC_CONFIG_WEN_Wen);
        NRF_UICR_S->ERASEPROTECT=0x00000000;
        config_nvmc(NVMC_CONFIG_WEN_Ren);
        NVIC_SystemReset();
    }

    printk("Entering forever loop.\n");
    printk("Disclaimer: This example does not lock netcore, and can be unlocked by recovering netcore!\n See README for more information.\n");
    while(1){
        k_sleep(K_SECONDS(1));
    }
}
