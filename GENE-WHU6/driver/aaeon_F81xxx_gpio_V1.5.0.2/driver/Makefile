obj-m := gpio-fxxx.o

ifneq ("$(wildcard $(shell find /lib/modules/$(shell uname -r)))","")
KERNELDIR := $(shell find /lib/modules/$(shell uname -r)/ -iname "build")
else
KERNELDIR := $(shell find /lib/modules/ -iname "build")
endif

PWD := $(shell pwd)
all:
	make -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o *.ko *.mod.c .*.cmd modules.order Module.symvers .tmp_versions *.mod

.PHONY:clean




