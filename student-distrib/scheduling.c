/**
 * scheduling.c
 *
 * A file that holds functions for scheduling
 * and handling PIT interrupts.
 *
 */

#include "x86_desc.h"
#include "i8259.h"
#include "lib.h"
#include "bootinit/paging.h"
#include "sys/pcb.h"
#include "rtc.h"
#include "scheduling.h"
#include "sys/syscall.h"
#include "terminal.h"
#include "types.h"

#define EIGHT_MB    0x00800000
#define FOUR_MB     0x00400000
#define EIGHT_KB    0x00002000
#define FOUR_KB     0x00001000

// #define DATA_RATE   59659
#define DATA_RATE   11932
#define MODE_REG    0x43
#define DATA_REG    0x36
#define DATA_PORT   0x40
#define LOW_BYTE    0x00FF
#define HIGH_BYTE   8
#define IRQ_PIT     0

#define FIRST_TERM		0
#define SECOND_TERM		1
#define THIRD_TERM		2
#define NUM_TERMS     3
#define NOT_RUNNING		0
#define RUNNING				1

#define DATA_SIZE   4
#define MAX_PROCS   6

curr_terminal_active = 0;
next_terminal_active = 0;

/*
 *   init_pit()
 *     Inputs: none
 *     Return Value: none
 *     Function: Initializes Programmable Interrupt Timer
 *   links: osdever.net
 */
void init_pit() {

  /* 0x43 - Mode/Command register (write only)
   * 0x40 Channel 0 Data Port (read/write) */
  outb(0x34, MODE_REG);
  /* data rate = 1193180 / 20 Hz */
  outb(DATA_RATE & LOW_BYTE, DATA_PORT);             // set low byte of divisor
  outb(DATA_RATE >> HIGH_BYTE, DATA_PORT);           // set high byte of divisor

  /* Set PIT Interrupts (IRQ 0) */
  enable_irq(IRQ_PIT);
  return;
}

/*
 *   handle_pit_interrupt
 *     Inputs: none
 *     Return Value: none
 *     Function: handles PIT interrupt, scheduling
 *               to enable background processes
 */
void handle_pit_interrupt() {
  int i;
  /* send end of interrupt */
  send_eoi(IRQ_PIT);


  /* if only terminal 0 is running, return */
  if (terminal[SECOND_TERM].shell_flag == NOT_RUNNING && terminal[THIRD_TERM].shell_flag == NOT_RUNNING) {
    return;
  }

  /* go to next running terminal */
  next_terminal_active = curr_terminal_active;

  for (i = 0; i < NUM_TERMS; i++) {
      if (terminal[next_terminal_active].active_flag == NOT_RUNNING) {
          next_terminal_active = next_terminal_active + 1;
          if (next_terminal_active >= NUM_TERMS) {
            next_terminal_active -= NUM_TERMS;
          }
      } else {
        break;
      }
  }
  int32_t next_process = (int32_t)terminal[next_terminal_active].curr_process_num;

  int32_t phys_addr = EIGHT_MB + (next_process * FOUR_MB);
  add_program_page((void*)phys_addr, RUNNING);

  pcb_t * current_pcb = (pcb_t*)(EIGHT_MB - (terminal[curr_terminal_active].curr_process_num + 1) * EIGHT_KB);
  pcb_t * next_pcb = (pcb_t*)(EIGHT_MB - (next_process + 1) * EIGHT_KB);
  curr_pcb = next_pcb;
  curr_terminal_active = next_terminal_active;

  /* restore next process's TSS */
  tss.ss0 = KERNEL_DS;
  tss.esp0 = EIGHT_MB - (next_process) * EIGHT_KB - DATA_SIZE;

  /* switch to next process's kernel stack */
  asm volatile(
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        :"=g"(current_pcb->esp), "=g"(current_pcb->ebp)
        :
        : "memory"
  );

  asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        :
        :"g"(next_pcb->esp), "g"(next_pcb->ebp)
        : "memory"
  );

  return;
}
