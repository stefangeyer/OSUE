#include <linux/module.h>
#include <linux/fs.h>
#include "module.h"

MODULE_AUTHOR("Stefan Geyer");
MODULE_DESCRIPTION("secvault");
MODULE_LICENSE("GPL");

static bool debug;
module_param(debug, bool, 0);

static dev_t dev = MKDEV(DEVICE_MAJOR, DEVICE_CTL_MINOR);

static int __init mod_init(void) {
    int ret = 0;
    alert("Enabling module");
    ret = register_chrdev_region(dev, DEVICE_DATA_AMOUNT + 1, "secvault");
    if (ret < 0) {
        goto out;
    }

    if ((ret = ctl_device_create()) < 0) {
        unregister_chrdev_region(dev, DEVICE_DATA_AMOUNT + 1);
        goto out;
    }

    data_device_setup_all();
out:
    return ret;
}

static void __exit mod_exit(void) {
    alert("Disabling module");
    ctl_device_destroy();
    data_device_destroy_all();
    unregister_chrdev_region(dev, DEVICE_DATA_AMOUNT + 1);
}

void alert(const char *message) {
    if (debug) printk(KERN_ALERT "SECVAULT: %s\n", message);
}

void alerti(const char *message, int arg) {
    if (debug) printk(KERN_ALERT "SECVAULT: %s %d\n", message, arg);
}

module_init(mod_init);
module_exit(mod_exit);