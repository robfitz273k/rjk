/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

#define UNSIGNED   0x00000001
#define HEX        0x00000010
#define OCTAL      0x00000020
#define SIZE_LONG  0x00000100
#define SIZE_SHORT 0x00000200
#define MINUS      0x00001000
#define ADDRESS    0x00002000

const kuint8* digits = "0123456789ABCDEF";
const kuint8* blank_line = " \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07";
kuint video_memory;
kuint video_next;
kuint8* kprintf_buffer;
volatile kuint kprintf_spinlock;

void write_character(kuint c, kuint8** buffer);
void write_string(kuint8* s, kuint8** buffer);
void write_number_kuint32(kuint32 number, kuint flags, kuint8** buffer);
void write_number_kuint64(kuint64 number, kuint flags, kuint8** buffer);
void write_number_kfloat(kfloat number, kuint flags, kuint8** buffer);
void write_number_kfloat32(kfloat32 number, kuint flags, kuint8** buffer);
void write_number_kfloat64(kfloat64 number, kuint flags, kuint8** buffer);

kfunction void kprintf_init() {
	kuint i;

	video_memory = 0x000B8000;

	kmemory_linear_page_map(video_memory, ((80 * 25 * 2) / KPAGESIZE) + 1); 

	for(i = 0; i < (80 * 25 * 2); i += (80 * 2)) {
		kmemory_virtual_copy((void*)(video_memory + i), (void*)blank_line, (80 * 2));
	}

	kprintf_buffer = kmemory_virtual_page_allocate(1, 0);
}

kfunction kint kprintf(kuint8* format, ...) {
	kuint count;
	va_list args;

	va_start(args, format);
	count = kvprintf(format, args);
	va_end(args);

	return count;
}

kfunction kint kvprintf(kuint8* format, va_list args) {
	kuint irqflags;
	kuint c;
	kuint count;
	kuint8* buffer;

	kspinlock_lock_irqsave(&kprintf_spinlock, &irqflags);

	buffer = kprintf_buffer;

	count = kvsprintf(buffer, format, args);

	while((c = *(buffer++))) {
		kioport_out_kuint8(0xE9, c); /* Write out to the bochs debug port.  */

		if(c == '\n') {
			video_next = ((video_next / 160) * 160) + 160;
		} else {
			kmemory_linear_write_kuint8(video_memory + video_next, c);
			video_next += 2;
		}

		if(video_next >= (80 * 25 * 2)) {
			video_next = (80 * 24 * 2);

			kmemory_virtual_copy((void*)(video_memory), (void*)(video_memory + 160), (80 * 24 * 2));
			kmemory_virtual_copy((void*)(video_memory + video_next), (void*)blank_line, (80 * 2));
		}
	}

	kspinlock_unlock_irqrestore(&kprintf_spinlock, &irqflags);

	return count;
}

