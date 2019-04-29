/**
 * rtc.h
 *
 * h file that holds definitions of functions, including all the
 * functions in rtc.c
 */
#ifndef _RTC_H
#define _RTC_H

#include "x86_desc.h"
#include "bootinit/idt.h"
#include "linkage.h"

/* variables to hold rtc information */
int global_rtc_freq;

/* function declarations */

/* Function to initialize rtc */
extern void init_rtc();

/* Function to handle rtc interrupt */
extern void handle_rtc_interrupt();

/* Function to open RTC and reset frequency */
int32_t rtc_open (const uint8_t* filename);

/* Function to close RTC and reset frequency */
int32_t rtc_close (int32_t fd);

/* Function to wait for RTC interrupt */
int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes);

/* Function to write frequency to RTC */
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes);

#endif // _RTC_H
