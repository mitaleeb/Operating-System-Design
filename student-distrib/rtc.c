/**
 * rtc.c
 *
 * A file that holds functions that help initialize
 * and handle interrupts from the rtc.
 */

#include "i8259.h"
#include "lib.h"
#include "bootinit/idt.h"
#include "devices.h"


#define MASTER_PORT_A 0x20
#define SLAVE_PORT_A 0xA0
#define MASTER_START_INTERRUPT 0x20
#define SLAVE_START_INTERRUPT 0x28
#define SLAVE_END_INTERRUPT   SLAVE_START_INTERRUPT + 7
#define PIC_ACK     0x20
#define IRQ_RTC 			8

/* void init_rtc()
 * Inputs: none
 * Return Value: none
 * Function: Initializes RTC setting IRQ line
 */
extern void init_rtc() {
  /* clear interrupts */
  disable_irq(IRQ_RTC);
  /* 0x70, 0x71 - I/O ports
   * 0x8B - RTC status register */
  outb(0x8A,
       OUT_RTC); /* write to status register A, disable non-maskable interrupts */
  unsigned char a = inb(IN_RTC);   /* read from status register A */
  outb(0x8B, OUT_RTC);     /* write to status register B */
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
int32_t rtc_open () {
  /* define var for frequency and set to min frequency (2 Hz) */
  char rate = 0x0F;

  /* write to status register A, disable non-maskable interrupts */
  outb(0x8A, OUT_RTC);
  /* read from status register A */
  unsigned char a = inb(IN_RTC);
  /* write min rate (2 Hz) to status register A */
  outb(0x8A, OUT_RTC);
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
int32_t rtc_close () {
  /* define var for frequency and set to min frequency (2 Hz) */
  char rate = 0x0F;

  /* write to status register A, disable non-maskable interrupts */
  outb(0x8A, OUT_RTC);
  /* read from status register A */
  unsigned char a = inb(IN_RTC);
  /* write min rate (2 Hz) to status register A */
  outb(0x8A, OUT_RTC);
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
int32_t rtc_read () {
  /* wait until interrupt occurs */
  while (interrupt_flag == 0) {}

	/* clear interrupt */
	interrupt_flag = 0;

	return 0;
}

/*
 * int32_t rtc_write()
 *
 * Inputs: freq - frequency
 * Return Value: 0 on success, -1 on failure
 * Function: Writes to the RTC
 */
int32_t rtc_write (int32_t freq) {
  /* declare vars for frequency */
  char rate;

  /* set rate (Hz) */
  switch(freq) {
    case 1024: {
      rate = 0x06;
      break;
    } case 512: {
      rate = 0x07;
      break;
    } case 256: {
      rate = 0x08;
      break;
    } case 128: {
      rate = 0x09;
      break;
    } case 64: {
      rate = 0x0A;
      break;
    } case 32: {
      rate = 0x0B;
      break;
    } case 16: {
      rate = 0x0C;
      break;
    } case 8: {
      rate = 0x0D;
      break;
    } case 4: {
      rate = 0x0E;
      break;
    } case 2: {
      rate = 0x0F;
      break;
    } case 0: {
      rate = 0x00;
      break;
    }
    default:
      return -1;
  }

  /* write to status register A, disable non-maskable interrupts */
  outb(0x8A, OUT_RTC);
  /* read from status register A */
  unsigned char a = inb(IN_RTC);
  /* write to status register A */
  outb(0x8A, OUT_RTC);
  outb((0xF0 & a) | rate, IN_RTC);

	return 0;
}