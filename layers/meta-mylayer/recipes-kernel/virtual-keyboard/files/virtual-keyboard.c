#include <linux/module.h>
#include <linux/init.h>
#include <linux/serdev.h>
#include <linux/input.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

/*
 * Character to KEY code mapping table.
 * Indexed by ASCII character value (0-127).
 * Maps received serial characters directly to input KEY codes.
 */
static const unsigned char vk_char_to_key[128] = {
	/* 0-31: Control characters - unmapped */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 32-47: Space and punctuation */
	KEY_SPACE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 48-57: Numbers '0'-'9' */
	KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
	/* 58-64: Punctuation */
	0, 0, 0, 0, 0, 0, 0, 0,
	/* 65-90: Uppercase letters 'A'-'Z' */
	KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
	KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
	KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
	/* 91-96: More punctuation */
	0, 0, 0, 0, 0, 0,
	/* 97-122: Lowercase letters 'a'-'z' (same as uppercase) */
	KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
	KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
	KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
	/* 123-127: Remaining characters */
	0, 0, 0, 0, 0
};

#define VK_KEYMAP_SIZE (sizeof(vk_char_to_key) / sizeof(vk_char_to_key[0]))

/* Declare the probe and remove functions */
static int serdev_echo_probe(struct serdev_device *serdev);
static void serdev_echo_remove(struct serdev_device *serdev);

static struct of_device_id serdev_echo_ids[] = {
	{
		.compatible = "amh,myuart",
	}, 
	{}
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
	printk(KERN_INFO "interact_open device has opened.\n");
	return 0;
}

/*
 * interact_close() is a callback from the input close routine.
 */
static void interact_close(struct input_dev *dev)
{
	printk(KERN_INFO "interact_close device has closed.\n");
}

/**
 * @brief Callback is called whenever a character is received
 */