kfunction kint kvsprintf(kuint8* buffer, kuint8* format, va_list args) {
	kuint c;
	kuint8* local_buffer = buffer;

	while((c = *(format++))) {
		if(c != '%') {
			write_character(c, &local_buffer);
			continue;
		} else if((c = *(format++))) {
			void* t;
			kuint flags = 0;

			switch(c) {
				case 'h':
					flags |= SIZE_SHORT;
					c = *(format++); 
					break;
				case 'l':
					flags |= SIZE_LONG;
					c = *(format++); 
					break;
			}
			switch(c) {
				case '%':
					write_character(c, &local_buffer);
					continue;
				case 'c':
					c = (kuint8)va_arg(args, kuint8);
					write_character(c, &local_buffer);
					continue;
				case 's':
					t = (void*)va_arg(args, kuint8*);
					if(t) {
						write_string((kuint8*)t, &local_buffer);
					} else {
						write_string("(nil)", &local_buffer);
					}
					continue;
				case 'p':
					t = (void*)va_arg(args, void*);
					if(t) {
						write_number_kuint32((kuint32)t, (HEX | UNSIGNED | ADDRESS), &local_buffer);
					} else {
						write_string("(null)", &local_buffer);
					}
					continue;
				case 'X':
				case 'x':
					flags |= HEX;
				case 'o':
					if(c == 'o') {
						flags |= OCTAL;
					}
				case 'u':
					flags |= UNSIGNED;
				case 'd':
				case 'i':
					if(flags & SIZE_LONG) {
						write_number_kuint64((kuint64)va_arg(args, kuint64), flags, &local_buffer);
					} else if(flags & SIZE_SHORT) {
						write_number_kuint32((kuint16)va_arg(args, kuint16), flags, &local_buffer);
					} else {
						write_number_kuint32((kuint32)va_arg(args, kuint32), flags, &local_buffer);
					}
					continue;
				case 'e':
				case 'f':
				case 'G':
					if(flags & SIZE_LONG) {
						write_number_kfloat64((kfloat64)va_arg(args, kfloat64), flags, &local_buffer);
					} else if(flags & SIZE_SHORT) {
						write_number_kfloat32((kfloat32)va_arg(args, kfloat32), flags, &local_buffer);
					} else {
						write_number_kfloat((kfloat)va_arg(args, kfloat), flags, &local_buffer);
					}
					continue;
			}
		}

		write_string("<format error>", &local_buffer);
		break;
	}

	(*local_buffer) = 0;

	return ((kuint)local_buffer - (kuint)buffer);
}

void write_character(kuint c, kuint8** buffer) {
	(*((*buffer)++)) = c;
}

void write_string(kuint8* s, kuint8** buffer) {
	kuint c;

	while((c = *(s++))) {
		write_character(c, buffer);
	}
}

void write_number_kuint32(kuint32 number, kuint flags, kuint8** buffer) {
	kuint c;
	kuint8 array[16];
	kuint8* p = array;
	kuint32 base = 10;

	if(flags & SIZE_SHORT) {
		number = (kuint32)((kint32)((kint16)number));
	}

	if(flags & HEX) {
		base = 16;
	} else if(flags & OCTAL) {
		base = 8;
	} else if((!(flags & UNSIGNED)) && ((number & 0x80000000ul) > 0)) {
		number = (~number) + 1;
		flags |= MINUS;
	}

	*(p++) = '\0';

	do {
		*(p++) = digits[number % base];
	} while((number /= base) > 0);

	if(flags & ADDRESS) {
		*(p++) = 'x';
		*(p++) = '0';
	} else if(flags & MINUS) {
		*(p++) = '-';
	}

	while((c = *(--p))) {
		write_character(c, buffer);
	}
}

void write_number_kuint64(kuint64 number, kuint flags, kuint8** buffer) {
	kuint c;
	kuint8 array[32];
	kuint8* p = array;
	kuint64 base = 10ull;

	if(flags & HEX) {
		base = 16ull;
	} else if(flags & OCTAL) {
		base = 8ull;
	} else if((!(flags & UNSIGNED)) && ((number & 0x8000000000000000ull) > 0)) {
		number = (~number) + 1ull;
		flags |= MINUS;
	}

	*(p++) = '\0';

	do {
		*(p++) = digits[number % base];
	} while((number /= base) > 0ull);

	if(flags & ADDRESS) {
		*(p++) = 'x';
		*(p++) = '0';
	} else if(flags & MINUS) {
		*(p++) = '-';
	}

	while((c = *(--p))) {
		write_character(c, buffer);
	}
}

void write_number_kfloat(kfloat number, kuint flags, kuint8** buffer) {
	write_string("?f", buffer);
}

void write_number_kfloat32(kfloat32 number, kuint flags, kuint8** buffer) {
	write_string("?f32", buffer);
}

void write_number_kfloat64(kfloat64 number, kuint flags, kuint8** buffer) {
	write_string("?f64", buffer);
}
