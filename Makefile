#
# Copyright (C) 2000, 2001, 2024 Robert Fitzsimons
#
# This file is subject to the terms and conditions of the GNU General
# Public License.  See the file "COPYING" in the main directory of
# this archive for more details.
#

kernel.elf : ELF_LINKER_FLAGS=-Wl,-Ttext-segment,0x00100000 -Wl,-z,noexecstack
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

TARGET =
CC = gcc
CFLAGS = -m32 -O0 -g -Wall -Wstrict-prototypes
CINC = -I include -I arch/ia32/include
OBJDUMP = objdump

.SUFFIXES :
.SUFFIXES : .o .elf

%.o : %.c
	$(TARGET)$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $(CINC) -c \
		-o $@ $<

%.o : %.S
	$(TARGET)$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $(CINC) -c \
		-o $@ $<

%.elf :
	$(TARGET)$(CC) -m32 -nostartfiles -nostdlib -static $(ELF_LINKER_FLAGS) -o $@ $^
	$(TARGET)$(OBJDUMP) --disassemble-all --all-headers --line-numbers --source $@ > $@.out

