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

// initialize flag to check if interrupt has occurred
volatile int interrupt_flag = 0;


/* void init_rtc()
 * Inputs: none
 * Return Value: none
 * Function: Initializes RTC setting IRQ line
 */
extern void init_rtc() {
  /* clear interrupts */
  disable_irq(IRQ_RTC);
  /* 0x70, 0x71 - I/O ports */
  outb(STATUS_REGISTER_A,
       OUT_RTC); /* write to status register A, disable non-maskable interrupts */
  unsigned char a = inb(IN_RTC);   /* read from status register A */
  outb(STATUS_REGISTER_B, OUT_RTC);     /* write to status register B */
  outb(a | 0x40, IN_RTC); /* turn on 6th bit of status register */
                        /* RTC is IR0 on slave PIC - irq num is 8 */
  enable_irq(IRQ_RTC);
}

/* void handle_rtc_interrupt()
 * Inputs: none
 * Return Value: none
 * Function: Function to handle rtc interrupt
 */
extern void handle_rtc_interrupt() {
  /* clear interrupts */
  disable_irq(IRQ_RTC);
  /* UNCOMMENT THIS FUNCTION call to do RTC TEST */
  // test_interrupts();
  // send End of Interrupt Signal
  send_eoi(IRQ_RTC);

  /* read from status register C */
  outb(0x0C, OUT_RTC);
  inb(IN_RTC);
  /* set interrupts */
  enable_irq(IRQ_RTC);

  /* set interrupt flag */
  interrupt_flag = 1;

  /* do not return from handler in this checkpoint */
  // while(1);
}

/*
 * int32_t rtc_open()
 *
 * Inputs: none
 * Return Value: 0
 * Function: Opens the RTC
 */
int32_t rtc_open (const uint8_t* filename) {
  /* define var for frequency and set to min frequency (2 Hz) */
  char rate = RATE_2;

  /* write to status register A, disable non-maskable interrupts */
  outb(STATUS_REGISTER_A, OUT_RTC);
  /* read from status register A */
  unsigned char a = inb(IN_RTC);
  /* write min rate (2 Hz) to status register A */
  outb(STATUS_REGISTER_A, OUT_RTC);
  outb((0xF0 & a) | rate, IN_RTC);

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
  /* define var for frequency and set to min frequency (2 Hz) */
  char rate = RATE_2;

  /* write to status register A, disable non-maskable interrupts */
  outb(STATUS_REGISTER_A, OUT_RTC);
  /* read from status register A */
  unsigned char a = inb(IN_RTC);
  /* write min rate (2 Hz) to status register A */
  outb(STATUS_REGISTER_A, OUT_RTC);
  outb((0xF0 & a) | rate, IN_RTC);

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
  /* wait until interrupt occurs */
  while (interrupt_flag == 0) {}

  /* clear interrupt */
  interrupt_flag = 0;

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
  /* declare vars for frequency */
  char rate;
  int32_t freq;

  /* check if writing 4 bytes */
  if ((int32_t)buf == NULL || nbytes != 4) {
    return -1;
  } else {
    freq = *((int32_t*)buf);
  }

  /* set rate (Hz) */
  switch(freq) {
    case 1024:
      rate = RATE_1024;
      break;
    case 512: 
      rate = RATE_512;
      break;
    case 256:
      rate = RATE_256;
      break;
    case 128: 
      rate = RATE_128;
      break;
    case 64: 
      rate = RATE_64;
      break;
    case 32: 
      rate = RATE_32;
      break;
    case 16: 
      rate = RATE_16;
      break;
    case 8: 
      rate = RATE_8;
      break;
    case 4: 
      rate = RATE_4;
      break;
    case 2: 
      rate = RATE_2;
      break;
    case 0: 
      rate = RATE_0;
      break;
    default:
      return -1;
  }

  /* write to status register A, disable non-maskable interrupts */
  outb(STATUS_REGISTER_A, OUT_RTC);
  /* read from status register A */
  unsigned char a = inb(IN_RTC);
  /* write to status register A */
  outb(STATUS_REGISTER_A, OUT_RTC);
  outb((0xF0 & a) | rate, IN_RTC);

  return 0;
}
