/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

kfunction kuint kioport_in_kuint(kuint port) {
	kuint value;
	asm volatile(
		"inl %%dx, %%eax ;"
		: "=a" (value)
		: "d" (port)
	);
	return value;
}
kfunction kuint8 kioport_in_kuint8(kuint port) {
	kuint value;
	asm volatile(
		"inb %%dx, %%al ;"
		: "=a" (value)
		: "d" (port)
	);
	return value;
}
kfunction kuint16 kioport_in_kuint16(kuint port) {
	kuint value;
	asm volatile(
		"inw %%dx, %%ax ;"
		: "=a" (value)
		: "d" (port)
	);
	return value;
}
kfunction kuint32 kioport_in_kuint32(kuint port) {
	kuint value;
	asm volatile(
		"inl %%dx, %%eax ;"
		: "=a" (value)
		: "d" (port)
	);
	return value;
}
kfunction kuint64 kioport_in_kuint64(kuint port) {
	return 0;
}

kfunction void kioport_out_kuint(kuint port, kuint value) {
	asm volatile(
		"outl %%eax, %%dx ;"
		:
		: "d" (port), "a" (value)
	);
}
kfunction void kioport_out_kuint8(kuint port, kuint8 value) {
	asm volatile(
		"outb %%al, %%dx ;"
		:
		: "d" (port), "a" (value)
	);
}
kfunction void kioport_out_kuint16(kuint port, kuint16 value) {
	asm volatile(
		"outw %%ax, %%dx ;"
		:
		: "d" (port), "a" (value)
	);
}
kfunction void kioport_out_kuint32(kuint port, kuint32 value) {
	asm volatile(
		"outl %%eax, %%dx ;"
		:
		: "d" (port), "a" (value)
	);
}
kfunction void kioport_out_kuint64(kuint port, kuint64 value) {
}

kfunction kuint kioport_in_kuint_port(kuint port, kuint* array, kuint offset, kuint length) {
	array += offset;
	asm volatile(
		"cld ;"
		"rep ;"
		"insl ;"
		: "=c" (length), "=d" (port), "=D" (array)
		:
		: "memory"
	);
	return length;
}
kfunction kuint kioport_in_kuint8_port(kuint port, kuint8* array, kuint offset, kuint length) {
	array += offset;
	asm volatile(
		"cld ;"
		"rep ;"
		"insb ;"
		: "=c" (length), "=d" (port), "=D" (array)
		:
		: "memory"
	);
	return length;
}
kfunction kuint kioport_in_kuint16_port(kuint port, kuint16* array, kuint offset, kuint length) {
	array += offset;
	asm volatile(
		"cld ;"
		"rep ;"
		"insw ;"
		: "=c" (length), "=d" (port), "=D" (array)
		:
		: "memory"
	);
	return length;
}
kfunction kuint kioport_in_kuint32_port(kuint port, kuint32* array, kuint offset, kuint length) {
	array += offset;
	asm volatile(
		"cld ;"
		"rep ;"
		"insl ;"
		: "=c" (length), "=d" (port), "=D" (array)
		:
		: "memory"
	);
	return length;
}
kfunction kuint kioport_in_kuint64_port(kuint port, kuint64* array, kuint offset, kuint length) {
	return 0;
}

kfunction kuint kioport_out_kuint_port(kuint port, kuint* array, kuint offset, kuint length) {
	array += offset;
	asm volatile(
		"cld ;"
		"rep ;"
		"outsl ;"
		: "=c" (length), "=d" (port), "=D" (array)
		:
		: "memory"
	);
	return length;
}
kfunction kuint kioport_out_kuint8_port(kuint port, kuint8* array, kuint offset, kuint length) {
	array += offset;
	asm volatile(
		"cld ;"
		"rep ;"
		"outsb ;"
		: "=c" (length), "=d" (port), "=D" (array)
		:
		: "memory"
	);
	return length;
}
kfunction kuint kioport_out_kuint16_port(kuint port, kuint16* array, kuint offset, kuint length) {
	array += offset;
	asm volatile(
		"cld ;"
		"rep ;"
		"outsw ;"
		: "=c" (length), "=d" (port), "=D" (array)
		:
		: "memory"
	);
	return length;
}
kfunction kuint kioport_out_kuint32_port(kuint port, kuint32* array, kuint offset, kuint length) {
	array += offset;
	asm volatile(
		"cld ;"
		"rep ;"
		"outsl ;"
		: "=c" (length), "=d" (port), "=D" (array)
		:
		: "memory"
	);
	return length;
}
kfunction kuint kioport_out_kuint64_port(kuint port, kuint64* array, kuint offset, kuint length) {
	return 0;
}

