/*
 * Copyright (C) 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "kinternal.h"

kfunction void kmutex_init(kmutex* mutex) {
	mutex->thread = 0;
	mutex->count = 0;
}

kfunction void kmutex_lock(kmutex* mutex) {
	while(1) {
		if(
			katomic_compare_and_set_if_equal(&mutex->thread, 0, kthread_current()) ||
			katomic_compare(&mutex->thread, kthread_current())
		) {
			(mutex->count)++;
			break;
		} else {
			kthread_yield();
		}
	}
}

kfunction void kmutex_unlock(kmutex* mutex) {
	if(!(--(mutex->count))) {
		katomic_set(&mutex->thread, 0);
	}
}

