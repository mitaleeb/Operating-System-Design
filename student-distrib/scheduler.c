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



int find_next_pid() {
  int next_term = (curr_pcb->term_index + 1) % MAX_TERMS;
  // make sure the next terminal we found is running
  while (terminal[next_term].is_started == 0) {
    next_term = (next_term + 1) % MAX_TERMS;
  }

  // sanity check to make sure that terminal doesn't have a null pcb
  if (terminal_pcbs[next_term] == NULL) {
    return -1;
  }

  // return the next pid the scheduler wants to switch to
  return terminal_pcbs[next_term]->pid;
}

// here maybe temporarily for debug purposes
void context_switch(int pid_from, int pid_to) {
  // error checking
  if (pid_from < 0 || pid_from >= MAX_PROCS || pid_to < 0 || pid_to >= MAX_PROCS) {
    return;
  }

  if (pid_from == pid_to) {
    return; // also don't do anything
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

  // update current_pcb
  curr_pcb = pcb_to;

  // change the relevant variables in the TSS
  tss.ss0 = KERNEL_DS;
  tss.esp0 = EIGHT_MB - (pid_to * EIGHT_KB) - 4;


  // save the esp and ebp
  // I think we have to do this in a separate asm block because
  // the context switch will not have outputted to the output values
  // before we context switch if we leave them together
  asm volatile (
    "movl %%esp, %0;"
    "movl %%ebp, %1;"
    : "=m" (pcb_from->my_esp), "=m"(pcb_from->my_ebp)
  );

  // context switch like in execute... do we need to do ring 3 conversion?
  // do we need to save all registers?
  asm volatile (
    "cli;"
    "movl %0, %%esp;"
    "movl %1, %%ebp;"
    "movl %%cr3, %%eax;"
    "movl %%eax, %%cr3;" // flush the tlb
    "sti;"
    : 
    :"m" (pcb_to->my_esp), "m"(pcb_to->my_ebp)
    :"memory", "%eax"
  );
}
