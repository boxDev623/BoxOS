#include <kcsl.h>
#include <kcvs.h>
#include <vga.h>
#include <vga_modes.h>

#include <io_ports.h>
#include <string.h>
#include <timer.h>

#include <mm/kheap.h>

#include <interrupt/gdt.h>
#include <interrupt/isr.h>

#include <drivers/mouse.h>
#include <drivers/keyboard.h>

void kmain(void)
{
    kcls_initialize();

    kheap_initialize();

    gdt_initialize();
    idt_initialize();

    kprintf("|--\\\\\n");
    kprintf("|  ||    /---\\    \\\\  //\n");
    kprintf("|--<    /     \\    \\\\//\n");
    kprintf("|  ||   \\     /    /\\\\\n");
    kprintf("|--//    \\---/    // \\\\     OS\n");

    timer_initialize(1);

    keyboard_initialize();
    mouse_initialize();
}
