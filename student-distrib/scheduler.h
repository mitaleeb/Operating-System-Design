/**
 * scheduler.h
 */

 #ifndef _SCHEDULER_H
 #define _SCHEDULER_H

#include "lib.h"
#include "x86_desc.h"
#include "bootinit/paging.h"
#include "sys/pcb.h"


// here maybe temporarily for debug purposes
void context_switch(int pid_from, int pid_to);

#endif
