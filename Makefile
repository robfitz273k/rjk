#
# Copyright (C) 2000 Robert Fitzsimons
#

TARGETS = \
	start.o init.o katomic.o kdebug.o kioport.o kirq.o kmemory.o \
	kstart.o kthread.o ktime.o

CFLAGS = -g -Wall -I.

kernel.elf : $(TARGETS)
	$(CC) -nostartfiles -static -Wl,-Ttext,0x00100000,--sort-common \
		-o $@ -lgcc $^
	objdump $@ --all-headers --disassemble-all > $@.dump

all : kernel.elf

clean :
	$(RM) *.o *.elf *.dump

install : all
	mount /mnt/floppy
	cp -f kernel.elf /mnt/floppy
	umount /mnt/floppy

.SUFFIXES :
.SUFFIXES : .o

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $<

