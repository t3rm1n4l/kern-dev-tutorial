obj-m := llist.o

ifeq ($(KERNEL_DIR),)
	KERNEL_DIR := /lib/modules/$(shell uname -r)/build
endif

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