static size_t serdev_echo_recv(struct serdev_device *serdev, const unsigned char *data, size_t count) {
	struct input_dev *input_dev_handle;
	unsigned char ch;
	unsigned char keycode;
	size_t i;

	input_dev_handle = serdev_device_get_drvdata(serdev);
	if (!input_dev_handle) {
		printk(KERN_ERR "input_dev is null\n");
		return 0;
	}

	// /* Process each received character */
	// for (i = 0; i < count; i++) {
	// 	ch = data[i];
		
	// 	/* Bounds check: ensure character is within valid range */
	// 	if (ch >= VK_KEYMAP_SIZE) {
	// 		printk(KERN_DEBUG "serdev_echo - Ignoring out-of-range character: 0x%02x\n", ch);
	// 		continue;
	// 	}

	// 	keycode = vk_char_to_key[ch];
		
	// 	/* Only process if character maps to a valid key */
	// 	if (keycode != 0) {

	// 		if (1 || (ch >= 65 && ch <= 90)) {
	// 			input_report_key(input_dev_handle, KEY_LEFTSHIFT, 1);
	// 		}
	// 		/* Send key press event */
	// 		input_report_key(input_dev_handle, keycode, 1);
	// 		/* Send key release event */
	// 		input_report_key(input_dev_handle, keycode, 0);

	// 		if (1 || (ch >= 65 && ch <= 90)) {
	// 			input_report_key(input_dev_handle, KEY_LEFTSHIFT, 0);
	// 		}			
			
	// 		printk(KERN_DEBUG "serdev_echo - Mapped char '%c' (0x%02x) to KEY_%u\n",
	// 		       (ch >= 32 && ch < 127) ? ch : '?', ch, keycode);
	// 	}
	// }
	
	// /* Sync all events at once for better performance */
	// input_sync(input_dev_handle);

	// if (data) {
	// 	serdev_device_write_buf(serdev, data, count);
	// 	serdev_device_write_buf(serdev, "\r\n", sizeof("\r\n")-1);
	// }

	if (count >= 1 && data[0] == 0x0) {
		printk(KERN_DEBUG "serdev_echo - Sync\n");
		input_sync(input_dev_handle);
	} else if (count >= 12 && data[0] == 0x1) {
		printk(KERN_DEBUG "serdev_echo - Mapped char |||| (0x%02x) (0x%02x) (0x%02x) (0x%02x) |||| (0x%02x) (0x%02x) (0x%02x) (0x%02x) |||| (0x%02x) (0x%02x) (0x%02x) (0x%02x)\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11]);
		unsigned int keyboard_code 	=  (data[4] | data[5] >> 8 | data[6] >> 16 | data[7] >> 24);
		int keyboard_value 			=  (data[8] | data[9] >> 8 | data[10] >> 16 | data[11] >> 24);
		printk(KERN_DEBUG "serdev_echo - Mapped char (%u) (%d)\n", keyboard_code, keyboard_code);

		input_report_key(input_dev_handle, keyboard_code, keyboard_value);
	// } else if (count >= 12 && data[0] == EV_MSC) {
	// 	unsigned int keyboard_code 	=  (data[4] | data[5] >> 8 | data[6] >> 16 | data[7] >> 24);
	// 	input_event(input_dev_handle, EV_MSC, MSC_SCAN, keyboard_code);
	} else {
		printk(KERN_ERR "serdev_echo - Invalid input\n");
	}

	// printk(KERN_DEBUG "serdev_echo - Mapped char (0x%02x)\n", data[0]);
	
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

	// Set up input device properties
	input_dev_handle->name = "myevent";
	input_dev_handle->phys = "serdev-echo/input0";
	input_dev_handle->id.bustype = BUS_RS232;  // Changed from BUS_VIRTUAL
	input_dev_handle->id.vendor = 0x0001;
	input_dev_handle->id.product = 0x0001;
	input_dev_handle->id.version = 0x0001;
	
	// CRITICAL: Set parent device for proper device hierarchy
	input_dev_handle->dev.parent = &serdev->dev;
	
	// CRITICAL: Assign open and close callbacks
	input_dev_handle->open = interact_open;
	input_dev_handle->close = interact_close;
	
	// Set up event capabilities
	set_bit(EV_KEY, input_dev_handle->evbit);
	// set_bit(EV_REP, input_dev_handle->evbit);
	// set_bit(EV_MSC, input_dev_handle->evbit);
	// set_bit(EV_LED, input_dev_handle->evbit);

	set_bit(KEY_LEFTSHIFT, input_dev_handle->keybit);
	set_bit(KEY_ENTER, input_dev_handle->keybit);
	// set_bit(LED_NUML, input_dev_handle->ledbit);
	// set_bit(LED_CAPSL, input_dev_handle->ledbit);

	
	// Automatically register all keys from the character mapping
	// This ensures all mapped keys are properly registered
	for (unsigned int i = 0; i < VK_KEYMAP_SIZE; i++) {
		unsigned char keycode = vk_char_to_key[i];
		if (keycode != 0) {
			set_bit(keycode, input_dev_handle->keybit);
		}
	}
	
	// Optional: Enable key repeat for better typing experience
	// set_bit(EV_REP, input_dev_handle->evbit);

	// Set driver data BEFORE registering
	serdev_device_set_drvdata(serdev, input_dev_handle);

	// Register the input device
	err = input_register_device(input_dev_handle);
	if (err) {
		printk(KERN_ERR "serdev_echo - Failed to register input device: %d\n", err);
		goto fail_register;
	}

	// Now set up serial device
	serdev_device_set_client_ops(serdev, &serdev_echo_ops);
	status = serdev_device_open(serdev);
	if(status) {
		printk("serdev_echo - Error opening serial port: %d\n", status);
		err = -status;
		goto fail_serial;
	}

	serdev_device_set_baudrate(serdev, 115200);
	serdev_device_set_flow_control(serdev, false);
	serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);

	const char mesg_to_send[] = "Type something: \r\n";
	status = serdev_device_write_buf(serdev, mesg_to_send, sizeof(mesg_to_send) - 1);
	printk("serdev_echo - Wrote %d bytes.\n", status);

	return 0;

fail_serial:
	input_unregister_device(input_dev_handle);
	// Don't set drvdata to NULL here, remove will handle it
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
	
	serdev_device_close(serdev);
	
	input_dev_handle = serdev_device_get_drvdata(serdev);
	if (input_dev_handle) {
		input_unregister_device(input_dev_handle);
		// input_unregister_device frees the device, so no need to call input_free_device
	}
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
	printk("serdev_echo - Unload driver\n");
	serdev_device_driver_unregister(&serdev_echo_driver);
}

module_init(my_init);
module_exit(my_exit);

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amirhossein");
MODULE_DESCRIPTION("A simple loopback driver for an UART port");