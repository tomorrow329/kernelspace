#makefile
modname ?= demo
KERNELDIR:= /lib/modules/$(shell uname -r)/build
PWD:= $(shell pwd)

all:
	make -C $(KERNELDIR) M=$(PWD) modules
	gcc 02_test.c

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	rm a.out

obj-m:= $(modname).o
