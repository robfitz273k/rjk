/*
 * Copyright (C) 2000 Robert Fitzsimons
 */

#include "kinterface.h"

struct thread {
	struct thread* next;
	kuint status;
	kint priority;
	void* key;
	void* stack;
	kuint stack_size;
	kuint eip;
	kuint esp;
	kuint8 fpu[108];
};

#define switch_thread(ctp, ntp) \
	asm volatile ( \
		"\tpusha\n" \
		"\tmovl $0f, 0(%%eax)\n" \
		"\tmovl %%esp, 4(%%eax)\n" \
		"\tfsave 8(%%eax)\n" \
		"\tjmp *0(%%edx)\n" \
		"0:\n" \
		"\tmovl 4(%%edx), %%esp\n" \
		"\tfrstor 8(%%edx)\n" \
		"\tpopa\n" \
		: "=a" ((ctp)->eip), "=d" ((ntp)->eip) \
		: \
		: "cc", "memory" \
	)

struct thread* current;

kfunction void kthread_init() {
	current = (struct thread*)0x00001000;
}

kfunction kthread kthread_create(void (*function)(void), kint priority, void * key) {
	struct thread* ntp = knull;
	struct thread* ctp = current;

	if((ntp = kmemory_virtual_page_allocate(1)) != knull) {
		ntp->priority = priority;
		ntp->key = key;
		if((ntp->stack = kmemory_virtual_page_allocate(4)) != knull) {
			ntp->stack_size = (kpage_size * 4);
			ntp->esp = ((kuint)ntp->stack + ntp->stack_size);
			asm volatile (
				"\tpusha\n"
				"\tmovl $1f, 0(%%eax)\n"
				"\tmovl %%esp, 4(%%eax)\n"
				"\tfsave 8(%%eax)\n"
				/* new */
				"\tmovl 4(%%edx), %%esp\n"
				"\tfinit\n"
				"\tcall *%%ecx\n"
				"0: jmp 0b\n"
				/* old */
				"1:\n"
				"\tmovl 4(%%edx), %%esp\n"
				"\tfrstor 8(%%edx)\n"
				"\tpopa\n"
				:
				: "a" (&ctp->eip), "d" (&ntp->eip), "c" (function)
				: "cc", "memory"
			);
		} else {
			kmemory_virtual_page_unallocate(ntp);
			ntp = knull;
		}
	}

	return (kthread)ntp;
}

kfunction void kthread_delete(kthread thread) {
}

kfunction kthread kthread_current() {
	return (kthread)current;
}

kfunction void kthread_yeild() {
}

kfunction void kthread_sleep(kint64 second, kint32 nanosecond) {
}

kfunction void kthread_priority_set(kthread thread, kint priority) {
	((struct thread*)thread)->priority = priority;
}

kfunction kint kthread_priority_get(kthread thread) {
	return ((struct thread*)thread)->priority;
}

kfunction void* kthread_key_set(kthread thread, void* value) {
	void* temp = ((struct thread*)thread)->key;
	((struct thread*)thread)->key = value;
	return temp;
}

kfunction void* kthread_key_get(kthread thread) {
	return ((struct thread*)thread)->key;
}

