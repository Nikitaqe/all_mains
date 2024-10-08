/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

#define SLEEP_TIME_MS	1

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE	DT_ALIAS(sw0)
#define SW1_NODE	DT_ALIAS(sw1)
#define SW2_NODE	DT_ALIAS(sw2)
#define SW3_NODE	DT_ALIAS(sw3)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
							      {0});
static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(SW1_NODE, gpios,
							      {0});
static const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET_OR(SW2_NODE, gpios,
							      {0});
static const struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET_OR(SW3_NODE, gpios,
							      {0});	

static struct gpio_callback button_cb_data;

/*
 * The led0 devicetree alias is optional. If present, we'll use it
 * to turn on the LED whenever the button is pressed.
 */
static struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios,
						     {0});
static struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios,
						     {0});
static struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led2), gpios,
						     {0});
static struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led3), gpios,
						     {0});							 							 							 

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&button0)) {
		printk("Error: button device %s is not ready\n",
		       button0.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
	gpio_pin_configure_dt(&button2, GPIO_INPUT);
	gpio_pin_configure_dt(&button3, GPIO_INPUT);
	gpio_pin_configure_dt(&button0, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button0.port->name, button0.pin);
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&button0,
					      GPIO_INT_EDGE_TO_ACTIVE);
	gpio_pin_interrupt_configure_dt(&button1,
					      GPIO_INT_EDGE_TO_ACTIVE);
	gpio_pin_interrupt_configure_dt(&button2,
					      GPIO_INT_EDGE_TO_ACTIVE);
	gpio_pin_interrupt_configure_dt(&button3,
					      GPIO_INT_EDGE_TO_ACTIVE);
	
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button0.port->name, button0.pin);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button0.pin));
	gpio_add_callback(button0.port, &button_cb_data);
	printk("Set up button at %s pin %d\n", button0.port->name, button0.pin);

	if (led0.port && !gpio_is_ready_dt(&led0)) {
		printk("Error %d: LED device %s is not ready; ignoring it\n",
		       ret, led0.port->name);
		led0.port = NULL;
	}
	if (led0.port) {
		ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT);
		gpio_pin_configure_dt(&led1, GPIO_OUTPUT);
		gpio_pin_configure_dt(&led2, GPIO_OUTPUT);
		gpio_pin_configure_dt(&led3, GPIO_OUTPUT);
		if (ret != 0) {
			printk("Error %d: failed to configure LED device %s pin %d\n",
			       ret, led0.port->name, led0.pin);
			led0.port = NULL;
		} else {
			printk("Set up LED at %s pin %d\n", led0.port->name, led0.pin);
		}
	}

	printk("Press the button\n");
	if (led0.port) {
		while (1) {
			/* If we have an LED, match its state to the button's. */

			if (gpio_pin_get_dt(&button0) >= 0) {
				gpio_pin_set_dt(&led0, gpio_pin_get_dt(&button0));
			}
			if (gpio_pin_get_dt(&button1) >= 0) {
				gpio_pin_set_dt(&led1, gpio_pin_get_dt(&button1));
			}
			if (gpio_pin_get_dt(&button2) >= 0) {
				gpio_pin_set_dt(&led2, gpio_pin_get_dt(&button2));
			}
			if (gpio_pin_get_dt(&button3) >= 0) {
				gpio_pin_set_dt(&led3, gpio_pin_get_dt(&button3));
			}
			k_msleep(SLEEP_TIME_MS);
		}
	}
	return 0;
}
