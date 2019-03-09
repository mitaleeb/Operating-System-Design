/**
 * idt.h
 *
 * h file that holds a lot of definitions of functions, including all the
 * functions in idt.c as well as declarations for all the interrupt handlers in
 * the first 32 IDT locations.
 */

#include "x86_desc.h"

/* Declare functions that we use to set up to the idt */
void populate_idt(idt_desc_t* idt);


/* The following block of code declares functions for all the handlers of the
reserved vectors. We can use an "&" to get the address of this when we use them
to set IDT value */

/**
 * idt_de()
 *
 * Divide by zero error. Interrupt handler for interrupt vector 0x00. 
 */
extern void idt_de();

extern void idt_db();

extern void idt_nmi();

extern void idt_bp();

extern void idt_of();

extern void idt_br();

extern void idt_ud();

extern void idt_kk();

extern void handle_keyboard_interrupt();