/**
 * devices.c
 * 
 * A file that holds functions that help initialize
 * and handle interrupts from the keyboard and RTC.
 */

#include "i8259.h"
#include "lib.h"
#include "idt.h"
#include "devices.h"

/* Function to initialize keyboard device */
void init_keyboard() {
    /* keyboard is on IR1 of master PIC */
    unsigned int irq_keyboard = 1;
    
    enable_irq(irq_keyboard);
}

/* Function to handle keyboard interrupt */
unsigned char handle_keyboard_interrupt() {
    /* first, need to set 0x28 vector in the IDT to
     * handle_keyboard_interrupt */
    
    unsigned char c = 0x00;
    
    while (c != 0x00) {
        /* read from 0x60 = data port from keyboard controller */
        c = inb(0x60);
    }
}

/* Function to initialize rtc */
extern void init_rtc() {
    
    /* clear interrupts */
    // cli();
    
    /* 0x70, 0x71 - I/O ports
     * 0x8B - RTC status register */
    outb(0x8A, 0x70);		    /* write to status register A, disable non-maskable interrupts */
    char a = inb(0x71);	        /* read from status register A */
    outb(0x8B, 0x70);		    /* write to status register B */
    outb(a | 0x40, 0x71);       /* turn on 6th bit of status register */
    

    /* RTC is IR0 on slave PIC - irq num is 8 */
    enable_irq(8);

    /* set interrupts */
    // sti();
}

/* Function to handle rtc interrupt */
extern void handle_rtc_interrupt() {
    
    /* send end of interrupt */
	send_eoi(8);
	
    /* clear interrupts */
    // cli();

	/* read from status register C */
	outb(0x8C, 0x70);
	inb(0x71);
	
	/* set interrupts */
    // sti();
}
