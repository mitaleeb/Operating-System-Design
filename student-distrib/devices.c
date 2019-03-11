/**
 * devices.c
 *
 * A file that holds functions that help initialize
 * and handle interrupts from the keyboard and RTC.
 */

#include "i8259.h"
#include "lib.h"
#include "idt.h"
#include "devices.h"


#define MASTER_PORT_A 0x20
#define SLAVE_PORT_A 0xA0
#define MASTER_START_INTERRUPT 0x20
#define SLAVE_START_INTERRUPT 0x28
#define SLAVE_END_INTERRUPT   SLAVE_START_INTERRUPT + 7
#define PIC_ACK     0x20
#define IRQ_KEYBOARD 	1
#define IRQ_RTC 			8

// static uint8_t key_state = 0x00;
static uint8_t keyboard_output[128] = {
	 '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	 '\0', '\0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	 '\0', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	 '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '*',
	 '\0', ' ', '\0'
};
/* void init_keyboard()
 * Inputs: none
 * Return Value: none
 * Function: initialize keyboard device by masking IRQ line
 */
void init_keyboard() {
    /* keyboard is on IR1 of master PIC */
    enable_irq(IRQ_KEYBOARD);
    //printf("KEYboard initialized");
}
/* void handle_keyboard_interrupt()
 * Inputs: none
 * Return Value: none
 * Function: Handles keyboard interrupt and outputs key press to screen
 */
void handle_keyboard_interrupt() {
  	/* clear interrupts */
		disable_irq(IRQ_KEYBOARD);
		send_eoi(IRQ_KEYBOARD);
    /* 0x21 vector in the IDT was set to
     * handle_keyboard_interrupt */
    /* read from 0x60 = data port from keyboard controller */
    uint8_t c = 0x00; // character input
    do {
        if (inb(0x60) != 0) {
            c = inb(0x60);
            if (c > 0) {
                break;
            }
        }
    } while(1);
    /* print character to screen only on key press not release */
    if(c <128) {
			putc(keyboard_output[c]);
		}
    /* set interrupts */
		enable_irq(IRQ_KEYBOARD);
}

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
    outb(0x8A, 0x70);		    /* write to status register A, disable non-maskable interrupts */
    char a = inb(0x71);	        /* read from status register A */
    outb(0x8B, 0x70);		    /* write to status register B */
    outb(a | 0x40, 0x71);       /* turn on 6th bit of status register */
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
	// uncomment this function call to do RTC TEST
	//test_interrupts();
	// send End of Interrupt Signal
	send_eoi(IRQ_RTC);

	/* read from status register C */
	outb(0x0C, 0x70);
	inb(0x71);
	/* set interrupts */
  enable_irq(IRQ_RTC);

  // test by setting to certain frequency and whenever you get interrupt print stuff out
  // get more than one interrupt
  /* do not return from handler in this checkpoint */
  //while(1);
}
