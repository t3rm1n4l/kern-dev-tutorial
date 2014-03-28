#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

static struct usb_device_id kbd_id_table [] =
{
    { USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
                         USB_INTERFACE_SUBCLASS_BOOT,
                         USB_INTERFACE_PROTOCOL_KEYBOARD) },
    { /* Terminating entry */ }
};

MODULE_DEVICE_TABLE(usb, kbd_id_table);

static int __init hello_init(void)
{
	pr_debug("Hello world!\n");
	return 0;
}

static void __exit hello_cleanup(void)
{
	pr_debug("Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);



