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
    // i8259_auto_eoi = auto_eoi;
    short saved_21 =  in(0x21);
    short saved_A1 =  in(0xA1);
    
    /* mask interrupts on all PICS */
    cli();
    outb(0xff, MASTER_8259_PORT + 1);
    outb(0xff, SLAVE_8259_PORT + 1);
    
    /* initialize 4 ICWs for master PIC */
    outb(ICW1, MASTER_8259_PORT);               /* ICW1: select 8259A-1 init */
    outb(ICW2_MASTER, MASTER_8259_PORT + 1);    /* ICW2: 8259A-1 IR0-7 mapped to 0x20-0x27 */
    outb(ICW3_MASTER, MASTER_8259_PORT + 1);    /* ICW3: 8259A-1 (master) has slave on IR1 (keyboard) */
    outb(ICW4, MASTER_8259_PORT + 1);           /* master expects normal EOI */
    
    /* initialize 4 ICWs for slave PIC */
    outb(ICW1, SLAVE_8259_PORT);                /* ICW1: select 8259A-2 init */
    outb(ICW2_SLAVE, SLAVE_8259_PORT + 1);      /* ICW2: 8259A-2 IR0-7 mapped to 0x28-0x2F */
    outb(ICW3_SLAVE, SLAVE_8259_PORT + 1);      /* ICW3: 8259A-2 is slave on master's IR2 */
    outb(ICW4, SLAVE_8259_PORT + 1);            /* ICW4: normal EOI */


    outb(saved_21, MASTER_8259_PORT + 1)        /* restore master IRQ mask */
    outb(saved_A1, SLAVE_8259_PORT + 1)         /* restore slave IRQ mask */
    sti();
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    // enable port to enable interrupts
    unsigned int mask, irq = irq_num - I8259A_IRQ_BASE;
	unsigned long flags;

	mask = ~(1 << irq);
	
	/* clear interrupts */
	cli();
	
	cached_irq_mask &= mask;
	if (irq & 8)
		outb(cached_slave_mask, PIC_SLAVE_IMR);
	else
		outb(cached_master_mask, PIC_MASTER_IMR);
		
	/* set interrupts */
	sti();
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    unsigned int mask, irq = irq_num - I8259A_IRQ_BASE;
	unsigned long flags;

	mask = 1 << irq;
	
	/* clear interrupts */
	cli();
	
	cached_irq_mask |= mask;
	if (irq & 8)
		outb(cached_slave_mask, PIC_SLAVE_IMR);
	else
		outb(cached_master_mask, PIC_MASTER_IMR);
		
	/* set interrupts */
	sti();
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    // OR if active low
    // AND with 0 if active high
}
