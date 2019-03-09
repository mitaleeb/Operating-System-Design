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


static uint8_t keyboard_output[4][128] = {
	/* regular */
	{'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	 '\0', '\0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	 '\0', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	 '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '*',
	 '\0', ' ', '\0'},
	/* shift */
	{'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	 '\0', '\0', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
	 '\0', '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ':', '"', '~',
	 '\0', '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', '\0', '*',
	 '\0', ' ', '\0'},
	/* caps lock */
	{'\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	 '\0', '\0', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
	 '\0', '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' , ';', '\'', '`',
	 '\0', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', '\0', '*',
	 '\0', ' ', '\0'},
	/* caps lock and shift */
	{'\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	 '\0', '\0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}',
	 '\0', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' , ':', '"', '~',
	 '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', '\0', '*',
	 '\0', ' ', '\0'}
};



/* Function to initialize keyboard device */
void init_keyboard() {
    
    /* keyboard is on IR1 of master PIC */
    unsigned int irq_keyboard = 1;
    enable_irq(irq_keyboard);
}

/* Function to handle keyboard interrupt */
unsigned char handle_keyboard_interrupt() {
    
    /* 0x28 vector in the IDT was set to
     * handle_keyboard_interrupt */
    
    unsigned char c = 0x00;
    
    while (c != 0x00) {
        /* read from 0x60 = data port from keyboard controller */
        c = inb(0x60);
    }
    
    switch (c) {
        // based on which character was input
    }
    
    unsigned int irq_keyboard = 1;
    send_eoi(irq_keyboard);
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
}
