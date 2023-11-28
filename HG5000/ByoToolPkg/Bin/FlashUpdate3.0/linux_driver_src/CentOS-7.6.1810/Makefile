MODULE_NAME:=ufudev

ifeq ($(KERNELRELEASE),)  
KERNELDIR := /lib/modules/$(shell uname -r)/build
#KERNELDIR := /lib/modules/3.5.0-23-generic/build/
PWD := $(shell pwd) 

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	insmod ufudev.ko

modules_install:  
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install  
clean:  
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers  

.PHONY: modules modules_install clean  

else 
RESMAIN_CORE_OBJS := ufumain.o
$(MODULE_NAME)-objs := $(RESMAIN_CORE_OBJS)
obj-m := $(MODULE_NAME).o 
endif  


