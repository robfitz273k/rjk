/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "kinternal.h"

#define bcd_to_binary(v) ({ \
	kuint8 __v = (v); \
	((__v & 0xF) + ((__v >> 4) * 10)); \
})

void ktimer_check(ktime* time);

ktime current_time;
kuint thread_ticks;
kuint time_ticks;
volatile kuint ktime_spinlock;

void read_rtc(volatile ktime* t) {
	kuint value;
	kuint year;
	kuint month;
	kuint day;
	kuint hour;
	kuint minute;
	kuint second;

	do {
		kioport_out_kuint8(0x70, 0x0A);
		value = kioport_in_kuint8(0x71);
	} while(value & 0x80);

	kioport_out_kuint8(0x70, 0x32); /* Century */
	year = (100 * bcd_to_binary(kioport_in_kuint8(0x71)));
	kioport_out_kuint8(0x70, 0x09); /* Year */
	year += bcd_to_binary(kioport_in_kuint8(0x71));
	kioport_out_kuint8(0x70, 0x08); /* Month */
	month = bcd_to_binary(kioport_in_kuint8(0x71));
	kioport_out_kuint8(0x70, 0x07); /* Day */
	day = bcd_to_binary(kioport_in_kuint8(0x71));
	kioport_out_kuint8(0x70, 0x04); /* Hour */
	hour = bcd_to_binary(kioport_in_kuint8(0x71));
	kioport_out_kuint8(0x70, 0x02); /* Minute */
	minute = bcd_to_binary(kioport_in_kuint8(0x71));
	kioport_out_kuint8(0x70, 0x00); /* Second */
	second = bcd_to_binary(kioport_in_kuint8(0x71));

	if(0 >= (kint)(month -= 2)) {
		month += 12;
		year -= 1;
	}

	t->second = ((kint64)(((((year / 4) - (year / 100) + (year / 400) + ((367 * month) / 12) + day) + (year * 365) - 719499) * 24 + hour) * 60 + minute) * 60 + second);
	t->nanosecond = 0;
}

kuint rtc_irq_handler(kuint number) {
	kuint8 value;

	kioport_out_kuint8(0x70, 0x0C);
	value = kioport_in_kuint8(0x71); /* Acknowledge interrupt */

	kspinlock_lock(&ktime_spinlock);

	if(value & 0x40) { /* Periodic interrupt */
		current_time.nanosecond += (1000000000 / 1024);
	}

	if(value & 0x10) { /* Update ended interrupt */
		if(time_ticks--) {
			current_time.second++;
			current_time.nanosecond = 0;
		} else {
			read_rtc(&current_time);
			time_ticks = 37;
		}
	}

	kspinlock_unlock(&ktime_spinlock);

	if(!(thread_ticks--)) {
		thread_ticks = 11;
		return 1;
	}

	return 0;
}

void ktime_init() {
	kuint8 value;

	do {
		kioport_out_kuint8(0x70, 0x0A);
		value = kioport_in_kuint8(0x71);
	} while(value & 0x80);

	kioport_out_kuint8(0x70, 0x0A);
	value = kioport_in_kuint8(0x71);
	kioport_out_kuint8(0x70, 0x0A);
	kioport_out_kuint8(0x71, ((value & 0xF0) | 0x06)); /* 1024 interrupts per second */

	kioport_out_kuint8(0x70, 0x0B);
	value = kioport_in_kuint8(0x71);
	kioport_out_kuint8(0x70, 0x0B);
	kioport_out_kuint8(0x71, ((value & 0x0F) | 0x52)); /* periodic interrupt, update interrupt, 24 hour mode */

	kioport_out_kuint8(0x70, 0x0C);
	value = kioport_in_kuint8(0x71); /* acknowledge interrupt */

	read_rtc(&current_time);

	kirq_assign_irq(8, rtc_irq_handler);
}

kfunction void ktime_get(ktime* t) {
	kuint flags;

	kspinlock_lock_irqsave(&ktime_spinlock, &flags);

	t->second = current_time.second;
	t->nanosecond = current_time.nanosecond;

	kspinlock_unlock_irqrestore(&ktime_spinlock, &flags);
}

kfunction void ktime_set(ktime* t) {
	kuint flags;

	kspinlock_lock_irqsave(&ktime_spinlock, &flags);

	current_time.second = t->second;
	current_time.nanosecond = t->nanosecond;

	kspinlock_unlock_irqrestore(&ktime_spinlock, &flags);
}

