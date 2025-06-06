#include <linux/module.h>
#include <linux/init.h>
#include <linux/serdev.h>
#include <linux/input.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

#define BUFFER_SIZE 256

/* Declate the probe and remove functions */
static int serdev_echo_probe(struct serdev_device *serdev);
static void serdev_echo_remove(struct serdev_device *serdev);

static struct of_device_id serdev_echo_ids[] = {
	{
		.compatible = "amh,myuart",
	}, { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, serdev_echo_ids);

static struct serdev_device_driver serdev_echo_driver = {
	.probe = serdev_echo_probe,
	.remove = serdev_echo_remove,
	.driver = {
		.name = "serdev-echo",
		.of_match_table = serdev_echo_ids,
	},
};

/*
 * interact_open() is a callback from the input open routine.
 */

static int interact_open(struct input_dev *dev)
{
	printk(KERN_INFO "interact_open device has opened.");
	// struct interact *interact = input_get_drvdata(dev);

	// gameport_start_polling(interact->gameport);
	return 0;
}

/*
 * interact_close() is a callback from the input close routine.
 */

static void interact_close(struct input_dev *dev)
{
	printk(KERN_INFO "interact_open device has closed.");
	// struct interact *interact = input_get_drvdata(dev);

	// gameport_stop_polling(interact->gameport);
}

/**
 * @brief Callback is called whenever a character is received
 */
static int serdev_echo_recv(struct serdev_device *serdev, const unsigned char *data, size_t count) {
	struct input_dev *input_dev_handle;

	input_dev_handle = serdev_device_get_drvdata(serdev);
	if (!input_dev_handle) {
		printk(KERN_ERR "input_dev is null");
		return -1;
	}
	
	// Send button press
	input_report_key(input_dev_handle, BTN_0, 1);
	input_sync(input_dev_handle);
	
	// Send button release (important for proper event handling)
	input_report_key(input_dev_handle, BTN_0, 0);
	input_sync(input_dev_handle);

	input_report_key(input_dev_handle, KEY_ESC, 1);
	input_sync(input_dev_handle);

	input_report_key(input_dev_handle, KEY_ESC, 0); // release
	input_sync(input_dev_handle);
	
	printk(KERN_INFO "serdev_echo - Generated input event for %zu bytes\n", count);
	
	return count;
}

static const struct serdev_device_ops serdev_echo_ops = {
	.receive_buf = serdev_echo_recv,
};

/**
 * @brief This function is called on loading the driver 
 */
static int serdev_echo_probe(struct serdev_device *serdev) {
	int err;
	int status;
	struct input_dev *input_dev_handle;
	
	printk("serdev_echo - Now I am in the probe function!\n");

	// Allocate input device first
	input_dev_handle = input_allocate_device();
	if (!input_dev_handle) {
		err = -ENOMEM;
		goto fail_alloc;
	}

	// Set up input device properties BEFORE setting drvdata
	input_dev_handle->name = "myevent";
	input_dev_handle->id.bustype = BUS_RS232;
	input_dev_handle->open = interact_open;
	input_dev_handle->close = interact_close;
	
	// Set up event capabilities
	set_bit(EV_KEY, input_dev_handle->evbit);
	set_bit(BTN_0, input_dev_handle->keybit);
	// set_bit(KEY_ENTER, input_dev_handle->keybit);
	set_bit(KEY_ESC, input_dev_handle->keybit);



	// Register the input device
	err = input_register_device(input_dev_handle);
	if (err) {
		printk(KERN_ERR "serdev_echo - Failed to register input device\n");
		goto fail_register;
	}

	// Set driver data AFTER input device is properly set up
	serdev_device_set_drvdata(serdev, input_dev_handle);

	// Now set up serial device
	serdev_device_set_client_ops(serdev, &serdev_echo_ops);
	status = serdev_device_open(serdev);
	if(status) {
		printk("serdev_echo - Error opening serial port!\n");
		err = -status;
		goto fail_serial;
	}

	serdev_device_set_baudrate(serdev, 9600);
	serdev_device_set_flow_control(serdev, false);
	serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);

	status = serdev_device_write_buf(serdev, "Type something: ", sizeof("Type something: ") - 1);
	printk("serdev_echo - Wrote %d bytes.\n", status);

	return 0;

fail_serial:
	input_unregister_device(input_dev_handle);
	return err;
fail_register:
	input_free_device(input_dev_handle);
fail_alloc:
	return err;
}

/**
 * @brief This function is called on unloading the driver 
 */
static void serdev_echo_remove(struct serdev_device *serdev) {
	struct input_dev *input_dev_handle;

	printk("serdev_echo - Now I am in the remove function\n");
	input_dev_handle = serdev_device_get_drvdata(serdev);
	if (input_dev_handle) {
		input_unregister_device(input_dev_handle);
	}
	serdev_device_close(serdev);
}

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init my_init(void) {
	printk("serdev_echo - Loading the driver...\n");
	if(serdev_device_driver_register(&serdev_echo_driver)) {
		printk("serdev_echo - Error! Could not load driver\n");
		return -1;
	}
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit my_exit(void) {
	printk("serdev_echo - Unload driver");
	serdev_device_driver_unregister(&serdev_echo_driver);
}

module_init(my_init);
module_exit(my_exit);

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amirhossein");
MODULE_DESCRIPTION("A simple loopback driver for an UART port");