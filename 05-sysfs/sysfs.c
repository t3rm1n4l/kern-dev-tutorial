#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/stat.h>
#include <linux/mutex.h>

#define ID_STR "some_id"
#define ROOT_DIR "root_sysfs_dir"
#define ID_FILE "id"
#define FOO_FILE "foo"
#define JIFFIES_FILE "jiffies"

static char id_str[] = ID_STR;
static int foo_size;
static char foo_tmp[PAGE_SIZE];
static struct mutex foo_mutex;

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buff)
{
	memcpy(buff, id_str, sizeof(id_str));
	return sizeof(id_str);
}

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buff, size_t count)
{
	if (count != sizeof(id_str))
		return -EINVAL;

	if (strncmp(id_str, buff, sizeof(id_str) - 1))
		return -EINVAL;

	return count;
}

static struct kobj_attribute id_attribute =
	__ATTR(id, S_IRUGO | S_IWUGO, id_show, id_store);

static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buff)
{
	return sprintf(buff, "%lu\n", jiffies);
}

static struct kobj_attribute jiffies_attribute =
	__ATTR(jiffies, S_IRUGO, jiffies_show, NULL);

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buff)
{
	mutex_lock(&foo_mutex);
	strncpy(buff, foo_tmp, foo_size);
	mutex_unlock(&foo_mutex);
	return foo_size;
}

static ssize_t foo_store(struct  kobject *kobj, struct kobj_attribute *attr,
		const char *buff, size_t count)
{
	int ret;

	foo_size = min(count, PAGE_SIZE);
	mutex_lock(&foo_mutex);
	strncpy(foo_tmp, buff, foo_size);
	mutex_unlock(&foo_mutex);
	return count;
}

static struct kobj_attribute foo_attribute =
	__ATTR(foo, S_IRUGO | S_IWUSR, foo_show, foo_store);

static struct attribute *attrs[] = {
	&id_attribute.attr,
	&jiffies_attribute.attr,
	&foo_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *kobj;

int __init sysfs_init(void)
{
	int ret;

	mutex_init(&foo_mutex);
	kobj = kobject_create_and_add(ROOT_DIR, kernel_kobj);
	if (!kobj)
		return -ENOMEM;

	ret = sysfs_create_group(kobj, &attr_group);
	if (ret)
		kobject_put(kobj);

	return ret;
}

void __exit sysfs_exit(void)
{
	kobject_put(kobj);
}

module_init(sysfs_init);
module_exit(sysfs_exit);
MODULE_LICENSE("GPL");
