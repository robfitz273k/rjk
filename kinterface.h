/*
 * Copyright (C) 2000 Robert Fitzsimons
 */

#ifndef _kinterface_h
#define _kinterface_h

#include "kimplementation.h"

/*
 * Kernel Interface: Data Type's
 *
 * This interface defines most of the public data types, data
 * structures, and values used throughout the Kernel Interface.
 */

/*
 * The following data types are defined by this interface:
 *
 * Name         Size                Type
 * ----         ----                ----
 * kint         platform specific   signed two's complement integer
 * kint8        8  bits             signed two's complement integer
 * kint16       16 bits             signed two's complement integer
 * kint32       32 bits             signed two's complement integer
 * kint64       64 bits             signed two's complement integer
 * kuint        platform specific   unsigned integer
 * kuint8       8  bits             unsigned integer
 * kuint16      16 bits             unsigned integer
 * kuint32      32 bits             unsigned integer
 * kuint64      64 bits             unsigned integer
 * kfloat       platfrom specific   IEEE 754 floating point number
 * kfloat32     32 bits             IEEE 754 floating point number
 * kfloat64     64 bits             IEEE 754 floating point number
 * kinterrupt   platform specific   kernel specific
 * kmonitor     platform specific   kernel specific
 * kthread      platform specific   kernel specific
 * ktimer       platform specific   kernel specific
 */

/*
 * The following data structures are defined by this interface:
 */

typedef struct ktime {
	kint64 second;
	kint32 nanosecond;
} ktime;

/*
 * The following values are defined by this interface:
 *
 * Name         Value
 * ----         -----
 * ktrue        value when an expression elevates as true
 * kfalse       value when an expression elevates as false
 * knull        value of a null pointer
 * kpage_size   size of a system page
 */

/*
 * Kernel Interface: Variable Argument's
 */

/*
 * Kernel Interface: Atomic Operation's
 */

kfunction kuint katomic_get(kuint* atomic);

kfunction kint katomic_compare(kuint* atomic, kuint compare);

kfunction void katomic_set(kuint* atomic, kuint value);
kfunction void katomic_add(kuint* atomic, kuint value);
kfunction void katomic_sub(kuint* atomic, kuint value);
kfunction void katomic_inc(kuint* atomic);
kfunction void katomic_dec(kuint* atomic);

kfunction kint katomic_set_if_equal(kuint* atomic, kuint compare, kuint value);
kfunction kint katomic_add_if_equal(kuint* atomic, kuint compare, kuint value);
kfunction kint katomic_sub_if_equal(kuint* atomic, kuint compare, kuint value);
kfunction kint katomic_inc_if_equal(kuint* atomic, kuint compare);
kfunction kint katomic_dec_if_equal(kuint* atomic, kuint compare);

kfunction kint katomic_bit_get(kuint* atomic, kuint bit);

kfunction void katomic_bit_set(kuint* atomic, kuint bit);
kfunction void katomic_bit_reset(kuint* atomic, kuint bit);

kfunction kint katomic_bit_test_and_set(kuint* atomic, kuint bit);
kfunction kint katomic_bit_test_and_reset(kuint* atomic, kuint bit);

/*
 * Kernel Interface: Debugging
 */

kfunction void kdebug(kuint8* format, ...);

/*
 * Kernel Interface: IRQ's
 */

kfunction kirq kirq_assign(kuint number, kuint (*handler)(kuint number));
kfunction void kirq_unassing(kirq irq);
kfunction void kirq_enable(kuint number);
kfunction void kirq_disable(kuint number);
kfunction void kirq_enable_all();
kfunction void kirq_disable_all();

/*
 * Kernel Interface: Input Output Port's
 */

kfunction void kioport_long_delay();

kfunction kuint kioport_in_kuint(kuint port);
kfunction kuint8 kioport_in_kuint8(kuint port);
kfunction kuint16 kioport_in_kuint16(kuint port);
kfunction kuint32 kioport_in_kuint32(kuint port);
kfunction kuint64 kioport_in_kuint64(kuint port);

kfunction void kioport_out_kuint(kuint port, kuint value);
kfunction void kioport_out_kuint8(kuint port, kuint8 value);
kfunction void kioport_out_kuint16(kuint port, kuint16 value);
kfunction void kioport_out_kuint32(kuint port, kuint32 value);
kfunction void kioport_out_kuint64(kuint port, kuint64 value);

kfunction kuint kioport_in_kuint_port(kuint port, kuint* array, kuint offset, kuint length);
kfunction kuint kioport_in_kuint8_port(kuint port, kuint8* array, kuint offset, kuint length);
kfunction kuint kioport_in_kuint16_port(kuint port, kuint16* array, kuint offset, kuint length);
kfunction kuint kioport_in_kuint32_port(kuint port, kuint32* array, kuint offset, kuint length);
kfunction kuint kioport_in_kuint64_port(kuint port, kuint64* array, kuint offset, kuint length);

