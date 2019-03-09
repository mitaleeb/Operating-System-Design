/**
 * keyboard.c
 * 
 * A file that holds functions that help initialize
 * and handle interrupts from the keyboard.
 */

#include "i8259.h"
#include "lib.h"
#include "idt.h"
#include "keyboard.h"

/* Function to initialize keyboard device */
void init_keyboard() {
    /* keyboard is on IR1 of master PIC */
    unsigned int irq_keyboard = 1;
    
    enable_irq(irq_keyboard);
}

/* Function to handle keyboard interrupt */
void handle_keyboard_interrupt() {
    /* first, need to set 0x28 vector in the IDT to
     * handle_keyboard_interrupt */
    
    char c = 0x00;
    
    while (c != 0x00) {
        c = inb(0x60);
    }
}
