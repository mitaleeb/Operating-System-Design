/**
 * rtc.c
 *
 * A file that holds functions that help initialize
 * and handle interrupts from the rtc.
 */

#include "rtc.h"
#include "bootinit/idt.h"
#include "i8259.h"
#include "lib.h"
#include "constants.h"
#include "sys/pcb.h"

#define OUT_RTC 0x70
#define IN_RTC 0x71
#define STATUS_REGISTER_A 0x8A
#define STATUS_REGISTER_B 0x8B

#define RATE_0 0x00
#define RATE_2 0x0F
#define RATE_4 0x0E
#define RATE_8 0x0D
#define RATE_16 0x0C
#define RATE_32 0x0B
#define RATE_64 0x0A
#define RATE_128 0x09
#define RATE_256 0x08
#define RATE_512 0x07
#define RATE_1024 0x06

#define MAX_FREQ 1024
#define OPENED 1
#define NOT_OPENED 0
#define NUM_BYTES 4

// initialize flag to check if interrupt has occurred
volatile uint32_t ticks;


/* void init_rtc()
 * Inputs: none
 * Return Value: none
 * Function: Initializes RTC setting IRQ line
 */
extern void init_rtc() {
  /* clear interrupts */
  disable_irq(IRQ_RTC);
  ticks = 0;
  /* 0x70, 0x71 - I/O ports */
  outb(STATUS_REGISTER_A,
       OUT_RTC); /* write to status register A, disable non-maskable interrupts */
  unsigned char a = inb(IN_RTC);   /* read from status register A */
  outb(STATUS_REGISTER_B, OUT_RTC);     /* write to status register B */
  outb(a | 0x40, IN_RTC); /* turn on 6th bit of status register */
                        /* RTC is IR0 on slave PIC - irq num is 8 */

  // @TODO: i just copied this in, look here first for bugs
  /* write to status register A, disable non-maskable interrupts */
  outb(STATUS_REGISTER_A, OUT_RTC);
  /* read from status register A */
  a = inb(IN_RTC);
  /* write min rate (2 Hz) to status register A */
  outb(STATUS_REGISTER_A, OUT_RTC);
  outb((0xF0 & a) | RATE_2, IN_RTC);

  enable_irq(IRQ_RTC);
}

/* void handle_rtc_interrupt()
 * Inputs: none
 * Return Value: none
 * Function: Function to handle rtc interrupt
 */
extern void handle_rtc_interrupt() {
  // disable rtc interrupts while we handle one
  disable_irq(IRQ_RTC);

  // let the PIC handle other interrupts
  send_eoi(IRQ_RTC);

  /* read from status register C */
  outb(0x0C, OUT_RTC);
  inb(IN_RTC);

  // increment the ticks (with overflow logic)
  if (ticks == MAXUINT32) {
    ticks = 1;
  } else {
    ticks++;
  }

  // done handling the rtc interrupt, allow more
  enable_irq(IRQ_RTC);
}

/*
 * int32_t rtc_open()
 *
 * Inputs: none
 * Return Value: 0
 * Function: Opens the RTC
 */
int32_t rtc_open (const uint8_t* filename) {
  // set per-process rtc information
  curr_pcb->rtc_opened = OPENED;
  curr_pcb->rtc_freq = RATE_2;

  return 0;
}

/*
 * int32_t rtc_close()
 *
 * Inputs: none
 * Return Value: 0
 * Function: Closes the RTC
 */
int32_t rtc_close (int32_t fd) {
  // set per-process rtc information
  curr_pcb->rtc_opened = NOT_OPENED;
  return 0;
}

/*
 * int32_t rtc_read()
 *
 * Inputs: none
 * Return Value: 0
 * Function: Reads from RTC, waits for interrupt
 */
int32_t rtc_read (int32_t fd, void* buf, int32_t nbytes) {
  uint32_t curr_ticks = ticks;
  int proc_freq = curr_pcb->rtc_freq;

  // do some math to figure out the amount of ticks to get
  uint32_t wanted_ticks = global_rtc_freq / proc_freq;
  uint32_t target_ticks;

  // handle the case where ticks is near MAXUINT32
  if (curr_ticks > MAXUINT32 - wanted_ticks) {
    target_ticks = wanted_ticks - (MAXUINT32 - curr_ticks);
    // What should we do in this case? We know the maximum frequency, so we can
    // hard code a loop like this until we overflow
    while (ticks > MAXUINT32 / 2) {
      curr_pcb->is_yield = 1; // yield our time slice
    }
  } else {
    target_ticks = curr_ticks + wanted_ticks;
  }

  while (ticks < target_ticks) {
      curr_pcb->is_yield = 1; // yield our time slice
    }

  return 0;
}

/*
 * int32_t rtc_write()
 *
 * Inputs: nbytes - frequency
 * Return Value: 0 on success, -1 on failure
 * Function: Writes to the RTC
 */
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes) {
  int32_t freq;
  char rate;
  // make sure we're writing 4 bytes
  if((int32_t)buf == NULL || nbytes != NUM_BYTES) {
    return -1;
  }

  freq = *((int32_t*)buf);

  if (freq > MAX_FREQ) {
    return -1;
  }

  // set the per-process frequency information
  curr_pcb->rtc_freq = freq;

  // set the global rtc frequency if necessary
  if (freq > global_rtc_freq) {
    global_rtc_freq = freq;
    switch(freq) {
      case 1024: rate = RATE_1024; break;
      case 512: rate = RATE_512; break;
      case 256: rate = RATE_256; break;
      case 128: rate = RATE_128; break;
      case 64: rate = RATE_64; break;
      case 32: rate = RATE_32; break;
      case 16: rate = RATE_16; break;
      case 8: rate = RATE_8; break;
      case 4: rate = RATE_4; break;
      case 2: rate = RATE_2; break;
      case 0: rate = RATE_0; break;
      default: return -1;
    }

    /* write to status register A, disable non-maskable interrupts */
    outb(STATUS_REGISTER_A, OUT_RTC);
    /* read from status register A */
    unsigned char a = inb(IN_RTC);
    /* write to status register A */
    outb(STATUS_REGISTER_A, OUT_RTC);
    outb((0xF0 & a) | rate, IN_RTC);
  }

  return 0;
}
