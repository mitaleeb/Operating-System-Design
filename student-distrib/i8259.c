/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xff; /* IRQs 0-7  */
uint8_t slave_mask = 0xff;  /* IRQs 8-15 */


/* Initialize the 8259 PIC */
void i8259_init(void) {
    // unsigned long flags;
    // i8259_auto_eoi = auto_eoi;
    short saved_21 =  inb(0x21);
    short saved_A1 =  inb(0xA1);
    
    /* mask interrupts on all PICS */
    // cli();
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


    outb(saved_21, MASTER_8259_PORT + 1);        /* restore master IRQ mask */
    outb(saved_A1, SLAVE_8259_PORT + 1);         /* restore slave IRQ mask */

    enable_irq(2);								 /* enable slave irq 2 on PIC */
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    
    unsigned int mask=0xFE; 		// Initializes mask as 1111 1110 to use bit zero as irq mask
	int i;							// Variable used for bit shifting in loop
	//mask = 1 << irq_num;
	
	/* clear interrupts */
	//cli();
	
	/* determine if PIC is master or slave */
	if (irq_num > 7) {
		irq_num -= 8; // Decrement to get the slave idx
		for (i = 0; i < irq_num; i++) {
			mask = (mask << 1) + 1;
		}
	    slave_mask &= mask;
		outb(slave_mask, SLAVE_8259_PORT + 1);
	} else {
		for (i = 0; i < irq_num; i++) {
			mask = (mask << 1) + 1;
		}
	    slave_mask &= mask;
		outb(master_mask, MASTER_8259_PORT + 1);
	}
		
	/* set interrupts */
	//sti();
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    /* decrement to bits 0-7 if on slave pic */
    if (irq_num > 7)
        irq_num -= 8;
    
    unsigned int mask=0x00;				// Disable IRQ line mask with '1' bit
	mask = 1 << irq_num;
	
	/* clear interrupts */
	//cli();
	
	/* determine if PIC is master or slave */
	if (irq_num > 7) {
	    // OR if active low
	    slave_mask |= mask;
		outb(slave_mask, SLAVE_8259_PORT + 1);
	} else {
		master_mask |= mask;
		outb(master_mask, MASTER_8259_PORT + 1);
	}
		
	/* set interrupts */
	//sti();
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    
    /* determine if PIC is master or slave */
	if(irq_num > 7) {
		irq_num -= 8;
		outb((EOI | irq_num), SLAVE_8259_PORT);     /* send EOI to slave */
		outb((EOI + 2), MASTER_8259_PORT);          /* send EOI to master */
	} else {
		outb((EOI | irq_num), MASTER_8259_PORT);    /* send EOI to master */
	}
}
