/*
 * Copyright (C) 2000 Robert Fitzsimons
 */

#include "kinterface.h"

kfunction kuint katomic_get(kuint* atomic) {
	return (*atomic);
}

kfunction kint katomic_compare(kuint* atomic, kuint compare) {
	if((*atomic) > compare) {
		return 1;
	} else if((*atomic) < compare) {
		return -1;
	} else {
		return 0;
	}
}

kfunction void katomic_set(kuint* atomic, kuint value) {
	(*atomic) = value;
}

kfunction void katomic_inc(kuint* atomic) {
	++(*atomic);
}

kfunction void katomic_dec(kuint* atomic) {
	--(*atomic);
}

kfunction void katomic_add(kuint* atomic, kuint value) {
	(*atomic) += value;
}

kfunction void katomic_sub(kuint* atomic, kuint value) {
	(*atomic) -= value;
}

kfunction kint katomic_set_if_equal(kuint* atomic, kuint compare, kuint value) {
	if((*atomic) == compare) {
		(*atomic) = value;
		return ktrue;
	}
	return kfalse;
}

kfunction kint katomic_add_if_equal(kuint* atomic, kuint compare, kuint value) {
	if((*atomic) == compare) {
		(*atomic) += value;
		return ktrue;
	}
	return kfalse;
}

kfunction kint katomic_sub_if_equal(kuint* atomic, kuint compare, kuint value) {
	if((*atomic) == compare) {
		(*atomic) -= value;
		return ktrue;
	}
	return kfalse;
}

kfunction kint katomic_inc_if_equal(kuint* atomic, kuint compare) {
	if((*atomic) == compare) {
		++(*atomic);
		return ktrue;
	}
	return kfalse;
}

kfunction kint katomic_dec_if_equal(kuint* atomic, kuint compare) {
	if((*atomic) == compare) {
		--(*atomic);
		return ktrue;
	}
	return kfalse;
}

kfunction kint katomic_bit_get(kuint* atomic, kuint bit) {
	kuint8 value;
	asm volatile (
		"btl %2, %1\n\t"
		"setc %0"
		: "=q" (value)
		: "m" (*atomic), "ir" (bit)
		: "cc"
	);
	return value;
}

kfunction void katomic_bit_set(kuint* atomic, kuint bit) {
	asm volatile (
		"bts %1, %0"
		:
		: "m" (*atomic), "ir" (bit)
		: "cc", "memory"
	);
}

kfunction void katomic_bit_reset(kuint* atomic, kuint bit) {
	asm volatile (
		"btr %1, %0"
		:
		: "m" (*atomic), "ir" (bit)
		: "cc", "memory"
	);
}

kfunction kint katomic_bit_test_and_set(kuint* atomic, kuint bit) {
	kuint8 value;
	asm volatile (
		"bts %2, %1\n\t"
		"setc %0"
		: "=q" (value)
		: "m" (*atomic), "ir" (bit)
		: "cc", "memory"
	);
	return value;
}

kfunction kint katomic_bit_test_and_reset(kuint* atomic, kuint bit) {
	kuint8 value;
	asm volatile (
		"btr %2, %1\n\t"
		"setc %0"
		: "=q" (value)
		: "m" (*atomic), "ir" (bit)
		: "cc", "memory"
	);
	return value;
}

