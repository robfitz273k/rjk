/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "kinterface.h"

#define LOCK "lock ;"

kfunction kuint katomic_get(volatile kuint* atomic) {
	return (*atomic);
}

kfunction void katomic_set(volatile kuint* atomic, kuint value) {
	(*atomic) = value;
}

kfunction void katomic_inc(volatile kuint* atomic) {
	asm volatile(
		LOCK "incl %0 ;"
		: "=m" (*atomic)
	);
}

kfunction void katomic_dec(volatile kuint* atomic) {
	asm volatile(
		LOCK "decl %0 ;"
		: "=m" (*atomic)
	);
}

kfunction void katomic_add(volatile kuint* atomic, kuint value) {
	asm volatile(
		LOCK "addl %1, %0 ;"
		: "=m" (*atomic)
		: "r" (value)
	);
}

kfunction void katomic_sub(volatile kuint* atomic, kuint value) {
	asm volatile(
		LOCK "subl %1, %0 ;"
		: "=m" (*atomic)
		: "r" (value)
	);
}

kfunction kuint katomic_dec_and_test(volatile kuint* atomic) {
	kuint8 test;

	asm volatile(
		LOCK "decl %0 ;"
		"sete %1 ;"
		: "=m" (*atomic), "=m" (test)
	);

	return test != 0;
}

kfunction kuint katomic_compare(volatile kuint* atomic, kuint compare) {
	kuint8 test;

	asm volatile(
		LOCK "subl %2, %0 ;"
		"sete %1 ;"
		: "=q" (compare), "=m" (test)
		: "m" (*atomic), "0" (compare)
		: "memory"
	);

	return test != 0;
}

kfunction kuint katomic_compare_and_set_if_equal(volatile kuint* atomic, kuint compare, kuint value) {
#if(CPU == 386)
	kuint test = 0;
	kuint flags;

	kprocessor_save_flags(&flags);
	kirq_disable_all();

	if((*atomic) == compare) {
		(*atomic) = value;
		test = 1;
	}

	kprocessor_restore_flags(&flags);

	return test;
#else
	kuint8 test;

	asm volatile(
		LOCK "cmpxchgl %3, %2 ;"
		"sete %1 ;"
		: "=a" (compare), "=m" (test)
		: "m" (*atomic), "q" (value), "0" (compare)
		: "memory"
	);

	return test != 0;
#endif
}

kfunction kuint katomic_bit_get(volatile kuint* atomic, kuint bit) {
	kuint8 test;

	asm volatile(
		LOCK "btl %2, %0 ;"
		"setc %1 ;"
		: "=m" (*atomic), "=m" (test)
		: "ir" (bit)
	);

	return test;
}

kfunction void katomic_bit_set(volatile kuint* atomic, kuint bit) {
	asm volatile(
		LOCK "btsl %1, %0 ;"
		: "=m" (*atomic)
		: "ir" (bit)
	);
}

kfunction void katomic_bit_reset(volatile kuint* atomic, kuint bit) {
	asm volatile(
		LOCK "btrl %1, %0 ;"
		: "=m" (*atomic)
		: "ir" (bit)
	);
}

kfunction kuint katomic_bit_test_and_set(volatile kuint* atomic, kuint bit) {
	kuint8 test;

	asm volatile(
		LOCK "btsl %2, %0 ;"
		"setc %1 ;"
		: "=m" (*atomic), "=m" (test)
		: "ir" (bit)
	);

	return test;
}

kfunction kuint katomic_bit_test_and_reset(volatile kuint* atomic, kuint bit) {
	kuint8 test;

	asm volatile(
		LOCK "btrl %2, %0 ;"
		"setc %1 ;"
		: "=m" (*atomic), "=m" (test)
		: "ir" (bit)
	);

	return test;
}

