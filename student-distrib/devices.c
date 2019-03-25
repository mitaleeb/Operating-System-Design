/**
 * devices.c
 *
 * A file that holds functions that help initialize
 * and handle interrupts from the keyboard and RTC.
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
#define IRQ_KEYBOARD 	1
#define IRQ_RTC 			8


#define CAPS_PRESS 0x3A
#define LEFT_SHIFT_PRESS 0x2A
#define LEFT_SHIFT_RELEASE 0xAA
#define RIGHT_SHIFT_PRESS 0x36
#define RIGHT_SHIFT_RELEASE 0xB6
#define CONTROL_PRESS 0x1D
#define CONTROL_RELEASE 0x9D
#define L_PRESS 0x26
#define ENTER_PRESS 0x1C
#define BACKSPACE_PRESS 0x0E


static int caps_flag = 0;
static int shift_flag = 0;
static int control_flag = 0;
static int enter_flag = 0;
static int backspace_flag = 0;
uint8_t *term_buffer;


// static uint8_t key_state = 0x00;
// keyboard_output1 for regular input
static uint8_t keyboard_output1[128] = {
	 '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	 '\0', '\0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	 '\0', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	 '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '\0',
	 '\0', ' ', '\0'
};

// keyboard_output2 for when shift is pressed
static uint8_t keyboard_output2[128] = {
	 '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	 '\0', '\0', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
	 '\0', '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',
	 '\0', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', '\0', '\0',
	 '\0', ' ', '\0'
};

//keyboard_output3 for when capslock is pressed
static uint8_t keyboard_output3[128] = {
	 '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	 '\0', '\0', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
	 '\0', '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
	 '\0', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', '\0', '\0',
	 '\0', ' ', '\0'
};

//keyboard_output4 for when capslock AND shift is pressed
static uint8_t keyboard_output4[128] = {
	 '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	 '\0', '\0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}',
	 '\0', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~',
	 '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', '\0', '\0',
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
    uint8_t c = 0x00;
	  do {
			  /* read from 0x60 = data port from keyboard controller */
				c = inb(0x60);
				/* if key code is negative, then button has been released */
        if (inb(0x60) & 0x80)
				{
					/* check for button release */
					if(c == LEFT_SHIFT_RELEASE || c == RIGHT_SHIFT_RELEASE)
						shift_flag = 0;
					if(c == CONTROL_RELEASE)
						control_flag = 0;
					/* check for capslock toggle */
					if(c == CAPS_PRESS)
						caps_flag ^= 1;

				}
				else
				{
					/* check for capslock toggle */
					if(c == CAPS_PRESS)
						caps_flag ^= 1;
					/* check for button press */
					if(c == CONTROL_PRESS)
						control_flag = 1;
					if(c == LEFT_SHIFT_PRESS || c == RIGHT_SHIFT_PRESS)
						shift_flag = 1;


					if(control_flag && c == L_PRESS) {
						clear();

					}
					/*if shift and caps */
					else if(shift_flag && caps_flag) {
						if (keyboard_output4[c] != '\0')
							putc(keyboard_output4[c]);
					}
					/*if only shift*/
					else if(shift_flag && !caps_flag) {
						if (keyboard_output2[c] != '\0')
							putc(keyboard_output2[c]);
					}
					/*if only caps*/
					else if (!shift_flag && caps_flag) {
						if (keyboard_output3[c] != '\0')
							putc(keyboard_output3[c]);
					}
					/*if neither*/
					else {
						if (keyboard_output1[c] != '\0')
							putc(keyboard_output1[c]);
					}
				}
				break;

    } while(1);


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

/* void handle_systemcall_interrupt()
 * Inputs: none
 * Return Value: none
 * Function: Function to handle systemcall interrupt
 */
void handle_systemcall_interrupt() {
	/* acknowledge systemcall interrupt() */
	printf("System call successful! \n");
}
