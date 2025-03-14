#include <zephyr/drivers/comparator.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <nrfx_lpcomp.h>

#include <zephyr/sys/poweroff.h>

LOG_MODULE_REGISTER(comp_app, LOG_LEVEL_INF);


static int poweroff = false;

static const struct device *comp_dev = DEVICE_DT_GET(DT_ALIAS(test_comp));

enum comp_state {
	WAITING_FOR_VOLT_HIGH,
	WAITING_FOR_VOLT_LOW,
};

#define LPCOMP_INPUT_CHANNEL NRF_LPCOMP_INPUT_4

static nrfx_lpcomp_config_t config_volt_high = {
    .reference = NRF_LPCOMP_REF_SUPPLY_7_8,
    .detection = NRF_LPCOMP_DETECT_UP,
#if NRF_LPCOMP_HAS_HYST
    .hyst = NRF_LPCOMP_HYST_ENABLED,
#endif
    .input = LPCOMP_INPUT_CHANNEL,
    .interrupt_priority = 6,
};

static nrfx_lpcomp_config_t config_volt_low = {
    .reference = NRF_LPCOMP_REF_SUPPLY_4_8,
    .detection = NRF_LPCOMP_DETECT_DOWN,
#if NRF_LPCOMP_HAS_HYST
    .hyst = NRF_LPCOMP_HYST_ENABLED,
#endif
    .input = LPCOMP_INPUT_CHANNEL,
    .interrupt_priority = 6,
};

static enum comp_state current_state = WAITING_FOR_VOLT_LOW;

static void comp_callback(const struct device *dev, void *user_data)
{
	nrfx_err_t err = NRFX_SUCCESS;
	ARG_UNUSED(dev);
	ARG_UNUSED(user_data);

	int output = comparator_get_output(comp_dev);

	if (current_state == WAITING_FOR_VOLT_HIGH && output == 0) {

    poweroff = false;
		LOG_INF("Voltage above VDD * 7/8");

		current_state = WAITING_FOR_VOLT_LOW;
		nrfx_lpcomp_stop();
		err = nrfx_lpcomp_reconfigure(&config_volt_low);
		nrfx_lpcomp_start(NRF_LPCOMP_INT_UP_MASK,0);
	} else if (current_state == WAITING_FOR_VOLT_LOW && output == 1) {

		LOG_INF("Voltage below VDD * 4/8");
    poweroff = true;

		current_state = WAITING_FOR_VOLT_HIGH;
		nrfx_lpcomp_stop();
		err = nrfx_lpcomp_reconfigure(&config_volt_high);
		nrfx_lpcomp_start(NRF_LPCOMP_INT_DOWN_MASK,0);
	}

	if(err != NRFX_SUCCESS ) {
		LOG_ERR("lpcomp reconfigure error: %d",err);
	}
}

int main(void)
{

	if (!device_is_ready(comp_dev)) {
		LOG_ERR("Comparator device not ready");
	}

	if (comparator_set_trigger_callback(comp_dev, comp_callback, NULL) != 0) {
		LOG_ERR("Failed to set comparator callback");
	}

	if (comparator_set_trigger(comp_dev, COMPARATOR_TRIGGER_RISING_EDGE) != 0) {
		LOG_ERR("Failed to set rising edge trigger");
	}

	LOG_INF("Comparator started; waiting for voltage events...");

	while (1) {
		k_sleep(K_MSEC(5000));
		LOG_INF("Main loop");

    if(poweroff) {
	    sys_poweroff();
    }
	}
	return 1;
}
