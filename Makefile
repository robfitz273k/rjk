#
# Copyright (C) 2000, 2001 Robert Fitzsimons
#
# This file is subject to the terms and conditions of the GNU General
# Public License.  See the file "COPYING" in the main directory of
# this archive for more details.
#

kernel.elf : ELF_LINKER_FLAGS=-Wl,-Ttext,0x00100000
kernel.elf : \
	arch/ia32/start.o \
	arch/ia32/condition.o \
	arch/ia32/entry.o \
	arch/ia32/init.o \
	arch/ia32/atomic.o \
	arch/ia32/internal.o \
	arch/ia32/ioport.o \
	arch/ia32/irq.o \
	arch/ia32/memory.o \
	arch/ia32/module.o \
	arch/ia32/mutex.o \
	arch/ia32/printf.o \
	arch/ia32/processor.o \
	arch/ia32/rwlock.o \
	arch/ia32/spinlock.o \
	arch/ia32/thread.o \
	arch/ia32/time.o \
	arch/ia32/timer.o \

module.elf : \
	module/main.o \

all : kernel.elf module.elf

clean :
	$(RM) arch/ia32/*.o module/*.o *.elf *.out *.log

install : all
	mount $(FLOPPY_MOUNT_POINT)
	cp -f kernel.elf $(FLOPPY_MOUNT_POINT)
	cp -f module.elf $(FLOPPY_MOUNT_POINT)
	umount $(FLOPPY_MOUNT_POINT)

CC = gcc
CFLAGS = -O0 -g -Wall -Wstrict-prototypes
CINC = -I include -I arch/ia32/include
FLOPPY_MOUNT_POINT = /mnt/fd0

.SUFFIXES :
.SUFFIXES : .o .elf

%.o : %.c
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $(CINC) -c \
		-o $@ $<

%.o : %.S
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $(CINC) -c \
		-o $@ $<

%.elf :
	$(CC) -nostartfiles -nostdlib -static $(ELF_LINKER_FLAGS) -o $@ $^ -lgcc
	objdump --disassemble-all --all-headers --line-numbers --source $@ > $@.out

