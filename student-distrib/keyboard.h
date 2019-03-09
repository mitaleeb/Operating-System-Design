/**
 * keyboard.h
 *
 * h file that holds a lot of definitions of functions, including all the
 * functions in keyboard.c
 */

#include "x86_desc.h"
#include "idt.h"

/**
 * Function Declarations
 */
 
/* Function to initialize keyboard device */
extern void init_keyboard();

/* Function to handle keyboard interrupt */
extern void handle_keyboard_interrupt();
