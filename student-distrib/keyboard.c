/**
 * keyboard.c
 *
 * A file that holds functions that help initialize
 * and handle interrupts from the keyboard and writes
 * to the terminal.
 */

#include "i8259.h"
#include "lib.h"
#include "bootinit/idt.h"
#include "keyboard.h"
#include "terminal.h"
#include "scheduler.h"
#include "sys/pcb.h"
#include "constants.h"

#define MASTER_PORT_A 0x20
#define SLAVE_PORT_A 0xA0
#define MASTER_START_INTERRUPT 0x20
#define SLAVE_START_INTERRUPT 0x28
#define SLAVE_END_INTERRUPT   SLAVE_START_INTERRUPT + 7
#define PIC_ACK     0x20
#define KEYBOARD_PORT 0x60
#define HIGH_BITMASK 0x80
#define TERM_COLS 80
#define TERM_ROWS 25

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

#define ALT_PRESS		0x38
#define ALT_RELEASE	0xB8
#define F1_PRESS		0x3B
#define F2_PRESS		0x3C
#define F3_PRESS		0x3D

static int caps_flag = 0;
static int shift_flag = 0;
static int control_flag = 0;
static int alt_flag = 0;
static int enter_flag = 0;
static int backspace_flag = 0;

// PS2 keyboard scancode can be seen here:
// http://www.quadibloc.com/comp/scan.htm

/* keyboard_output1 for regular input */
static uint8_t keyboard_output1[128] = {
	 '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	 '\0', '\0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	 '\0', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	 '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '\0',
	 '\0', ' ', '\0'
};

/* keyboard_output2 for when shift is pressed */
static uint8_t keyboard_output2[128] = {
	 '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	 '\0', '\0', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
	 '\0', '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',
	 '\0', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', '\0', '\0',
	 '\0', ' ', '\0'
};

/*keyboard_output3 for when capslock is pressed*/
static uint8_t keyboard_output3[128] = {
	 '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	 '\0', '\0', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
	 '\0', '\0', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
	 '\0', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', '\0', '\0',
	 '\0', ' ', '\0'
};

/*keyboard_output4 for when capslock AND shift is pressed*/
static uint8_t keyboard_output4[128] = {
	 '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	 '\0', '\0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}',
	 '\0', '\0', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~',
	 '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', '\0', '\0',
	 '\0', ' ', '\0'
};


/*
* int32_t terminal_open()
*   Inputs: filename - Doesn't matter
*   Return Value: 0 if successful, -1 otherwise
*		Function: open terminal driver
*/
int32_t terminal_open(const uint8_t* filename){
	return 0;
}

/*
* int32_t terminal_close()
*   Inputs: fd - file descriptor to close (doesn't matter)
*   Return Value: 0
*		Function: close terminal driver
*/
int32_t terminal_close(int32_t fd){
	return 0;
}


/*
* int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t length)
*   Inputs: uint8_t* buf = buffer
*		int32_t length = length
*   int32_t fd = file decorator
*   Return Value: number of bytes read
*		Function: read keyboard buffer, clear keyboard buffer
* 	calling terminal read should give a clear buffer
*/
int32_t terminal_read(int32_t fd, void* buf, int32_t length) {
	int i;
	/* check if invalid buffer is passed in */
	if(buf == NULL || length < 0)
		return 0;

	/* return unless buffer is ready to be read */
	/* I'm pretty sure it doesnt matter which read_flag we read bc they technically should be the same */
	while(!terminal[curr_pcb->term_index].read_ready) {
		curr_pcb->is_yield = 1; // yield our time slice
	}

	/* read from input to old terminal buffer */
	cli();
	if (length < terminal[curr_pcb->term_index].old_tbi) {
		memcpy(buf, &(terminal[curr_pcb->term_index].old_term_buffer), length);
	} else {
		memcpy(buf, &(terminal[curr_pcb->term_index].old_term_buffer), terminal[curr_pcb->term_index].old_tbi);
	}
	sti();

	/* clear new terminal buffer */
	for(i = 0; i < MAXBUFFER; i++) {
		terminal[curr_pcb->term_index].new_term_buffer[i] = '\0';
	}

	/* reset flag so terminal is not ready to be read */
	terminal[curr_pcb->term_index].read_ready = 0;

	/* return number of bytes read */
	if (length < terminal[curr_pcb->term_index].old_tbi) {
		return length;
	} else {
		return terminal[curr_pcb->term_index].old_tbi;
	}
}

