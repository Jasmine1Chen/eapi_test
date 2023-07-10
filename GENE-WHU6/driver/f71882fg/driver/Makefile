obj-m	:= f71882fg.o
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)
CFLGS	:= -DDEBUG 
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o *~ core .*.cmd *.mod.c .tmp_versions Module.* modules.* *.ko *.mod
