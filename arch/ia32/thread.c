/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

#define STATUS_ALIVE   0x00000001
#define STATUS_SLEEP   0x00000002
#define STATUS_SUSPEND 0x00000004

struct thread {
	struct thread* next;
	kuint status;
	kint priority;
	void* data;
	void* key;
	void* stack;
	kuint stack_size;
	kuint esp;
	ktime time;
	kuint8 fpu[108];
};

struct thread* schedule_thread();
void cleanup_thread();
void thread_switch(struct processor_regs* regs);

struct thread* current_thread;
struct thread* thread_list;
volatile kuint kthread_spinlock;

kuint kthread_setup() {
	current_thread = (void*)kmemory_linear_page_allocate(1, 1);

	current_thread->status = STATUS_ALIVE;
	current_thread->priority = -1;
	current_thread->key = KNULL;
	current_thread->stack = 0;
	current_thread->stack_size = 0;
	current_thread->next = current_thread;
	current_thread->esp = ((kuint)current_thread + KPAGESIZE);

	return current_thread->esp;
}

kfunction kuint kthread_create(void (*function)(void* data), void* data, kint priority) {
	struct thread* ntp;
	kuint* stack;
	kuint irqsave;

	if((ntp = kmemory_virtual_page_allocate(1, 1))) {
		ntp->status = STATUS_ALIVE;
		ntp->data = data;
		ntp->priority = priority;
		ntp->key = KNULL;

		if((ntp->stack = kmemory_virtual_page_allocate(16, 0))) {
			ntp->stack_size = (KPAGESIZE * 16);
			stack = (void*)((kuint)ntp->stack + ntp->stack_size);
			ntp->esp = ((kuint)stack - ((1 + 1 + 3 + 1 + 7) * 4));

			stack[-1] = (kuint)ntp->data;
			stack[-2] = (kuint)kthread_kill_current;
			stack[-3] = 0x200; /* eflags */
			stack[-4] = 0x10; /* cs */
			stack[-5] = (kuint)function; /* eip */
			stack[-6] = 0; /* error */
			stack[-7] = 0; /* eax */
			stack[-8] = 0; /* ebx */
			stack[-9] = 0; /* ecx */
			stack[-10] = 0; /* edx */
			stack[-11] = 0; /* edi */
			stack[-12] = 0; /* esi */
			stack[-13] = 0; /* ebp */
			stack[-14] = 0; /* esp */

			kspinlock_lock_irqsave(&kthread_spinlock, &irqsave);

			ntp->next = current_thread->next;
			current_thread->next = ntp;

			kspinlock_unlock_irqrestore(&kthread_spinlock, &irqsave);

			asm volatile(
				"int $0x30 ;"
				:
				:
				: "memory"
			);
		} else {
			kmemory_virtual_page_unallocate(ntp);
			ntp = KNULL;
		}
	}

	return (kuint)ntp;
}

kfunction kuint kthread_current() {
	return (kuint)current_thread;
}

kfunction void kthread_yield() {
	asm volatile(
		"int $0x30 ;"
		:
		:
		: "memory"
	);
}

kfunction void kthread_sleep(kuint thread, kint64 second, kint32 nanosecond) {
	kuint irqsave;
	struct thread* local_thread = (struct thread*)thread;

	kspinlock_lock_irqsave(&kthread_spinlock, &irqsave);

	ktime_get(&local_thread->time);

	local_thread->time.second += second;
	local_thread->time.nanosecond += nanosecond;

	local_thread->status |= STATUS_SLEEP;

	kspinlock_unlock_irqrestore(&kthread_spinlock, &irqsave);

	if(local_thread == current_thread) {
		asm volatile(
			"int $0x30 ;"
			:
			:
			: "memory"
		);
	}
}

void kthread_kill_current() {
	kthread_kill((kuint)current_thread);
}

kfunction void kthread_kill(kuint thread) {
	kuint irqsave;
	struct thread* local_thread = (struct thread*)thread;

	kspinlock_lock_irqsave(&kthread_spinlock, &irqsave);

	local_thread->status &= ~STATUS_ALIVE;

	kspinlock_unlock_irqrestore(&kthread_spinlock, &irqsave);

	if(local_thread == current_thread) {
		asm volatile(
			"int $0x30 ;"
			:
			:
			: "memory"
		);
	}
}

kfunction void kthread_suspend(kuint thread) {
	kuint irqsave;
	struct thread* local_thread = (struct thread*)thread;

	kspinlock_lock_irqsave(&kthread_spinlock, &irqsave);

	local_thread->status |= STATUS_SUSPEND;

	kspinlock_unlock_irqrestore(&kthread_spinlock, &irqsave);

	if(local_thread == current_thread) {
		asm volatile(
			"int $0x30 ;"
			:
			:
			: "memory"
		);
	}
}

kfunction void kthread_resume(kuint thread) {
	kuint irqsave;
	struct thread* local_thread = (struct thread*)thread;

	kspinlock_lock_irqsave(&kthread_spinlock, &irqsave);

	local_thread->status &= ~STATUS_SUSPEND;

	kspinlock_unlock_irqrestore(&kthread_spinlock, &irqsave);

	if(local_thread == current_thread) {
		asm volatile(
			"int $0x30 ;"
			:
			:
			: "memory"
		);
	}
}

kfunction void kthread_priority_set(kuint thread, kint priority) {
	((struct thread*)thread)->priority = priority;
}

kfunction kint kthread_priority_get(kuint thread) {
	return ((struct thread*)thread)->priority;
}

kfunction void* kthread_key_set(kuint thread, void* value) {
	void* temp = ((struct thread*)thread)->key;

	((struct thread*)thread)->key = value;

	return temp;
}

kfunction void* kthread_key_get(kuint thread) {
	return ((struct thread*)thread)->key;
}

struct thread* schedule_thread() {
	struct thread* ntp;
	ktime time;

	ktime_get(&time);

	for(ntp = current_thread->next; ntp; ntp = ntp->next) {
		if(ntp->status & (STATUS_SLEEP)) {
			if(
				(ntp->time.second < time.second)
				|| ((ntp->time.second == time.second) && (ntp->time.nanosecond <= time.nanosecond))
			) {
				ntp->time.second = 0;
				ntp->time.nanosecond = 0;
				ntp->status &= ~STATUS_SLEEP;
				break;
			} else {
				continue;
			}
		} else if(ntp->status & (STATUS_SUSPEND)) {
			continue;
		} else if(ntp->status & (STATUS_ALIVE)) {
			break;
		}
	}

	return ntp;
}

void thread_switch(struct processor_regs* regs) {
	kuint irqsave;

	kspinlock_lock_irqsave(&kthread_spinlock, &irqsave);

	asm volatile(
		"fsave %0 ;"
		: "=m" (current_thread->fpu)
		:
		: "memory"
	);

	current_thread->esp = regs->esp;

	current_thread = schedule_thread();

	regs->esp = current_thread->esp;

	asm volatile(
		"frstor %0 ;"
		: "=m" (current_thread->fpu)
		:
		: "memory"
	);

	kspinlock_unlock_irqrestore(&kthread_spinlock, &irqsave);
}

