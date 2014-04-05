#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>

struct identity {
	char name[20];
	int id;
	bool busy;
	struct list_head list;
};

static struct identity identity_list;

int identity_create(char *name, int id)
{
	struct identity *new;
	new = kmalloc(sizeof(*new), GFP_KERNEL);
	if (!new)
		return -ENOMEM;

	strncpy(new->name, name, 20);
	new->id = id;
	new->busy = false;
	list_add_tail(&new->list, &identity_list.list);

	return 0;
}

struct identity *identity_find(int id)
{
	struct list_head *ptr;
	struct identity *entry;

	list_for_each(ptr, &identity_list.list) {
		entry = list_entry(ptr, struct identity, list);
		if (entry->id == id)
			return entry;
	}

	return NULL;
}

void identity_destroy(int id)
{
	struct identity *entry;

	entry = identity_find(id);
	if (entry)
		list_del(&entry->list);
}

#define CHECK(STMT) do {if ((ret = STMT) < 0) goto cleanup; } while (0)

static int __init llist_init(void)
{
	int ret;
	struct identity *temp;

	INIT_LIST_HEAD(&identity_list.list);

	CHECK(identity_create("Alice", 1));
	CHECK(identity_create("Bob", 2));
	CHECK(identity_create("Dave", 3));
	CHECK(identity_create("Gena", 10));

	temp = identity_find(3);
	pr_debug("id 3 = %s\n", temp->name);

	temp = identity_find(42);
	if (temp == NULL)
		pr_debug("id 42 not found\n");

	identity_destroy(2);
	identity_destroy(1);
	identity_destroy(10);
	identity_destroy(42);
	identity_destroy(3);

	return 0;

cleanup:
	return ret;
}

static void __exit llist_cleanup(void)
{
	struct list_head *ptr, *next;
	struct identity *entry;

	list_for_each_safe(ptr, next, &identity_list.list) {
		entry = list_entry(ptr, struct identity, list);
		list_del(&entry->list);
		kfree(entry);
	}
}

module_init(llist_init);
module_exit(llist_cleanup);
