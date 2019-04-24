/**
 * pit.c
 */

#include "lib.h"
#include "scheduler.h"
#include "i8259.h"
#include "pit.h"

#define DATA_PORT 0x40
#define MODE_REG  0x43
#define MODE_HEX  0x36 // 0b00110110
#define DATA_RATE 47720 // 1.193 MHz / 25 Hz = 47720


/**
 * init_pit()
 * 
 * DESCRIPTION: initializes the PIT
 */
void init_pit() {
  /* Based on https://wiki.osdev.org/Programmable_Interval_Timer.
   * The pit channel we want to use is channel 0, which is connected to the PIC at IRQ 0. 
   * It generates interrupts at a set rate like RTC, but has the highest priority (which we want). 
   * the Data I/O port for channel 0 is 0x40. The port for the mode register is 0x43.
   * 
   * The mode register is the following 8-bit value:
   * Bits 7-6: Channel Select (we want 00)
   * Bits 5-4: Access Mode (we either want 01 or 11)
   * Bits 3-1: Mode Select (we want mode 2 or 3)
   * Bit    0: BCD/Binary Mode (we want binary mode, 0)
   * which is defined above.
   * 
   * The oscillator has a frequency of ~1.193 MHz. We write a frequency divider
   * value to the data port to set the frequency we want. For our purposes, we
   * want around a 25 Hz signal, so using some math we have 1.193 MHz / 25 Hz =
   * 47720. 
   */

  // write the mode to the mode register
  outb(MODE_HEX, MODE_REG);

  // set the data rate. The low byte first followed by the high byte
  outb((uint8_t)(DATA_RATE & LO_BYTE_MASK), DATA_PORT);
  outb((uint8_t)((DATA_RATE & HI_BYTE_MASK) >> BYTE_SIZE), DATA_PORT);

  // enable pit interrupts
  enable_irq(IRQ_PIT);
}

/**
 * handle_pit_interrupt
 * 
 * DESCRIPTION: handles an interrupt from the pit. In our case, schedules.
 */
void handle_pit_interrupt() {
  disable_irq(IRQ_PIT);

  // putc('p');

  // send end of interrupt
  send_eoi(IRQ_PIT);

  // perform task switching
  // int next_pid = find_next_pid();
  // context_switch(curr_pcb->pid, next_pid);

  enable_irq(IRQ_PIT);
}
