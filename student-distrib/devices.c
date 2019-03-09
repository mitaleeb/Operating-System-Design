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

// static uint8_t key_state = 0x00;
static uint8_t keyboard_output[128] = {
	 '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	 '\0', '\0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	 '\0', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	 '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '*',
	 '\0', ' ', '\0'
};



/* Function to initialize keyboard device */
void init_keyboard() {
    
    /* keyboard is on IR1 of master PIC */
    unsigned int irq_keyboard = 1;
    enable_irq(irq_keyboard);
}

/* Function to handle keyboard interrupt */
unsigned char handle_keyboard_interrupt() {
    
    /* 0x21 vector in the IDT was set to
     * handle_keyboard_interrupt */
     
    /* clear interrupts */
    // cli();
    
    /* read from 0x60 = data port from keyboard controller */
    uint8_t c = 0x00;
    while (c != 0x00) {
        c = inb(0x60);
    }
    
    /* print character to screen */
    putc(keyboard_output[c]);
    
    /* send end of interrupt */
    unsigned int irq_keyboard = 1;
    send_eoi(irq_keyboard);
    
    /* set interrupts */
    // sti();
    
    /* do not return from handler in this checkpoint */
    while(1);
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
    unsigned int irq_rtc = 8;
    enable_irq(irq_rtc);

    /* set interrupts */
    // sti();
    
}

/* Function to handle rtc interrupt */
extern void handle_rtc_interrupt() {
	
    /* clear interrupts */
    // cli();

	/* read from status register C */
	outb(0x8C, 0x70);
	inb(0x71);
	
	/* send end of interrupt */
    unsigned int irq_rtc = 8;
	send_eoi(irq_rtc);
	
	/* set interrupts */
    // sti();
    
    // test by setting to certain frequency and whenever you get interrupt print stuff out
    // get more than one interrupt
    
    /* do not return from handler in this checkpoint */
    while(1);
}
