/*
 * Copyright (C) 2000 Robert Fitzsimons
 */

#include "kinterface.h"

struct interrupt_handler {
	kuint (*handler)(kuint number);
};

struct interrupt_handler interrupt_handler[48];

kfunction void kirq_init() {
	extern kuint16 idt[];
	extern kuint32 isr[];
	kuint i = 0;

	for(i = 0; i < 48; i++) {
		idt[(i * 4) + 0] = (kuint16)((kuint)&isr[(i * 4)]);
		idt[(i * 4) + 3] = (kuint16)((kuint)&isr[(i * 4)] >> 16);
	}
}

void handle_exception(
	kuint number,
	kuint edi,
	kuint esi,
	kuint ebp,
	kuint esp,
	kuint ebx,
	kuint edx,
	kuint ecx,
	kuint eax,
	kuint ec,
	kuint eip,
	kuint cs,
	kuint eflags
) {
	kuint cr0 = 0;
	kuint cr1 = 0;
	kuint cr2 = 0;
	kuint cr3 = 0;
	kuint cr4 = 0;

	asm volatile (
		"\tmovl %%cr0, %0\n"
//		"\tmovl %%cr1, %1\n"
		"\tmovl %%cr2, %2\n"
		"\tmovl %%cr3, %3\n"
		"\tmovl %%cr4, %4\n"
		: "=r" (cr0), "=r" (cr1), "=r" (cr2), "=r" (cr3), "=r" (cr4)
	);

	kdebug("Exception=%u  Error Code=%x\n", number, ec);
	kdebug("cs=%x  eip=%x  eflags=%x\n", cs, eip, eflags);
	kdebug("eax=%x  ebx=%x  ecx=%x  edx=%x\n", eax, ebx, ecx, edx);
	kdebug("edi=%x  esi=%x  ebp=%x  esp=%x\n", edi, esi, ebp, esp);
	kdebug("cr0=%x  cr1=%x  cr2=%x  cr3=%x cr4=%x\n", cr0, cr1, cr2, cr3, cr4);

	asm volatile (
		"\tcli\n"
		"0:\tjmp 0b\n"
	);
}

kuint handle_irq(kuint number) {
	if(interrupt_handler[number].handler != knull) {
		interrupt_handler[number].handler(number);
	} else {
		kdebug("IRQ %u\n", number);
	}
	return 0x00000020; // Sets up %eax so we can send EOI to PIC1 & PIC2
}

kfunction kirq kirq_assign(kuint number, kuint (*handler)(kuint number)) {
	kirq irq = 0;
	if(number < 16) {
		if(interrupt_handler[number].handler == knull) {
			interrupt_handler[number].handler = handler;
			kirq_enable(number);
			irq = number + 0x0100;
		}
	}
	return irq;
}

kfunction void kirq_unassign(kirq irq) {
	irq -= 0x0100;
	if(irq < 16) {
		interrupt_handler[irq].handler = knull;
		kirq_disable(irq);
	}
}

kfunction void kirq_enable(kuint number) {
	kuint value;
	if(number < 8) {
		value = kioport_in_kuint8(0x21);
		kioport_out_kuint8(0x21, (value & ~(1 << number)));
	} else if(number < 16) {
		value = kioport_in_kuint8(0xA1);
		kioport_out_kuint8(0xA1, (value & ~(1 << (number - 8))));
		value = kioport_in_kuint8(0x21);
		kioport_out_kuint8(0x21, (value & ~(1 << 2)));
	}
}

kfunction void kirq_disable(kuint number) {
	kuint value;
	if(number < 8) {
		value = kioport_in_kuint8(0x21);
		kioport_out_kuint8(0x21, (value | (1 << number)));
	} else if(number < 16) {
		value = kioport_in_kuint8(0xA1);
		kioport_out_kuint8(0xA1, (value | (1 << (number - 8))));
	}
}

kfunction void kirq_enable_all() {
	asm volatile ("sti" : : : "cc");
}

kfunction void kirq_disable_all() {
	asm volatile ("cli" : : : "cc");
}

