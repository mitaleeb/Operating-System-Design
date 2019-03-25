/**
 * devices.h
 *
 * h file that holds a lot of definitions of functions, including all the
 * functions in devices.c
 */

#include "x86_desc.h"
#include "bootinit/idt.h"
#include "linkage.h"

/**
 * Function Declarations
 */

/* Function to initialize keyboard device */
extern void init_keyboard();

/* Function to handle keyboard interrupt */
extern void handle_keyboard_interrupt();

/* Function to initialize rtc */
extern void init_rtc();

/* Function to handle rtc interrupt */
extern void handle_rtc_interrupt();

/* Function to handle systemcall interrupt */
extern void handle_systemcall_interrupt();

/* Function to write to buffer from keyboard input */
extern void write_to_buffer(uint8_t k);

/* Function for when user presses backspace */
extern void backspace_buffer(void);

/* Function for when user presses enter */
extern void enter_buffer(void);

int32_t terminal_read(uint8_t* buf, int32_t length);