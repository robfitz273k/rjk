/*
 * Copyright (C) 2000 Robert Fitzsimons
 */

#include "kinterface.h"

struct multiboot_info {
	kuint flags;
	kuint mem_lower;
	kuint mem_upper;
	kuint boot_device;
	kuint mods_count;
	kuint mods_addr;
	kuint syms_0;
	kuint syms_1;
	kuint syms_2;
	kuint syms_3;
	kuint mmap_length;
	kuint mmap_addr;
};

extern void kmemory_setup(kuint mem_lower, kuint mem_upper);
extern void kstart();

void setup(struct multiboot_info* mb_info) {
	kuint mem_lower = (mb_info->mem_lower * 1024);
	kuint mem_upper = ((mb_info->mem_upper * 1024) + (1024 * 1024));

	kmemory_setup(mem_lower, mem_upper);
}

void init() {
	kmemory_init();
	kirq_init();
	kdebug_init();
	kthread_init();
	ktime_init();

	kthread_create(&kstart, 0, knull);
}

