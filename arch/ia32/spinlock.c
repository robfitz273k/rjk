/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

kfunction void kspinlock_lock(volatile kuint* lock) {
	while(!katomic_bit_test_and_set(lock, 0)) {}
}

kfunction void kspinlock_unlock(volatile kuint* lock) {
	katomic_bit_reset(lock, 0);
}

kfunction void kspinlock_read_lock(volatile kuint* lock) {
}

kfunction void kspinlock_read_unlock(volatile kuint* lock) {
}

kfunction void kspinlock_write_lock(volatile kuint* lock) {
}

kfunction void kspinlock_write_unlock(volatile kuint* lock) {
}

kfunction void kspinlock_lock_irqsave(volatile kuint* lock, kuint* flags) {
	kprocessor_save_flags(flags);
	kirq_disable_all();
	kspinlock_lock(lock);
}

kfunction void kspinlock_unlock_irqrestore(volatile kuint* lock, kuint* flags) {
	kprocessor_restore_flags(flags);
	kspinlock_unlock(lock);
}

