/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_2 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led_3 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led_4 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

int main(void)
{
	int ret1;
	int ret2;
	int ret3;
	int ret4;
	bool led1_state = true;
	bool led2_state = true;
	bool led3_state = true;
	bool led4_state = true;


	if (!gpio_is_ready_dt(&led_2)) {
		return 0;
	}

	ret1 = gpio_pin_configure_dt(&led_1, GPIO_OUTPUT_ACTIVE);
	ret2 = gpio_pin_configure_dt(&led_2, GPIO_OUTPUT_ACTIVE);
	ret3 = gpio_pin_configure_dt(&led_3, GPIO_OUTPUT_ACTIVE);
	ret4 = gpio_pin_configure_dt(&led_4, GPIO_OUTPUT_ACTIVE);
	if (ret1 < 0) {
		return 0;
	}

	while (1) {
		ret1 = gpio_pin_toggle_dt(&led_1);
		ret2 = gpio_pin_toggle_dt(&led_2);
		ret3 = gpio_pin_toggle_dt(&led_3);
		ret4 = gpio_pin_toggle_dt(&led_4);
		if (ret1 < 0) {
			return 0;
		}

		led1_state = !led1_state;
		led2_state = !led2_state;
		led3_state = !led3_state;
		led4_state = !led4_state;
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
