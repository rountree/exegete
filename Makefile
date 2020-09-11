# SPDX-License-Identifier: GPL-2.0-only
# obj-$(CONFIG_SAMPLE_KOBJECT) += kobject-example.o kset-example.o

#kset-example-objs := kset-example.o
obj-m += kobj-example.o kset-example.o exegete.o

all: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	rm -f *.ko *.mod *.mod.c *.o *.order *.symvers

