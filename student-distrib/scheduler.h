/**
 * scheduler.h
 */

 #ifndef _SCHEDULER_H
 #define _SCHEDULER_H

#include "lib.h"
#include "x86_desc.h"
#include "bootinit/paging.h"
#include "sys/pcb.h"
#include "terminal.h"

/**
 * find_next_pid()
 *
 * DESCRIPTION: finds the next process to switch to and and continue executing
 * INPUTS: NONE
 * OUTPUTS: -1 - if unsuccessful
 *          int - the number of the next process to switch to
 */
int find_next_pid();

/**
 * context_switch(int pid_from, int pid_to)
 *
 * DESCRIPTION: context switch from one pid to another via saving and restoring/ saving esp ebp
 * INPUTS: pid_from - pid we are switching from
 *         pid_to - pid to switch to
 * OUTPUTS: NONE
 */
void context_switch(int pid_from, int pid_to);

#endif
