#include <linux/module.h>
#include <linux/init.h>
#include <linux/serdev.h>
#include <linux/input.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>


static struct timer_list my_timer;
static struct input_dev *virt_input_dev = NULL;

static void timer_callback(struct timer_list *t) {
    // Your periodic work here
    printk(KERN_INFO "Timer callback executed\n");

    if (!virt_input_dev) {
        printk(KERN_INFO "Timer callback: is NULL\n");
        return;
    }

	// Send button press
	input_report_key(virt_input_dev, KEY_A, 1);
	input_sync(virt_input_dev);

	input_report_key(virt_input_dev, KEY_A, 0);
	input_sync(virt_input_dev);

	printk(KERN_INFO "timer_callback - Generated input event\n");	
    
    // Re-arm the timer for next execution (e.g., 1 second)
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
}

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init my_init(void) {
    int error;

    virt_input_dev = input_allocate_device();
    if (!virt_input_dev) {
        printk(KERN_ERR "button.c: Not enough memory\n");
        error = -ENOMEM;
        goto err_input_alloc_failed;
    }

    virt_input_dev->name = "Virtual Input Device";

    // Set capabilities (keyboard, mouse, etc.)
    set_bit(EV_KEY, virt_input_dev->evbit);
    set_bit(KEY_A, virt_input_dev->keybit);

    error = input_register_device(virt_input_dev);
    if (error) {
        printk(KERN_ERR "button.c: Failed to register device\n");
        goto err_free_dev;
    }    

    // Initialize timer
    timer_setup(&my_timer, timer_callback, 0);

    // Start timer (1 second from now)
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
    return 0;

err_free_dev:
    input_free_device(virt_input_dev);
err_input_alloc_failed:    
    return error;    
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit my_exit(void) {
    printk("serdev_echo - Unload driver");
    del_timer(&my_timer);
    input_unregister_device(virt_input_dev);
}

module_init(my_init);
module_exit(my_exit);

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amirhossein");
MODULE_DESCRIPTION("A simple loopback driver for an UART port");