#include <zephyr/logging/log.h>
#include "bootutil/mcuboot_status.h"

LOG_MODULE_REGISTER(custom_mcuboot_action_hooks,4);


void mcuboot_status_change(mcuboot_status_type_t status){
  switch(status) 
  {
    case MCUBOOT_STATUS_STARTUP:
      LOG_INF("MCUBOOT_STATUS_STARTUP\n");
      break;

    case MCUBOOT_STATUS_UPGRADING:
      LOG_INF("MCUBOOT_STATUS_UPGRADING\n");
      break;

    case MCUBOOT_STATUS_BOOTABLE_IMAGE_FOUND:
      LOG_INF("MCUBOOT_STATUS_BOOTABLE_IMAGE_FOUND\n");
      break;

    case MCUBOOT_STATUS_NO_BOOTABLE_IMAGE_FOUND:
      LOG_INF("MCUBOOT_STATUS_NO_BOOTABLE_IMAGE_FOUND\n");
      break;

    case MCUBOOT_STATUS_BOOT_FAILED:
      LOG_INF("MCUBOOT_STATUS_BOOT_FAILED\n");
      break;

    case MCUBOOT_STATUS_USB_DFU_WAITING:
      LOG_INF("MCUBOOT_STATUS_USB_DFU_WAITING\n");
      break;

    case MCUBOOT_STATUS_USB_DFU_ENTERED:
      LOG_INF("MCUBOOT_STATUS_USB_DFU_ENTERED\n");
      break;

    case MCUBOOT_STATUS_USB_DFU_TIMED_OUT:
      LOG_INF("MCUBOOT_STATUS_USB_DFU_TIMED_OUT\n");
      break;

    case MCUBOOT_STATUS_SERIAL_DFU_ENTERED:
      LOG_INF("MCUBOOT_STATUS_SERIAL_DFU_ENTERED\n");
      break;
    default:
      LOG_WRN("Unexpected state\n");
  }
}
