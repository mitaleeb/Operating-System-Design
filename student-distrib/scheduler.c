/**
 * scheduler.c -
 */

#include "constants.h"
#include "scheduler.h"
#include "pit.h"

/**
 * scheduler_pass()
 * 
 * DESCRIPTION: passes control from the current process to the next.
 * INPUTS: none
 * OUTPUTS: none
 */
void scheduler_pass() {
  // reset the pit's counter
  pit_ticks = 0;

  // perform a context switch
  int next_pid = find_next_pid();
  context_switch(curr_pcb->pid, next_pid);
}

/**
 * find_next_pid()
 *
 * DESCRIPTION: finds the next process to switch to and and continue executing
 * INPUTS: NONE
 * OUTPUTS: -1 - if unsuccessful
 *          int - the number of the next process to switch to
 */
int find_next_pid() {
  // no next pid if only one terminal is open
  int i, count;
  for (i = 0, count = 0; i < MAX_TERMS; i++) {
    count += terminal[i].is_started;
  }

  if (count <= 1) {
    return -1;
  }

  // find the next pid (the next terminal's bottom most proc)
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

/**
 * context_switch(int pid_from, int pid_to)
 *
 * DESCRIPTION: context switch from one pid to another via saving and restoring/
 *              saving esp ebp
 * INPUTS: pid_from - pid we are switching from
 *         pid_to - pid to switch to
 * OUTPUTS: NONE
 */
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
