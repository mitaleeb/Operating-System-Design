/**
 * devices.h
 *
 * h file that holds a lot of definitions of functions, including all the
 * functions in devices.c
 */

#include "x86_desc.h"
#include "bootinit/idt.h"

/**
 * Function Declarations
 */
 
/* Function to initialize keyboard device */
extern void init_keyboard();

/* Function to handle keyboard interrupt */
extern unsigned char handle_keyboard_interrupt();

/* Function to initialize rtc */
extern void init_rtc();

/* Function to handle rtc interrupt */
extern void handle_rtc_interrupt();
