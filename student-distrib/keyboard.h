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

/**
 * Function Declarations
 */

 /* void init_keyboard()
  * Inputs: none
  * Return Value: none
  * Function: initialize keyboard device by masking IRQ line
  */
extern void init_keyboard();

/* void handle_keyboard_interrupt()
 * Inputs: none
 * Return Value: none
 * Function: Handles keyboard interrupt and outputs key press to screen
 */
extern void handle_keyboard_interrupt();

/* void write_to_buffer(uint8_t k)
 * Inputs: the character to write to the terminal buffer
 * Return Value: none
 * Function: Handles keyboard input and writes to buffer to be written to screen
 */
extern void write_to_buffer(uint8_t k);

/* void backspace_buffer(void)
 * Inputs: none
 * Return Value: none
 * Function: Handles keyboard input for when backspace key is pressed
 */
extern void backspace_buffer(void);

/* void enter_buffer(void)
 * Inputs: none
 * Return Value: none
 * Function: Handles keyboard input for when enter key is pressed
 */
extern void enter_buffer(void);

/* terminal system call functions: */

/*
* int32_t terminal_open()
*   Inputs: filename - Doesn't matter
*   Return Value: 0 if successful, -1 otherwise
*		Function: open terminal driver
*/
extern int32_t terminal_open(const uint8_t* filename);

/*
* int32_t terminal_close()
*   Inputs: fd - file descriptor to close (doesn't matter)
*   Return Value: 0
*		Function: close terminal driver
*/
extern int32_t terminal_close(int32_t fd);

/*
* int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t length)
*   Inputs: uint8_t* buf = buffer
*		int32_t length = length
*   int32_t fd = file decorator
*   Return Value: number of bytes read
*		Function: read keyboard buffer, clear keyboard buffer
* 	calling terminal read should give a clear buffer
*/
extern int32_t terminal_read(int32_t fd, void* buf, int32_t length);

/*
* int32_t terminal_write(int32_t fd, uint8_t* buf, int32_t length)
*   Inputs: uint8_t* buf = buffer
*		int32_t length = length
*   int32_t fd = file decorator
*   Return Value: number of bytes written
*	Function: write keyboard buffer, clear keyboard buffer
*
*/
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t length);
