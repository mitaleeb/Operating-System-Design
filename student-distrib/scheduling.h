/**
 * scheduling.h
 *
 * h file that holds definitions of functions, including all the
 * functions in scheduling.c
 */

#ifndef _SCHEDULING_H
#define _SCHEDULING_H

/* function to initialize PIT */
extern void init_pit(void);

/* function to handle PIT interrupts */
extern void handle_pit_interrupt(void);

extern int curr_terminal_active;
extern int next_terminal_active;

#endif
