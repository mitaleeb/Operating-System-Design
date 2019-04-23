/**
 * scheduler.c - 
 */

#include "scheduler.h"


#define EIGHT_MB    0x00800000
#define TWELVE_MB   0x00C00000
#define FOUR_MB     0x00400000
#define EIGHT_KB    0x00002000
#define MB_128      0x08000000
#define MB_132      0x08400000

// here maybe temporarily for debug purposes
void context_switch(int pid_from, int pid_to) {
  // error checking
  if (pid_from < -1 || pid_from >= MAX_PROCS || pid_to < -1 || pid_to >= MAX_PROCS) {
    return;
  }

  pcb_t* pcb_from;
  pcb_t* pcb_to;
  
  // do math to get the pcb pointers 
  if (pid_from == -1) {
    pcb_from = &root_pcb; 
  } else {
    pcb_from = (pcb_t*) (EIGHT_MB - (pid_from + 1) * EIGHT_KB);
  }

  if (pid_to == -1) {
    pcb_to = &root_pcb;
  } else {
    pcb_to = (pcb_t*) (EIGHT_MB - (pid_to + 1) * EIGHT_KB);
  }

  if (pid_to > -1) {
    // switch the program page
    int32_t phys_addr = EIGHT_MB + (pid_to * FOUR_MB);
    add_program_page((void*)phys_addr, 1);
  }

  // change the relevant variables in the TSS
  tss.ss0 = KERNEL_DS;
  tss.esp0 = EIGHT_MB - (pid_to * EIGHT_KB) + 4;

  // context switch like in execute... do we need to do ring 3 conversion?
  asm volatile (
    "movl %%esp, %0;"
    "movl %%ebp, %1;"
    "cli;"
    "movl %2, %%esp;"
    "movl %3, %%ebp;"
    "sti;"
    :"=g" (pcb_from->my_esp), "=g"(pcb_from->my_ebp)
    : "g" (pcb_to->my_esp), "g"(pcb_to->my_ebp)
    : "memory"
  );
}