/*
* int32_t terminal_write(int32_t fd, uint8_t* buf, int32_t length)
*   Inputs: uint8_t* buf = buffer
*		int32_t length = length
*   int32_t fd = file decorator
*   Return Value: number of bytes written
*	Function: write keyboard buffer, clear keyboard buffer
*
*/
int32_t terminal_write(int32_t fd, const void* buf, int32_t length) {
	int i, j;

	/* check if invalid buffer is passed in */
	if(buf == NULL || length < 0)
		return 0;

	/* check to ensure buf.size > length, otherwise reassign length */
	int buflen = strlen((int8_t*) buf);
	if(buflen < length)
		length = buflen;
  /* print each buffer value to terminal */
	for(i = 0; i < length; i ++) {
		cli();
		if (visible_terminal == curr_pcb->term_index) {
			putc(((uint8_t*)buf)[i]);
		} else {
			term_putc(((uint8_t*)buf)[i]);
		}
		sti();
		if(((uint8_t*)buf)[i] == '\n') {
			for(j = 0; j < MAXBUFFER; j++) {
				terminal[curr_pcb->term_index].new_term_buffer[j] = '\0';
			}
			terminal[curr_pcb->term_index].term_buffer_index = 0;
		}
	}

	/*return the copied length, if successful */
	return length;
}

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

    uint8_t c = 0x00;
	  do {
			  /* read from 0x60 = data port from keyboard controller */
				c = inb(KEYBOARD_PORT);
				/* if key code is negative, then button has been released */
        if (c & HIGH_BITMASK)
				{
					/* check for button releases */
					if(c == LEFT_SHIFT_RELEASE || c == RIGHT_SHIFT_RELEASE)
						shift_flag = 0;
					else if(c == CONTROL_RELEASE)
						control_flag = 0;
					else if(c == ALT_RELEASE)
						alt_flag = 0;
					/* check for capslock toggle */
					else if(c == CAPS_PRESS)
						caps_flag ^= 1;
				}
				else    /* button is being pressed */
				{
					/* check for capslock toggle */
					if(c == CAPS_PRESS)
						caps_flag ^= 1;
					/* check for button press */
					else if(c == BACKSPACE_PRESS)
						backspace_flag = 1;
					else if(c == ENTER_PRESS) {
						enter_flag = 1;
					}
					else if(c == CONTROL_PRESS)
						control_flag = 1;
					else if(c == ALT_PRESS)
						alt_flag = 1;
					else if(c == LEFT_SHIFT_PRESS || c == RIGHT_SHIFT_PRESS)
						shift_flag = 1;


					/* if cntrl-l is pressed, clear screen */
					if(control_flag && c == L_PRESS) {
						clear();
						reset_position();
					}
					/* if ALT-F1 is pressed, switch to terminal 1 */
					else if(alt_flag && c == F1_PRESS) {
						if (num_procs < MAX_PROCS || terminal[FIRST_TERM].is_started == 1)
							switch_terminal(FIRST_TERM);
					}
					/* if ALT-F2 is pressed, switch to terminal 2 */
					else if(alt_flag && c == F2_PRESS) {
						if (num_procs < MAX_PROCS || terminal[SECOND_TERM].is_started == 1)
							switch_terminal(SECOND_TERM);
					}
					/* if ALT-F3 is pressed, switch to terminal 3 */
					else if(alt_flag && c == F3_PRESS) {
						if (num_procs < MAX_PROCS || terminal[THIRD_TERM].is_started == 1)
							switch_terminal(THIRD_TERM);
					}

					/* if cntrl-l is pressed, clear screen */
					if(control_flag && c == L_PRESS) {
						clear();
						reset_position();
						//update_cursor();
					}
					/* if control is held down, do not print any characters */
					else if(control_flag)
						break;
					/* if backspace is pressed */
					else if(backspace_flag)
						backspace_buffer();
					/* if enter is pressed */
					else if(enter_flag) {
						write_to_buffer('\n');
						enter_buffer();
						terminal[visible_terminal].read_ready = 1;
					}
					/*if shift and caps */
					else if(shift_flag && caps_flag) {
						if (keyboard_output4[c] != '\0')
							write_to_buffer(keyboard_output4[c]);
					}
					/*if only shift*/
					else if(shift_flag && !caps_flag) {
						if (keyboard_output2[c] != '\0')
							write_to_buffer(keyboard_output2[c]);
					}
					/*if only caps*/
					else if(!shift_flag && caps_flag) {
						if (keyboard_output3[c] != '\0')
							write_to_buffer(keyboard_output3[c]);
					}
					/*if neither*/
					else {
						if (keyboard_output1[c] != '\0')
							write_to_buffer(keyboard_output1[c]);
					}
				}
				break;

    } while(1);


    /* set interrupts */
		enable_irq(IRQ_KEYBOARD);
}

/* void write_to_buffer(uint8_t k)
 * Inputs: the character to write to the terminal buffer
 * Return Value: none
 * Function: Handles keyboard input and writes to buffer to be written to screen
 */
extern void write_to_buffer(uint8_t k) {
	/* check if end of buffer has been reached */
	if(terminal[visible_terminal].term_buffer_index >= MAXBUFFER - 2 && k != '\n') {
		return;
	}

	/* If it hasnt, write to terminal */
	terminal[visible_terminal].new_term_buffer[terminal[visible_terminal].term_buffer_index] = k;
	putc(k);
	terminal[visible_terminal].term_buffer_index++;
}

/* void backspace_buffer(void)
 * Inputs: none
 * Return Value: none
 * Function: Handles keyboard input for when backspace key is pressed
 */
extern void backspace_buffer(void) {
	/* check to ensure beginning of line is not reached */
	if(terminal[visible_terminal].term_buffer_index > 0) {
		terminal[visible_terminal].term_buffer_index--;
		terminal[visible_terminal].new_term_buffer[terminal[visible_terminal].term_buffer_index] = '\0';
		decrement_position();
		putc(' ');
		decrement_position();
		update_cursor();
	}
	backspace_flag = 0;
}

/* void enter_buffer(void)
 * Inputs: none
 * Return Value: none
 * Function: Handles keyboard input for when enter key is pressed
 */
 extern void enter_buffer(void) {
	 int i;
	 for(i = 0; i < MAXBUFFER; i++)
	 {
		 /*copy new buff into old buff */
		 terminal[visible_terminal].old_term_buffer[i] = terminal[visible_terminal].new_term_buffer[i];
		 /* clear new buff */
		 terminal[visible_terminal].new_term_buffer[i] = '\0';
	 }
	 terminal[visible_terminal].old_tbi = terminal[visible_terminal].term_buffer_index;
	 terminal[visible_terminal].term_buffer_index = 0;
	 //enter_position();
	 //update_cursor();
	 enter_flag = 0;
 }
