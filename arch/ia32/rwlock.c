/*
 * Copyright (C) 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

const kuint krwlock_debug = 0;

kfunction void krwlock_read_lock(volatile kuint* lock) {
	kspinlock_lock(lock);

	if(krwlock_debug) kprintf("rl 1 %08x %08x\n", *lock, kthread_current());

	while(1) {
		(*((kint16*)lock))++;
		if((*((kint16*)lock)) > 0) {
			break;
		}
		(*((kint16*)lock))--;

		if(krwlock_debug) kprintf("rl 2 %08x %08x\n", *lock, kthread_current());

		kspinlock_unlock(lock);
		kthread_yield();
		kspinlock_lock(lock);
	}

	if(krwlock_debug) kprintf("rl 3 %08x %08x\n", *lock, kthread_current());

	kspinlock_unlock(lock);
}

kfunction void krwlock_read_unlock(volatile kuint* lock) {
	kspinlock_lock(lock);

	if(krwlock_debug) kprintf("ru 1 %08x %08x\n", *lock, kthread_current());

	(*((kint16*)lock))--;

	if(krwlock_debug) kprintf("ru 2 %08x %08x\n", *lock, kthread_current());

	kspinlock_unlock(lock);
}

kfunction void krwlock_write_lock(volatile kuint* lock) {
	kuint temp = 0;

	kspinlock_lock(lock);

	if(krwlock_debug) kprintf("wl 1 %08x %08x %08x\n", *lock, kthread_current(), temp);

	if(katomic_bit_test_and_set(lock, 15)) {
		while((temp = (*lock & 0x7FFF7FFF))) {
			if(krwlock_debug) kprintf("wl 2 %08x %08x %08x\n", *lock, kthread_current(), temp);

			kspinlock_unlock(lock);
			kthread_yield();
			kspinlock_lock(lock);
		}
	} else {
		while((temp = (*lock & 0x00007FFF))) {
			if(krwlock_debug) kprintf("wl 3 %08x %08x %08x\n", *lock, kthread_current(), temp);

			kspinlock_unlock(lock);
			kthread_yield();
			kspinlock_lock(lock);
		}
	}

	*lock += 0x00010000;

	if(krwlock_debug) kprintf("wl 4 %08x %08x %08x\n", *lock, kthread_current(), temp);

	kspinlock_unlock(lock);
}

kfunction void krwlock_write_unlock(volatile kuint* lock) {
	kuint temp = 0;

	kspinlock_lock(lock);

	if(krwlock_debug) kprintf("wu 1 %08x %08x %08x\n", *lock, kthread_current(), temp);

	*lock -= 0x00010000;

	if(!(temp = (*lock & 0x7FFF0000))) {
		if(krwlock_debug) kprintf("wu 2 %08x %08x %08x\n", *lock, kthread_current(), temp);
		katomic_bit_reset(lock, 15);
	}

	if(krwlock_debug) kprintf("wu 3 %08x %08x %08x\n", *lock, kthread_current(), temp);

	kspinlock_unlock(lock);
}

