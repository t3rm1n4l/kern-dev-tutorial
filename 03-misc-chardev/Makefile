obj-m := hello.o

ifeq ($(KERNEL_DIR),)
	KERNEL_DIR := /lib/modules/$(shell uname -r)/build
endif

all: test
	make -C $(KERNEL_DIR) M=$(PWD) modules

test:
	gcc -o test test.c

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
