/**
 * rtc.h
 *
 * h file that holds definitions of functions, including all the
 * functions in rtc.c
 */

#include "x86_desc.h"
#include "bootinit/idt.h"
#include "linkage.h"

/**
 * Function Declarations
 */

/* Function to initialize rtc */
extern void init_rtc();

/* Function to handle rtc interrupt */
extern void handle_rtc_interrupt();