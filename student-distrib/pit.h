/**
 * pit.h
 *
 * h file that holds definitions of functions, including all the
 * functions in pit.c
 */

#ifndef _PIT_H
#define _PIT_H

/* function to initialize PIT */
extern void init_pit(void);

/* function to handle PIT interrupts */
extern void handle_pit_interrupt(void);

extern int curr_terminal_active;
extern int next_terminal_active;

#endif
