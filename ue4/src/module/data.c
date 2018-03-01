#include <linux/fs.h>
#include <linux/cred.h>
#include <asm-generic/uaccess.h>
#include "data.h"

static sv_dev_t devices[DEVICE_DATA_AMOUNT];

static void data_crypt(char *data, const char *key, int start, int len) {
    for (int i = 0; i < len; i++) {
        data[i] ^= key[(start + i) % KEY_SIZE];
    }
}

static int data_open(struct inode *inode, struct file *file) {
    int ret = 0;

    sv_dev_t *dev = container_of(inode->i_cdev, struct sv_dev, cdev);
    file->private_data = dev;

    if (down_interruptible(&dev->sem)) return -ERESTARTSYS;

    // Cannot open a device you have no access to
    if (!data_user_has_access(dev->id)) {
        ret = -EACCES;
        goto out;
    }

    alerti("Attempting to open vault", dev->id);

    // If file is opened for writing, clear it's content first
    if ((file->f_flags & O_ACCMODE) == O_WRONLY) data_clear(dev->id);

out: // exit the function
    up(&dev->sem);
    return ret;
}

static int data_release(struct inode *inode, struct file *file) {
    sv_dev_t *dev = file->private_data;

    alerti("Attempting to release vault", dev->id);

    // Cannot release device you have no access to
    if (!data_user_has_access(dev->id)) return -EACCES;

    return 0;
}

static ssize_t data_read(struct file *file, char __user *buffer, size_t count, loff_t *offsetp) {
    ssize_t ret = 0;
    sv_dev_t *dev = file->private_data;
    loff_t offset = *offsetp;
    char content[count];

    if (down_interruptible(&dev->sem)) return -ERESTARTSYS;

    // Cannot open a device you have no access to
    if (!data_user_has_access(dev->id)) {
        ret = -EACCES;
        goto out;
    }

    // Check if the offset is in the correct boundaries
    if (offset < 0 || offset > dev->current_size) {
        ret = -EINVAL;
        goto out;
    }

    if (offset + count > dev->current_size) {
        count = dev->current_size - (int) offset;
    }

    alerti("Reading from ", dev->id);

    memcpy(content, &(dev->content[offset]), count);

    // decrypt and copy
    data_crypt(content, dev->key, (int) offset, (int) count);
    if (copy_to_user(buffer, content, count) != 0) {
        ret = -EFAULT;
        goto out;
    }

    *offsetp += count;
    ret = count;

out: // exit the function
    alerti("Done reading from", dev->id);
    up(&dev->sem);
    return ret;
}

static ssize_t data_write(struct file *file, const char __user *buffer, size_t count, loff_t *offsetp) {
    ssize_t ret;
    sv_dev_t *dev = file->private_data;
    loff_t offset = *offsetp;

    if (down_interruptible(&dev->sem)) return -ERESTARTSYS;

    // Cannot open a device you have no access to
    if (!data_user_has_access(dev->id)) {
        ret = -EACCES;
        goto out;
    }

    if (offset < 0 || offset > dev->current_size || offset + count > dev->max_size) {
        ret = -EINVAL;
        goto out;
    }

    alerti("Writing to ", dev->id);

    if (copy_from_user(&(dev->content[offset]), buffer, count) != 0) {
        ret = -EFAULT;
        goto out;
    }

    ret = count;

    // encrypt and save
    data_crypt(&(dev->content[offset]), dev->key, (int) offset, (int) count);
    *offsetp += count;
    dev->current_size = (unsigned int) *offsetp;

out: // exit the function
    alerti("Done reading from", dev->id);
    up(&dev->sem);
    return ret;
}

static loff_t data_llseek(struct file *file, loff_t offset, int mode) {
    struct sv_dev *dev = file->private_data;
    loff_t pos;

    // chose between modes
    switch (mode) {
        case 0: // SEEK_SET
            pos = offset;
            break;
        case 1: // SEEK_CUR
            pos = file->f_pos + offset;
            break;
        case 2: // SEEK_END
            pos = dev->current_size + offset;
            break;
        default:
            return -EINVAL;
    }

    if (pos < 0) return -EINVAL;
    file->f_pos = pos;
    return pos;
}

static const struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = data_open,
        .read = data_read,
        .write = data_write,
        .release = data_release,
        .llseek = data_llseek
};

// Operations

int data_device_create(unsigned int id, unsigned int size, char *key) {
    int ret = 0;
    dev_t dev = MKDEV(DEVICE_MAJOR, id + 1);

    if (devices[id].content != NULL) {
        ret = -1;
        goto out;
    }

    alerti("Creating data device with id", id);

    cdev_init(&devices[id].cdev, &fops);
    devices[id].cdev.owner = THIS_MODULE;
    devices[id].cdev.ops = &fops;

    if ((ret = cdev_add(&devices[id].cdev, dev, 1)) < 0) {
        goto out;
    }

    devices[id].max_size = size;
    data_update_key(id, key);
    devices[id].user = current_uid().val;

    // allocate normal kernel ram
    devices[id].content = kmalloc(size * sizeof(char), GFP_KERNEL);
    if (devices[id].content == NULL) {
        data_device_destroy(id);
        ret = -ENOMEM;
    }
out:
    return ret;
}

void data_device_destroy(unsigned int id) {
    alerti("Destroying data device with id", id);

    kfree(devices[id].content);
    devices[id].content = NULL;
    devices[id].user = 0;
    cdev_del(&devices[id].cdev);
}

void data_clear(unsigned int device_id) {
    memset(devices[device_id].content, 0, devices[device_id].max_size * sizeof(char));
    devices[device_id].current_size = 0;
}

void data_update_key(unsigned int id, char *key) {
    memcpy(devices[id].key, key, KEY_SIZE * sizeof(char));
}

unsigned int data_size(unsigned int id) {
    return devices[id].max_size;
}

bool data_user_has_access(unsigned int device_id) {
    sv_dev_t dev = devices[device_id];

    // vault unused or vault owner is current user
    if (dev.content == NULL || dev.user == current_uid().val) {
        return true;
    }

    return false;
}

void data_device_destroy_all() {
    for (unsigned int i = 0; i < DEVICE_DATA_AMOUNT; i++) {
        sv_dev_t dev = devices[i];
        if (dev.content != NULL) data_device_destroy(dev.id);
    }
}

void __init data_device_setup_all() {
    for (unsigned int i = 0; i < DEVICE_DATA_AMOUNT; i++) {
        memset(&devices[i], 0, sizeof(sv_dev_t));
        devices[i].id = i;
        sema_init(&devices[i].sem, 1);
    }
}