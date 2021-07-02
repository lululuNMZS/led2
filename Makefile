ifneq ($(KERNELRELEASE),)
 
obj-m := rgbled2.o
 
else
	
KDIR := /home/lxl/raspi/RpiLinux/

TEST := rgbled2_test
TEST_SRC := $(TEST).c
TEST_OBJ := $(TEST).o

all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=/home/lxl/raspi/RpiTools/tools-master/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf-
 
test:
	arm-linux-gnueabihf-gcc $(CFLAGS) -c $(TEST_SRC) -o $(TEST_OBJ)
	arm-linux-gnueabihf-gcc $(LFLAGS) -o $(TEST) $(TEST_OBJ)

clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers  modul*
 
endif
