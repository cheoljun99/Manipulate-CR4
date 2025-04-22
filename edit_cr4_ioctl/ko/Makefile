obj-m += edit_cr4_kernel.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	cp edit_cr4_kernel.ko ..

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f ../edit_cr4_kernel.ko
