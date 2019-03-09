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
    
    // cli();
    
    /* 0x70, 0x71 - I/O ports
     * 0x8B - RTC status register */
     
    outb(0x70, 0x8B);		    /* write to status register, disable non-maskable interrupts */
    
    char c;
    c = inb(0x71);	            /* read from register */
    
    outb(0x70, 0x8B);		    /* rewrite to status register, since read resets it */
    outb(0x71, c | 0x40);       /* turn on 6th bit of status register */
    
    // sti();
}

/* Function to handle rtc interrupt */
extern void handle_rtc_interrupt() {
    
}
