/*
 * Copyright (C) 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "kinternal.h"
#include "kinterface.h"

kuint8* kernel_info[] = {
	(kuint8*)"kernel_name=RJK",
	(kuint8*)"kernel_version=0.0.0.0",
	(kuint8*)"kernel_contact=Robert Fitzsimons <robfitz@273k.net>",
	(kuint8*)"kernel_license=GPL",
	KNULL,
};

struct kinterface _kinterface __attribute__ ((aligned(KPAGESIZE))) = {
	/*
	 * Kernel Interface
	 */

	kinterface_check_version,
	kinterface_kernel_info,

	/*
	 * Kernel Interface: Modules
	 */

	kmodule_get,
	kmodule_free,

	/*
	 * Kernel Interface: Atomic Operation's
	 */

	katomic_get,
	katomic_set,

	katomic_inc,
	katomic_dec,
	katomic_add,
	katomic_sub,

	katomic_dec_and_test,

	katomic_compare,
	katomic_compare_and_set_if_equal,

	katomic_bit_get,
	katomic_bit_set,
	katomic_bit_reset,

	katomic_bit_test_and_set,
	katomic_bit_test_and_reset,

	/*
	 * Kernel Interface: Debugging
	 */

	kdebug,

	/*
	 * Kernel Interface: IRQ's
	 */

	kirq_assign_irq,
	kirq_unassign_irq,
	kirq_enable_irq,
	kirq_disable_irq,
	kirq_enable_all,
	kirq_disable_all,

	/*
	 * Kernel Interface: Input Output Port's
	 */

	kioport_in_kuint,
	kioport_in_kuint8,
	kioport_in_kuint16,
	kioport_in_kuint32,
	kioport_in_kuint64,

	kioport_out_kuint,
	kioport_out_kuint8,
	kioport_out_kuint16,
	kioport_out_kuint32,
	kioport_out_kuint64,

	kioport_in_kuint_port,
	kioport_in_kuint8_port,
	kioport_in_kuint16_port,
	kioport_in_kuint32_port,
	kioport_in_kuint64_port,

	kioport_out_kuint_port,
	kioport_out_kuint8_port,
	kioport_out_kuint16_port,
	kioport_out_kuint32_port,
	kioport_out_kuint64_port,

	/*
	 * Kernel Interface: Spinlock
	 */

	kspinlock_lock,
	kspinlock_unlock,
	kspinlock_read_lock,
	kspinlock_read_unlock,
	kspinlock_write_lock,
	kspinlock_write_unlock,
	kspinlock_lock_irqsave,
	kspinlock_unlock_irqrestore,

	/*
	 * Kernel Interface: Mutex
	 */

	kmutex_init,
	kmutex_lock,
	kmutex_unlock,

	/*
	 * Kernel Interface: Thread's
	 */

	kthread_create,

	kthread_current,

	kthread_yield,

	kthread_kill,
	kthread_suspend,
	kthread_resume,
	kthread_sleep,

	kthread_priority_set,
	kthread_priority_get,
	kthread_key_set,
	kthread_key_get,

	/*
	 * Kernel Interface: Time
	 */

	ktime_get,
	ktime_set,

	/*
	 * Kernel Interface: Timer's
	 */

	ktimer_add,
	ktimer_remove,

	/*
	 * Kernel Interface: Liner Memory
	 */

	kmemory_linear_page_allocate,
	kmemory_linear_page_unallocate,

	kmemory_linear_page_map,
	kmemory_linear_page_unmap,

	kmemory_linear_read_kuint,
	kmemory_linear_read_kuint8,
	kmemory_linear_read_kuint16,
	kmemory_linear_read_kuint32,
	kmemory_linear_read_kuint64,

	kmemory_linear_write_kuint,
	kmemory_linear_write_kuint8,
	kmemory_linear_write_kuint16,
	kmemory_linear_write_kuint32,
	kmemory_linear_write_kuint64,

	kmemory_linear_read_kuint_array,
	kmemory_linear_read_kuint8_array,
	kmemory_linear_read_kuint16_array,
	kmemory_linear_read_kuint32_array,
	kmemory_linear_read_kuint64_array,

	kmemory_linear_write_kuint_array,
	kmemory_linear_write_kuint8_array,
	kmemory_linear_write_kuint16_array,
	kmemory_linear_write_kuint32_array,
	kmemory_linear_write_kuint64_array,

	/*
	 * Kernel Interface: Virtual Memory
	 */

	kmemory_virtual_page_allocate,
	kmemory_virtual_page_unallocate,

	kmemory_virtual_copy,
	kmemory_virtual_fill,

	/*
	 * Kernel Interface: Processor
	 */

	kprocessor_save_flags,
	kprocessor_restore_flags,

	kprocessor_byteswap_kuint,
	kprocessor_byteswap_kuint8,
	kprocessor_byteswap_kuint16,
	kprocessor_byteswap_kuint32,
	kprocessor_byteswap_kuint64,

};

kfunction kuint kinterface_check_version(kuint interface_version, kuint kernel_version) {
	if(
		(interface_version == INTERFACE_VERSION)
		&& (kernel_version == KERNEL_VERSION)
	) {
		return 1;
	}

	return 0;
}

kfunction kuint8** kinterface_kernel_info() {
	return kernel_info;
}

