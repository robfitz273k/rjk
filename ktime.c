/*
 * Copyright (C) 2000 Robert Fitzsimons
 */

#include "kinterface.h"

#define get_and_convert_bcd_value() \
	(((value = kioport_in_kuint8(0x71)) & 0x0F) + ((value >> 4) * 10))

volatile ktime current_time;

void read_rtc(volatile ktime* t) {
	kuint value = 0;
	kuint year = 0;
	kuint month = 0;
	kuint day = 0;
	kuint hour = 0;
	kuint minute = 0;
	kuint second = 0;

	do {
		kioport_out_kuint8(0x70, 0x0A);
		value = kioport_in_kuint8(0x71);
	} while((value & 0x80) > 0);

	kioport_out_kuint8(0x70, 0x32); // Century
	year = (100 * get_and_convert_bcd_value());
	kioport_out_kuint8(0x70, 0x09); // Year
	year += get_and_convert_bcd_value();
	kioport_out_kuint8(0x70, 0x08); // Month
	month = get_and_convert_bcd_value();
	kioport_out_kuint8(0x70, 0x07); // Day
	day = get_and_convert_bcd_value();
	kioport_out_kuint8(0x70, 0x04); // Hour
	hour = get_and_convert_bcd_value();
	kioport_out_kuint8(0x70, 0x02); // Minute
	minute = get_and_convert_bcd_value();
	kioport_out_kuint8(0x70, 0x00); // Second
	second = get_and_convert_bcd_value();

	if(0 >= (kint)(month -= 2)) {
		month += 12;
		year -= 1;
	}
	t->second = ((kint64)(((((year / 4) - (year / 100) + (year / 400) + ((367 * month) / 12) + day) + (year * 365) - 719499) * 24 + hour) * 60 + minute) * 60 + second);
	t->nanosecond = 0;
}

kuint rtc_irq_handler(kuint number) {
	kuint8 value = 0;

	kioport_out_kuint8(0x70, 0x0C);
	value = kioport_in_kuint8(0x71);

	if((value & 0x10) > 0) {
		read_rtc(&current_time);
	} else if((value & 0x40) > 0) {
		current_time.nanosecond += 1000;
	}

	return 0;
}

void ktime_init() {
	kuint8 value = 0;

	kioport_out_kuint8(0x70, 0x0A);
	value = kioport_in_kuint8(0x71);
	kioport_out_kuint8(0x70, 0x0A);
	kioport_out_kuint8(0x71, ((value & 0xF0) | 0x06)); // 1024 interrupts per second
	kioport_long_delay();

	kioport_out_kuint8(0x70, 0x0B);
	value = kioport_in_kuint8(0x71);
	kioport_out_kuint8(0x70, 0x0B);
	kioport_out_kuint8(0x71, (value | 0x52)); // 24 hour mode, update interrupt, periodic interrupt
	kioport_long_delay();

	read_rtc(&current_time);

	kirq_assign(8, &rtc_irq_handler);
}

kfunction void ktime_get(ktime* t) {
	t->second = current_time.second;
	t->nanosecond = current_time.nanosecond;
}

kfunction void ktime_set(ktime* t) {
	current_time.second = t->second;
	current_time.nanosecond = t->nanosecond;
}

