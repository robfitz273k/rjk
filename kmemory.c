/*
 * Copyright (C) 2000 Robert Fitzsimons
 */

#include "kinterface.h"

extern void* _start;
extern void* _end;

#define ALLOCATED   (0x00000001)
#define READWRITE   (0x00000002)

#define tbl_invalidate() \
	asm volatile ( \
		"\tmovl $page_table_directory, %%eax\n" \
		"\tmovl %%eax, %%cr3\n" \
		: \
		: \
		: "eax" \
	)

kuint page_table_directory[1024] __attribute__ ((aligned(kpage_size), section("block")));
kuint page_table_0[1024] __attribute__ ((aligned(kpage_size), section("block")));
kuint linear_page_count;
kuint linear_free_count;
kuint next_page;

#define pte(la) (((kuint*)(page_table_directory[((la) >> 22)] & 0xFFFFF000))[(((la) << 10) >> 22)])

void kmemory_print_page_table(kuint table) {
	kuint i = 0;
	kuint* pt = (kuint*)(page_table_directory[table] & 0xFFFFF000);
	kuint pb = table * 1024;

	for(i = 0; i < 1025; i++) {
		if(page_table_directory[i] != 0) {
			kdebug("%u : %x\n", i, page_table_directory[i]);
		}
	}

	kdebug("Table %u : %x\n", table, page_table_directory[table]);

	table <<= 12;

	for(i = 0; i < 1024; i++) {
		if(pt[i] != 0) {
			kdebug("%u : %a -> %x\n", (pb + i), (table | (i << 12)), pt[i]);
		}
	}
}

void kmemory_page_table_set(kuint index, kuint address) {
	page_table_directory[index] = ((address & 0xFFFFF000) | ALLOCATED | READWRITE);
	tbl_invalidate();
}

void kmemory_setup(kuint mem_lower, kuint mem_upper) {
	linear_page_count = linear_free_count = (mem_upper / kpage_size);

	kmemory_page_table_set(0, (kuint)&page_table_0); /* 0 - 4 Mb */

	kmemory_linear_page_allocate(((kuint)&_start & 0xFFFFF000), ((((kuint)&_end - (kuint)&_start) / kpage_size) + 1)); /* kernel */
	kmemory_linear_page_allocate(0x00001000, 5); /* init thread */

	next_page = 0x00200000;
}

kfunction void kmemory_init() {
}

kfunction kuint kmemory_linear_page_allocate(kuint address, kuint count) {
	kuint la = (address & 0xFFFFF000);
	kuint i = 0;
	for(i = 0; i < count; i++) {
		pte(la) = (la | ALLOCATED | READWRITE);
		la += kpage_size;
		linear_free_count--;
	}
	return (address & 0xFFFFF000);
	tbl_invalidate();
}

kfunction void kmemory_linear_page_unallocate(kuint address, kuint count) {
	kuint la = (address & 0xFFFFF000);
	kuint i = 0;
	for(i = 0; i < count; i++) {
		pte(la) = 0;
		la += kpage_size;
		linear_free_count++;
	}
	tbl_invalidate();
	return;
}

kfunction kuint kmemory_linear_read_kuint(kuint address) {
	return *(kuint*)address;
}
kfunction kuint8 kmemory_linear_read_kuint8(kuint address) {
	return *(kuint8*)address;
}
kfunction kuint16 kmemory_linear_read_kuint16(kuint address) {
	return *(kuint16*)address;
}
kfunction kuint32 kmemory_linear_read_kuint32(kuint address) {
	return *(kuint32*)address;
}
kfunction kuint64 kmemory_linear_read_kuint64(kuint address) {
	return *(kuint64*)address;
}

kfunction void kmemory_linear_write_kuint(kuint address, kuint value) {
	*(kuint*)address = value;
}
kfunction void kmemory_linear_write_kuint8(kuint address, kuint8 value) {
	*(kuint8*)address = value;
}
kfunction void kmemory_linear_write_kuint16(kuint address, kuint16 value) {
	*(kuint16*)address = value;
}
kfunction void kmemory_linear_write_kuint32(kuint address, kuint32 value) {
	*(kuint32*)address = value;
}
kfunction void kmemory_linear_write_kuint64(kuint address, kuint64 value) {
	*(kuint64*)address = value;
}

kfunction kuint kmemory_linear_read_kuint_array(kuint address, kuint* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint* p = (kuint*)address;
	array += offset;
	while(i++ < length) {
		*array++ = *p++;
	}
	return length;
}
kfunction kuint kmemory_linear_read_kuint8_array(kuint address, kuint8* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint8* p = (kuint8*)address;
	array += offset;
	while(i++ < length) {
		*array++ = *p++;
	}
	return length;
}
kfunction kuint kmemory_linear_read_kuint16_array(kuint address, kuint16* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint16* p = (kuint16*)address;
	array += offset;
	while(i++ < length) {
		*array++ = *p++;
	}
	return length;
}
kfunction kuint kmemory_linear_read_kuint32_array(kuint address, kuint32* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint32* p = (kuint32*)address;
	array += offset;
	while(i++ < length) {
		*array++ = *p++;
	}
	return length;
}
kfunction kuint kmemory_linear_read_kuint64_array(kuint address, kuint64* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint64* p = (kuint64*)address;
	array += offset;
	while(i++ < length) {
		*array++ = *p++;
	}
	return length;
}

