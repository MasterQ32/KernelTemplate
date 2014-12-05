#include "interrupts.h"
#include "console.h"
#include "stdlib.h"

#define GDT_ENTRIES 256
#define IDT_ENTRIES 8

static uint64_t gdt[GDT_ENTRIES];
static uint64_t idt[IDT_ENTRIES];

#include "intr_stubs.h"

void intr_routine()
{
	kprintf("\n\x12\x04Interrupt occurred!\x12\0x7\n"); 
	while(1);
}

static void gdt_entry(int i, uint32_t base, uint32_t limit, int flags)
{
    gdt[i] = limit & 0xffffLL;
    gdt[i] |= (base & 0xffffffLL) << 16;
    gdt[i] |= (flags & 0xffLL) << 40;
    gdt[i] |= ((limit >> 16) & 0xfLL) << 48;
    gdt[i] |= ((flags >> 8 )& 0xffLL) << 52;
    gdt[i] |= ((base >> 24) & 0xffLL) << 56;
}

static void idt_entry(int i, void (*fn)(), unsigned int selector, int flags)
{
	unsigned long int handler = (unsigned long int) fn;
	idt[i] = handler & 0xffffLL;
	idt[i] |= (selector & 0xffffLL) << 16;
	idt[i] |= (flags & 0xffLL) << 40;
	idt[i] |= ((handler>> 16) & 0xffffLL) << 48;
}

static void init_gdt(void)
{
	memset(gdt, 0, sizeof(gdt));
	
    gdt_entry(0, 0, 0, 0);
    gdt_entry(1, 0, 0xfffff, GDTF_SEGMENT | GDTF_32_BIT |
        GDTF_CODESEG | GDTF_4K_GRAN | GDTF_PRESENT);
    gdt_entry(2, 0, 0xfffff, GDTF_SEGMENT | GDTF_32_BIT |
        GDTF_DATASEG | GDTF_4K_GRAN | GDTF_PRESENT);
    gdt_entry(3, 0, 0xfffff, GDTF_SEGMENT | GDTF_32_BIT |
        GDTF_CODESEG | GDTF_4K_GRAN | GDTF_PRESENT | GDTF_RING3);
    gdt_entry(4, 0, 0xfffff, GDTF_SEGMENT | GDTF_32_BIT |
        GDTF_DATASEG | GDTF_4K_GRAN | GDTF_PRESENT | GDTF_RING3);
	
	struct
	{
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) gdtp =
	{
		.limit = GDT_ENTRIES * 8 - 1,
		.pointer = gdt,
	};
	__asm__ volatile("lgdt %0" : : "m" (gdtp));
	__asm__ volatile(
		"mov $0x10, %ax;"
		"mov %ax, %ds;"
		"mov %ax, %es;"
		"mov %ax, %ss;"
		"ljmp $0x8, $.1;"
		".1:"
	);
}

#define IDT_FLAG_INTERRUPT_GATE 0xe
#define IDT_FLAG_PRESENT 0x80
#define IDT_FLAG_RING0 0x00
#define IDT_FLAG_RING3 0x60
  
static void init_idt()
{
	memset(idt, 0, sizeof(idt));

	//kprintf("\n%x %x %x\n", intr_stub_0, &intr_stub_0, intr_handler[0]);
	
	for(int i = 0; i < IDT_ENTRIES; i++)
	{
		idt_entry(
			i, 
			intr_stub_0, 
			0x08, 
			IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_PRESENT | IDT_FLAG_RING0);
	}
	struct
	{
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) idtp = 
	{
		.limit = IDT_ENTRIES * 8 - 1,
		.pointer = idt,
	};
	__asm__ volatile("lidt %0" : : "m" (idtp));
}

void intr_init(void)
{
	init_gdt();
	init_idt();
}

void irq_enable(void)
{
	__asm__ volatile("sti");
}

void irq_disable(void)
{
	__asm__ volatile("cli");
}