/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

struct timer {
	struct timer* next;
	kuint timer;
	void (*function)(void* data);
	void* data;
	kint64 second;
	kint32 nanosecond;
};

void timer_thread(void* data);

volatile kuint ktimer_spinlock;
struct timer timer_list;
kuint timer_count;

void ktimer_init() {
	kthread_create(timer_thread, KNULL, 0);
}

kfunction kuint ktimer_add(void (*function)(void* data), void* data, kuint64 second, kuint32 nanosecond) {
	kuint flags;
	struct timer* p;
	struct timer* c;
	struct timer* n;
	kuint timer = 0;
	ktime time;

	kspinlock_lock_irqsave(&ktimer_spinlock, &flags);

	ktime_get(&time);
	time.second += second;
	time.nanosecond += nanosecond;

	p = &timer_list;
	c = p->next;

	while(c) {
		if((time.second < c->second) || ((time.second == c->second) && (time.nanosecond <= c->nanosecond))) {
			break;
		} else {
			p = c;
			c = c->next;
		}
	}

	if((n = kmemory_virtual_page_allocate(1, 0))) {
		p->next = n;
		n->next = c;

		timer = n->timer = ++timer_count;
		n->function = function;
		n->data = data;
		n->second = time.second;
		n->nanosecond = time.nanosecond;
	}

	kspinlock_unlock_irqrestore(&ktimer_spinlock, &flags);

	return timer;
}

kfunction void ktimer_remove(kuint timer) {
}

void timer_thread(void* data) {
	kuint flags;
	struct timer* p;
	struct timer* c;
	ktime time;

	while(1) {
		kspinlock_lock_irqsave(&ktimer_spinlock, &flags);

		p = &timer_list;
		c = p->next;

		ktime_get(&time);

		if(
			(c)
			&& (
				(c->second < time.second)
				|| ((c->second == time.second) && (c->nanosecond <= time.nanosecond))
			)
		) {
			void (*function)(void* data) = c->function;
			void* data = c->data;

			p->next = c->next;
			kmemory_virtual_page_unallocate(c);

			kspinlock_unlock_irqrestore(&ktimer_spinlock, &flags);

			function(data);
		} else {
			kspinlock_unlock_irqrestore(&ktimer_spinlock, &flags);
			kthread_yield();
		}
	}
}

