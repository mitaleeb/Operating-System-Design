/**
 * devices.c
 *
 * A file that holds functions that help initialize
 * and handle interrupts from the keyboard and writes
 * to the terminal.
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

#define MAXBUFFER 128

static int caps_flag = 0;
static int shift_flag = 0;
static int control_flag = 0;
static int enter_flag = 0;
static int backspace_flag = 0;
uint8_t old_term_buffer[MAXBUFFER];
uint8_t new_term_buffer[MAXBUFFER];
static int term_buffer_index = 0;
static int term_flag = 0;
static int column_index = 0;


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


/*
* int32_t terminal_open()
*   Inputs: none
*   Return Value: 0
*		Function: none
*/
int32_t terminal_open(){
	return 0;
}

/*
* int32_t terminal_close()
*   Inputs: none
*   Return Value: 0
*		Function: none
*/
int32_t terminal_close(){
	return 0;
}


/*
* int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t length)
*   Inputs: uint8_t* buf = buffer
*		int32_t length = length
*   Return Value: number of bytes read
*	Function: read keyboard buffer, clear keyboard buffer
* 	calling terminal read should give a clear buffer
*/
int32_t terminal_read(uint8_t* buf, int32_t length) {
	int i;

	/* check if invalid buffer is passed in */
	if(buf == NULL || length < 0)
		return 0;

	/* return unless buffer is ready to be read */
	if(!term_flag)
		return 0;

	/* read from input to old terminal buffer */
	if (length <= MAXBUFFER) {
		for(i = 0; i < length; i++) {
			old_term_buffer[i] = buf[i];
		}
		for(i = length; i < MAXBUFFER; i++) {
			old_term_buffer[i] = '\0';
		}
	 	// memcpy(buf, &(old_term_buffer), length)

	} else {
		for(i = 0; i < MAXBUFFER; i++) {
			old_term_buffer[i] = buf[i];
		}
	 	// memcpy(buf, &(old_term_buffer), MAXBUFFER)

	}

	/* clear new terminal buffer */
	for(i = 0; i < MAXBUFFER; i++) {
		new_term_buffer[i] = '\0';
	}

	/* reset flag so terminal is not ready to be read */
	term_flag = 0;

	/* return number of bytes read */
	if (length < MAXBUFFER) {
		return length;
	} else {
		return MAXBUFFER;
	}
}

/*
* int32_t terminal_write(int32_t fd, uint8_t* buf, int32_t length)
*   Inputs: uint8_t* buf = buffer
*		int32_t length = length
*   Return Value: number of bytes written
*	Function: write keyboard buffer, clear keyboard buffer
*
*/
int32_t terminal_write(uint8_t* buf, int32_t length) {
	int i;
	/* check if invalid buffer is passed in */
	if(buf == NULL || length < 0)
		return 0;

	for(i = 0; i < length; i ++) {
			putc(buf[i]);
	}
	return length;
}

/* void init_keyboard()
 * Inputs: none
 * Return Value: none
 * Function: initialize keyboard device by masking IRQ line
 */
void init_keyboard() {
		int i;
		/* clear terminal buffer */
		for(i = 0; i < MAXBUFFER; i++) {
			new_term_buffer[i] = '\0';
		}
		term_flag = 0;

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
					else if(c == CONTROL_RELEASE)
						control_flag = 0;
					/* check for capslock toggle */
					else if(c == CAPS_PRESS)
						caps_flag ^= 1;
				}
				else
				{
					/* check for capslock toggle */
					if(c == CAPS_PRESS)
						caps_flag ^= 1;
					/* check for button press */
					else if(c == BACKSPACE_PRESS)
						backspace_flag = 1;
					else if(c == ENTER_PRESS) {
						enter_flag = 1;
						term_flag = 1;
					}
					else if(c == CONTROL_PRESS)
						control_flag = 1;
					else if(c == LEFT_SHIFT_PRESS || c == RIGHT_SHIFT_PRESS)
						shift_flag = 1;

					/* if cntrl-l is pressed, clear screen */
					if(control_flag && c == L_PRESS) {
						clear();
						reset_position();
					}
					/* if backspace is pressed */
					else if(backspace_flag)
						backspace_buffer();
					/* if enter is pressed */
					else if(enter_flag)
						enter_buffer();
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
	if(term_buffer_index >= 127) {
		return;
	}
	if(column_index > 79) {
		enter_position();
		column_index = 0;
	}
	new_term_buffer[term_buffer_index] = k;
	putc(k);
	term_buffer_index++;
	column_index++;
}
/* void backspace_buffer(void)
 * Inputs: none
 * Return Value: none
 * Function: Handles keyboard input for when backspace key is pressed
 */
extern void backspace_buffer(void) {
	if(term_buffer_index > 0) {
		term_buffer_index--;
		new_term_buffer[term_buffer_index] = '\0';
		decrement_position();
		putc(' ');
		decrement_position();
		column_index--;
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
		 old_term_buffer[i] = new_term_buffer[i];
		 /* clear new buff */
		 new_term_buffer[i] = '\0';
	 }
	 term_buffer_index = 0;
	 column_index = 0;
	 enter_position();
	 enter_flag = 0;
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
