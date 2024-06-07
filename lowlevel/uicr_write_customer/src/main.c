#include <zephyr/kernel.h>
#include <nrfx_nvmc.h>

#define CUSTOMER0_TEST_DATA ((uint32_t)0x12345678)

int main(void)
{
	printk("CUSTOMER0 before: %x\n",(uint32_t)NRF_UICR->CUSTOMER[0]);
	bool is_writable = nrfx_nvmc_word_writable_check((uint32_t)&NRF_UICR->CUSTOMER[0],CUSTOMER0_TEST_DATA);

	if(is_writable){
		nrfx_nvmc_word_write((uint32_t)&NRF_UICR->CUSTOMER[0],CUSTOMER0_TEST_DATA);
		printk("Written\n");
	} else {
		printk("Not written\n");
	}

	printk("CUSTOMER0 after: %x\n",(uint32_t)NRF_UICR->CUSTOMER[0]);



	return 0;
}