kfunction kuint kmemory_linear_write_kuint_array(kuint address, kuint* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint* p = (kuint*)address;
	array += offset;
	while(i++ < length) {
		*p++ = *array++;
	}
	return length;
}
kfunction kuint kmemory_linear_write_kuint8_array(kuint address, kuint8* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint8* p = (kuint8*)address;
	array += offset;
	while(i++ < length) {
		*p++ = *array++;
	}
	return length;
}
kfunction kuint kmemory_linear_write_kuint16_array(kuint address, kuint16* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint16* p = (kuint16*)address;
	array += offset;
	while(i++ < length) {
		*p++ = *array++;
	}
	return length;
}
kfunction kuint kmemory_linear_write_kuint32_array(kuint address, kuint32* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint32* p = (kuint32*)address;
	array += offset;
	while(i++ < length) {
		*p++ = *array++;
	}
	return length;
}
kfunction kuint kmemory_linear_write_kuint64_array(kuint address, kuint64* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint64* p = (kuint64*)address;
	array += offset;
	while(i++ < length) {
		*p++ = *array++;
	}
	return length;
}

kfunction kuint kmemory_linear_read_kuint_address(kuint address, kuint* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint* p = (kuint*)address;
	array += offset;
	while(i++ < length) {
		*array++ = *p;
	}
	return length;
}
kfunction kuint kmemory_linear_read_kuint8_address(kuint address, kuint8* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint8* p = (kuint8*)address;
	array += offset;
	while(i++ < length) {
		*array++ = *p;
	}
	return length;
}
kfunction kuint kmemory_linear_read_kuint16_address(kuint address, kuint16* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint16* p = (kuint16*)address;
	array += offset;
	while(i++ < length) {
		*array++ = *p;
	}
	return length;
}
kfunction kuint kmemory_linear_read_kuint32_address(kuint address, kuint32* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint32* p = (kuint32*)address;
	array += offset;
	while(i++ < length) {
		*array++ = *p;
	}
	return length;
}
kfunction kuint kmemory_linear_read_kuint64_address(kuint address, kuint64* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint64* p = (kuint64*)address;
	array += offset;
	while(i++ < length) {
		*array++ = *p;
	}
	return length;
}

kfunction kuint kmemory_linear_write_kuint_address(kuint address, kuint* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint* p = (kuint*)address;
	array += offset;
	while(i++ < length) {
		*p = *array++;
	}
	return length;
}
kfunction kuint kmemory_linear_write_kuint8_address(kuint address, kuint8* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint8* p = (kuint8*)address;
	array += offset;
	while(i++ < length) {
		*p = *array++;
	}
	return length;
}
kfunction kuint kmemory_linear_write_kuint16_address(kuint address, kuint16* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint16* p = (kuint16*)address;
	array += offset;
	while(i++ < length) {
		*p = *array++;
	}
	return length;
}
kfunction kuint kmemory_linear_write_kuint32_address(kuint address, kuint32* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint32* p = (kuint32*)address;
	array += offset;
	while(i++ < length) {
		*p = *array++;
	}
	return length;
}
kfunction kuint kmemory_linear_write_kuint64_address(kuint address, kuint64* array, kuint offset, kuint length) {
	kuint i = 0;
	kuint64* p = (kuint64*)address;
	array += offset;
	while(i++ < length) {
		*p = *array++;
	}
	return length;
}

kfunction void* kmemory_virtual_page_allocate(kuint count) {
	kuint address = next_page;
	next_page += (count * kpage_size);
	return (void*)kmemory_linear_page_allocate(address, count);
}

kfunction void kmemory_virtual_page_unallocate(void* pointer) {
}

kfunction void kmemory_virtual_copy(void* destination, void* source, kuint size) {
	if(size == 0) {
		asm volatile(
			"cld\n\t"
			"rep\n\t"
			"movsb\n\t"
			: "=c" (size), "=D" (destination), "=S" (source)
			: "c" (size), "D" (destination), "S" (source)
			: "cc", "memory"
		);
	}
}

kfunction void kmemory_virtual_fill(void* pointer, kuint size, kuint8 value) {
	if(size == 0) {
		asm volatile(
			"cld\n\t"
			"rep\n\t"
			"stosb\n\t"
			: "=c" (size), "=D" (pointer)
			: "a" (value), "c" (size), "D" (pointer)
			: "cc", "memory"
		);
	}
}

