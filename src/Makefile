CONFIG_MODULE_SIG=n

obj-m += cr3_reader.o

.PHONY: all clean install uninstall

#set KERNELDIR using command line
KERNELDIR ?= /lib/modules/$(shell uname -r)/build


all: clean echo build

echo:
	echo $(KERNELDIR)

build:
	make -C $(KERNELDIR) M=$(PWD) modules

clean:
	make -C $(KERNELDIR) M=$(PWD) clean

in:
	sudo insmod cr3_reader.ko

rm:
	sudo rmmod cr3_reader.ko

log:
	dmesg | grep cr3_reader
