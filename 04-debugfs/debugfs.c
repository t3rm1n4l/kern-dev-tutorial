#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/stat.h>
#include <linux/mutex.h>

#define ID_STR "some_id"
#define ROOT_DIR "dbgfs_root"
#define ID_FILE "id"
#define FOO_FILE "foo"
#define JIFFIES_FILE "jiffies"

static ssize_t id_read(struct file *fp, char *buff, size_t count, loff_t *off);
static ssize_t id_write(struct file *fp, const char *buff, size_t count,
								loff_t *off);
static ssize_t foo_read(struct file *fp, char *buff, size_t count, loff_t *off);
static ssize_t foo_write(struct file *fp, const char *buff, size_t count,
								loff_t *off);

static char id_str[] = ID_STR;
static char id_tmp[1024];
static int foo_size;
static char foo_tmp[PAGE_SIZE];
static struct dentry *root;
static struct mutex foo_mutex;

static const struct file_operations id_fops = {
	.read  = id_read,
	.write = id_write
};

static const struct file_operations foo_fops = {
	.read  = foo_read,
	.write = foo_write
};

static ssize_t id_read(struct file *fp, char *buff, size_t count, loff_t *off)
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

static ssize_t id_write(struct file *fp, const char *buff, size_t count,
								loff_t *off)
{
	if (count > sizeof(id_tmp) - *off)
		count = sizeof(id_tmp) - *off;

	if (copy_from_user(id_tmp + *off, buff, count))
		return -EFAULT;

	if (*off + count == sizeof(id_str)) {
		if (!strncmp(id_tmp, id_str, *off))
			return count;

		return -EINVAL;
	}
	return -EINVAL;
}

static ssize_t foo_read(struct file *fp, char *buff, size_t count,
								loff_t *off)
{
	int ret;
	ret = mutex_lock_interruptible(&foo_mutex);
	if (ret)
		return ret;

	if (count > foo_size - *off)
		count = foo_size - *off;

	if (!count) {
		ret = 0;
		goto cleanup;
	}

	if (copy_to_user(buff, foo_tmp + *off, count)) {
		ret = -EFAULT;
		goto cleanup;
	}

	*off += count;
	ret = count;

cleanup:
	mutex_unlock(&foo_mutex);
	return ret;
}

static ssize_t foo_write(struct file *fp, const char *buff, size_t count,
								loff_t *off)
{
	int ret;
	ret = mutex_lock_interruptible(&foo_mutex);
	if (ret)
		return ret;

	count = min(count, PAGE_SIZE);

	if (copy_from_user(foo_tmp, buff, count)) {
		ret = -EFAULT;
		goto cleanup;
	}

	foo_size = count;
	ret = count;

cleanup:
	mutex_unlock(&foo_mutex);
	return ret;
}

static int __init debugfs_init(void)
{
	foo_size = 0;
	root = debugfs_create_dir(ROOT_DIR, NULL);
	if (!root)
		return -ENOMEM;

	debugfs_create_file(ID_FILE, S_IRUGO | S_IWUGO, root, NULL, &id_fops);
	debugfs_create_u64(JIFFIES_FILE, S_IRUGO, root, (u64 *)&jiffies);
	debugfs_create_file(FOO_FILE, S_IRUGO | S_IWUSR, root, NULL, &foo_fops);
	mutex_init(&foo_mutex);

	return 0;
}

static void __exit debugfs_cleanup(void)
{
	debugfs_remove_recursive(root);
}

MODULE_LICENSE("GPL");
module_init(debugfs_init);
module_exit(debugfs_cleanup);