kfunction kuint kioport_out_kuint_port(kuint port, kuint* array, kuint offset, kuint length);
kfunction kuint kioport_out_kuint8_port(kuint port, kuint8* array, kuint offset, kuint length);
kfunction kuint kioport_out_kuint16_port(kuint port, kuint16* array, kuint offset, kuint length);
kfunction kuint kioport_out_kuint32_port(kuint port, kuint32* array, kuint offset, kuint length);
kfunction kuint kioport_out_kuint64_port(kuint port, kuint64* array, kuint offset, kuint length);

/*
 * Kernel Interface: Inter Thread Communication
 */

/*
 * Kernel Interface: Monitor's
 */

kfunction kmonitor kmonitor_create();
kfunction void kmonitor_delete(kmonitor monitor);

kfunction void kmonitor_lock(kmonitor monitor, kthread thread);
kfunction void kmonitor_unlock(kmonitor monitor, kthread thread);
kfunction void kmonitor_wait(kmonitor monitor, kthread thread);
kfunction void kmonitor_wait_time(kmonitor monitor, kthread thread, kint64 second, kint32 nanosecond);
kfunction void kmonitor_notify(kmonitor monitor, kthread thread);
kfunction void kmonitor_notify_all(kmonitor monitor, kthread thread);

/*
 * Kernel Interface: Thread's
 */

kfunction kthread kthread_create(void (*function)(void), kint priorty, void* key);
kfunction void kthread_delete(kthread thread);

kfunction kthread kthread_current();

kfunction void kthread_yield();
kfunction void kthread_sleep(kint64 second, kint32 nanosecond);

kfunction void kthread_priority_set(kthread thread, kint priority);
kfunction kint kthread_priority_get(kthread thread);
kfunction void* kthread_key_set(kthread thread, void* value);
kfunction void* kthread_key_get(kthread thread);

/*
 * Kernel Interface: Time
 */

kfunction void ktime_get(ktime* time);
kfunction void ktime_set(ktime* time);

/*
 * Kernel Interface: Timer's
 */

kfunction ktimer ktimer_add(void (*function)(kint data), kint data, kint64 second, kint32 nanosecond);
kfunction void ktimer_remove(ktimer timer);

/*
 * Kernel Interface: Memory
 */

kfunction kuint kmemory_linear_page_allocate(kuint address, kuint count);
kfunction void kmemory_linear_page_unallocate(kuint address, kuint count);

kfunction kuint kmemory_linear_read_kuint(kuint address);
kfunction kuint8 kmemory_linear_read_kuint8(kuint address);
kfunction kuint16 kmemory_linear_read_kuint16(kuint address);
kfunction kuint32 kmemory_linear_read_kuint32(kuint address);
kfunction kuint64 kmemory_linear_read_kuint64(kuint address);
kfunction void kmemory_linear_write_kuint(kuint address, kuint value);
kfunction void kmemory_linear_write_kuint8(kuint address, kuint8 value);
kfunction void kmemory_linear_write_kuint16(kuint address, kuint16 value);
kfunction void kmemory_linear_write_kuint32(kuint address, kuint32 value);
kfunction void kmemory_linear_write_kuint64(kuint address, kuint64 value);
kfunction kuint kmemory_linear_read_kuint_array(kuint address, kuint* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_read_kuint8_array(kuint address, kuint8* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_read_kuint16_array(kuint address, kuint16* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_read_kuint32_array(kuint address, kuint32* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_read_kuint64_array(kuint address, kuint64* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_write_kuint_array(kuint address, kuint* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_write_kuint8_array(kuint address, kuint8* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_write_kuint16_array(kuint address, kuint16* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_write_kuint32_array(kuint address, kuint32* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_write_kuint64_array(kuint address, kuint64* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_read_kuint_address(kuint address, kuint* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_read_kuint8_address(kuint address, kuint8* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_read_kuint16_address(kuint address, kuint16* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_read_kuint32_address(kuint address, kuint32* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_read_kuint64_address(kuint address, kuint64* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_write_kuint_address(kuint address, kuint* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_write_kuint8_address(kuint address, kuint8* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_write_kuint16_address(kuint address, kuint16* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_write_kuint32_address(kuint address, kuint32* array, kuint offset, kuint length);
kfunction kuint kmemory_linear_write_kuint64_address(kuint address, kuint64* array, kuint offset, kuint length);

kfunction void* kmemory_virtual_page_allocate(kuint count);
kfunction void kmemory_virtual_page_unallocate(void* pointer);

kfunction void kmemory_virtual_copy(void* destination, void* source, kuint size);
kfunction void kmemory_virtual_fill(void* pointer, kuint size, kuint8 value);

#endif /* _kinterface_h */

