/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    unsigned long flags;
    i8259_auto_eoi = auto_eoi;
    
    /* mask interrupts on all PICS */
    CLI();
    outb(0xff, 0x21);
    outb(0xff, 0x60);
    
    /* initialize 4 ICWs for master PIC */
    outb(0x11, 0x20);       /* ICW1: select 8259A-1 init */
    outb(0x20 + 0, 0x21);   /* ICW2: 8259A-1 IR0-7 mapped to 0x20-0x27 */
    outb(0x04, 0x21);       /* ICW3: 8259A-1 (master) has slave on IR1 (keyboard) */
    if (auto_eoi) {
        outb(0x03, 0x21);   /* master does auto EOI */
    } else {
        outb(0x01, 0x21);   /* master expects normal EOI */
    }
    

    /* initialize 4 ICWs for slave PIC */
    outb(0x11, 0xA0);       /* ICW1: select 8259A-2 init */
    outb(0x20 + 8, 0xA1);   /* ICW2: 8259A-2 IR0-7 mapped to 0x28-0x2F */
    outb(0x01, 0xA1);       /* ICW3: 8259A-2 is slave on master'sn IR1 */
    outb(0x01, 0xA1);       /* ICW4: normal EOI */

    // outb(cached_21, 0x21)   /* restore master IRQ mask */
    // outb(cached_A1, 0xA1)   /* restore slave IRQ mask */
    STI();
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    // enable port to enable interrupts
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    // OR if active low
    // AND with 0 if active high
}
