#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include "hello.h"

static ssize_t read(struct file *fp, char *buff, size_t count, loff_t *off);
static ssize_t write(struct file *fp, const char *buff, size_t count, loff_t *off);

static char id_str[] = ID_STR;
static char tmp[1024];
static struct miscdevice misc_cdev;

static struct file_operations fops = {
	read: read,
	write: write
};

static ssize_t read(struct file *fp, char *buff, size_t count, loff_t *off)
{
	if (count > sizeof(id_str) - *off)
		count = sizeof(id_str) - *off;

	if (!count)
		return 0;

	if (copy_to_user(buff, id_str + *off, count))
		return -EFAULT;

	*off += count;
	return count;
}

static ssize_t write(struct file *fp, const char *buff, size_t count, loff_t *off)
{
	if (count > sizeof(tmp) - *off)
		count = sizeof(tmp) - *off;

	if (copy_from_user(tmp + *off, buff, count))
		return -EFAULT;

	if (*off + count == sizeof(id_str)) {
		if (!strncmp(tmp, id_str, *off))
			return 0;

		return -EINVAL;
	}
	return -EINVAL;
}

static int __init hello_init(void)
{
	misc_cdev.minor = MISC_DYNAMIC_MINOR;
	misc_cdev.name = DEVICE_NAME;
	misc_cdev.fops = &fops;

	ret = misc_register(&misc_cdev);
	if (ret < 0)
		goto cleanup;

	pr_info("Hello world char device with minor no (%d)!\n", misc_cdev.minor);

cleanup:
	return ret;
}

static void __exit hello_cleanup(void)
{
	pr_info("Cleaning up module.\n");
	misc_deregister(&misc_cdev);
}

MODULE_LICENSE("GPL");
module_init(hello_init);
module_exit(hello_cleanup);
