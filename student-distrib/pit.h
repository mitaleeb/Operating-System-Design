/**
 * pit.h
 *
 * h file that holds definitions of functions, including all the
 * functions in pit.c
 */

#ifndef _PIT_H
#define _PIT_H

#include "constants.h"
#include "types.h"

/* defines to improve readability */

/**
 * init_pit()
 * 
 * DESCRIPTION: initializes the pit
 */
void init_pit();

/**
 * handle_pit_interrupt()
 * 
 * DESCRIPTION: handles a pit interrupt to perform what we need to happen during
 * one of these - namely, running the scheduling sequence.
 */
void handle_pit_interrupt();

#endif
