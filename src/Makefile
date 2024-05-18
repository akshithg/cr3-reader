CONFIG_MODULE_SIG=n

obj-m += cr3_reader.o

.PHONY: all clean install uninstall

all: clean build

build:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

in:
	sudo insmod cr3_reader.ko

rm:
	sudo rmmod cr3_reader.ko

log:
	dmesg | grep cr3_reader
