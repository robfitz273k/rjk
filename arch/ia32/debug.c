/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "kinternal.h"

#define UNSIGNED 0x00000001
#define HEX      0x00000010
#define SIZE_32  0x00000100
#define SIZE_64  0x00000200
#define MINUS    0x00001000
#define ADDRESS  0x00002000

static kuint8* digits = "0123456789ABCDEF";
static kuint8* blank_line = " \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07";
kuint video_memory;
kuint video_next;
volatile kuint kdebug_spinlock;

void write_character(kuint c);
void write_string(kuint8* s);
void write_number_kuint(kuint number, kuint flags);
void write_number_kuint32(kuint32 number, kuint flags);
void write_number_kuint64(kuint64 number, kuint flags);
void write_number_kfloat(kfloat number, kuint flags);
void write_number_kfloat32(kfloat32 number, kuint flags);
void write_number_kfloat64(kfloat64 number, kuint flags);

kfunction void kdebug_init() {
	kuint i;

	video_memory = 0x000B8000;

	kmemory_linear_page_map(video_memory, ((80 * 25 * 2) / KPAGESIZE) + 1); 

	for(i = 0; i < (80 * 25 * 2); i += (80 * 2)) {
		kmemory_virtual_copy((void*)(video_memory + i), (void*)blank_line, (80 * 2));
	}
}

kfunction void kdebug(kuint8* format, ...) {
	kuint flags;
	kuint c;
	va_list args;

	kspinlock_lock_irqsave(&kdebug_spinlock, &flags);

	va_start(args, format);
	while((c = *(format++)) != '\0') {
		if(c != '%') {
			write_character(c);
			continue;
		} else if((c = *(format++)) != '\0') {
			void* t;
			kuint flags = 0;
			switch(c) {
				case '%':
					write_character(c);
					continue;
				case 'c':
					c = (kuint8)va_arg(args, kuint8);
					write_character(c);
					continue;
				case 's':
					t = (void*)va_arg(args, kuint8*);
					if(t) {
						write_string((kuint8*)t);
					} else {
						write_string("(nil)");
					}
					continue;
				case 'p':
					t = (void*)va_arg(args, void*);
					if(t) {
						write_number_kuint((kuint)t, HEX | UNSIGNED | ADDRESS);
					} else {
						write_string("(null)");
					}
					continue;
				case 'i':
					flags |= SIZE_32;
					c = *(format++); 
					break;
				case 'l':
					flags |= SIZE_64;
					c = *(format++); 
					break;
			}
			switch(c) {
				case 'a':
					flags |= ADDRESS;
				case 'x':
					flags |= HEX;
				case 'u':
					flags |= UNSIGNED;
				case 'd':
					if((flags & SIZE_32) == SIZE_32) {
						write_number_kuint32((kuint32)va_arg(args, kuint32), flags);
					} else if((flags & SIZE_64) == SIZE_64) {
						write_number_kuint64((kuint64)va_arg(args, kuint64), flags);
					} else {
						write_number_kuint((kuint)va_arg(args, kuint), flags);
					}
					continue;
				case 'e':
				case 'f':
					if((flags & SIZE_32) == SIZE_32) {
						write_number_kfloat32((kfloat32)va_arg(args, kfloat32), flags);
					} else if((flags & SIZE_64) == SIZE_64) {
						write_number_kfloat64((kfloat64)va_arg(args, kfloat64), flags);
					} else {
						write_number_kfloat((kfloat)va_arg(args, kfloat), flags);
					}
					continue;
			}
		}
		write_string("<format error>");
		break;
	}
	va_end(args);

	kspinlock_unlock_irqrestore(&kdebug_spinlock, &flags);

	return;
}

void write_character(kuint c) {
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

void write_string(kuint8* s) {
	kuint c;

	while((c = *(s++)) != '\0') {
		write_character(c);
	}
}

void write_number_kuint(kuint number, kuint flags) {
	if(sizeof(kuint) == 4) {
		write_number_kuint32((kuint32)number, flags);
	} else {
		write_number_kuint64((kuint64)number, flags);
	}
}

void write_number_kuint32(kuint32 number, kuint flags) {
	kuint c;
	kuint8 buffer[16];
	kuint8* p = buffer;
	kuint32 base = 10;

	if((flags & HEX) == HEX) {
		base = 16;
	} else if(((flags & UNSIGNED) != UNSIGNED) && ((number & 0x80000000ul) > 0)) {
		number = (~number) + 1;
		flags |= MINUS;
	}

	*(p++) = '\0';

	do {
		*(p++) = digits[number % base];
	} while((number /= base) > 0);

	if((flags & ADDRESS) == ADDRESS) {
		*(p++) = 'x';
		*(p++) = '0';
	} else if((flags & MINUS) == MINUS) {
		*(p++) = '-';
	}

	while((c = *(--p)) != '\0') {
		write_character(c);
	}
}

void write_number_kuint64(kuint64 number, kuint flags) {
	kuint c;
	kuint8 buffer[32];
	kuint8* p = buffer;
	kuint64 base = 10ull;

	if((flags & HEX) == HEX) {
		base = 16ull;
	} else if(((flags & UNSIGNED) != UNSIGNED) && ((number & 0x8000000000000000ull) > 0)) {
		number = (~number) + 1ull;
		flags |= MINUS;
	}

	*(p++) = '\0';

	do {
		*(p++) = digits[number % base];
	} while((number /= base) > 0ull);

	if((flags & ADDRESS) == ADDRESS) {
		*(p++) = 'x';
		*(p++) = '0';
	} else if((flags & MINUS) == MINUS) {
		*(p++) = '-';
	}

	while((c = *(--p)) != '\0') {
		write_character(c);
	}
}

void write_number_kfloat(kfloat number, kuint flags) {
	write_string("?f");
}

void write_number_kfloat32(kfloat32 number, kuint flags) {
	write_string("?f32");
}

void write_number_kfloat64(kfloat64 number, kuint flags) {
	write_string("?f64");
}

