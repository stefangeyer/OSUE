#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include "ctl.h"


static struct cdev cdev;

static long ctl_ioctl(struct file *file, unsigned int command, unsigned long arg) {
    int ret = 0;

    if (command > CMD_MAX) {
        ret = -EINVAL;
        goto out;
    }

    switch (command) {

        default:
            break;
    }

    out:
    return ret;
}

static const struct file_operations fops = {
        .owner = THIS_MODULE,
        .unlocked_ioctl = ctl_ioctl
};

int __init ctl_device_create(void) {
    alert("Creating ctl device");
    dev_t dev = MKDEV(DEVICE_MAJOR, DEVICE_CTL_MINOR);

    cdev_init(&cdev, &fops);
    cdev.owner = THIS_MODULE;
    cdev.ops = &fops;
    return cdev_add(&cdev, dev, 1);
}

void ctl_device_destroy(void) {
    alert("Destroying ctl device");
    cdev_del(&cdev);
}