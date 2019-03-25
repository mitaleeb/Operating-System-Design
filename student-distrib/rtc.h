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

/* Function to open RTC and reset frequency */
int32_t rtc_open ();

/* Function to close RTC and reset frequency */
int32_t rtc_close ();

/* Function to wait for RTC interrupt */
int32_t rtc_read ();

/* Function to write frequency to RTC */
int32_t rtc_write (int32_t freq);
