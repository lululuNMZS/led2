ifneq ($(KERNELRELEASE),)
 
obj-m := rgbled2.o
 
else
	
KDIR := /home/lxl/raspi/RpiLinux/
all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=/home/lxl/raspi/RpiTools/tools-master/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-
 
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers  modul*
 
endif
