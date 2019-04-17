/**
 * keyboard.h
 *
 * h file that holds definitions of functions, including all the
 * functions in keyboard.c
 */

#include "x86_desc.h"
#include "bootinit/idt.h"
#include "linkage.h"


#define MAXBUFFER 128
uint8_t old_term_buffer[MAXBUFFER];
uint8_t new_term_buffer[MAXBUFFER];
int term_buffer_index;

/**
 * Function Declarations
 */

/* Function to initialize keyboard device */
extern void init_keyboard();

/* Function to handle keyboard interrupt */
extern void handle_keyboard_interrupt();

/* Function to write to buffer from keyboard input */
extern void write_to_buffer(uint8_t k);

/* Function for when user presses backspace */
extern void backspace_buffer(void);

/* Function for when user presses enter */
extern void enter_buffer(void);

/* terminal system call functions: */
extern int32_t terminal_open(const uint8_t* filename);
extern int32_t terminal_close(int32_t fd);
extern int32_t terminal_read(int32_t fd, void* buf, int32_t length);
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t length);
