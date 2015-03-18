#include "stdlib.h"
#include "console.h"
#include "interrupts.h"

static void debug_test()
{
	char buffer[64];
	kputs("itoa test:\n");
	kputs("10 =   "); kputs(itoa(10, buffer, 10)); kputc('\n');
	kputs("10 = 0x"); kputs(itoa(10, buffer, 16)); kputc('\n');
	kputs("10 = 0b"); kputs(itoa(10, buffer, 2)); kputc('\n');
	kputs("10 = 0o"); kputs(itoa(10, buffer, 8)); kputc('\n');

	kputs("printf test:\n");
	kprintf("This %s %c test line.\n", "is", 'a');
	kprintf("Numbers: %d %i %x %b\n", 15, 15, 15, 15);

	/*
	kputs("scroll-test:\n");
	for(int i = 0; i < 100; i++)
	{
		kprintf("They see me scrolling, they hating! %i\n", i);
	}
	for(int i = 0; i < 272; i++)
	{
		kprintf("x");
	}
	*/
}

void init(void)
{
	ksetcolor(COLOR_BLACK, COLOR_LIGHTGRAY);
	kclear();
	kputs("Welcome to \x12\x05nucleo\x12\x07!\n");

	kputs("Initialize interrupts: ");
	intr_init();
	kputs("success.\n");

	// kputs("Trigger INT0: ");
	//__asm__ volatile("int $0x20");
	// kputs("success.\n");

	kputs("Enable hw interrupts:  ");
	irq_enable();
	kputs("success.\n");

	while(0);
}
