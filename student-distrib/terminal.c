/**
 * terminal.c
 *
 * A file that holds functions that support
 * multiple terminals.
 */

#include "i8259.h"
#include "keyboard.h"
#include "lib.h"
#include "bootinit/paging.h"
#include "sys/pcb.h"
//#include "scheduling.h"
#include "sys/syscall.h"
#include "terminal.h"
#include "types.h"

#define EIGHT_MB    0x00800000
#define FOUR_MB     0x00400000
#define EIGHT_KB    0x00002000
#define FOUR_KB     0x00001000

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25

// terminal_t terminal1;
// terminal_t terminal2;
// terminal_t terminal3;

/*
 *   init_term()
 *     Function: Initializes 3 terminals
 */
void init_term() {
	uint32_t i, j;

  /* Separate input buffer per terminal, save current text
   * screen/cursor position/anything relevant to your OS */

	for (i = 0; i < 3; i++) {
		terminal[i].term_num = i;
		terminal[i].curr_process_num = -1;
    /* no terminals running */
		terminal[i].flag = 0;
		terminal[i].shell_flag = 0;

    /* initialize keyboard buffer */
		terminal[i].term_buffer_index = 0;
		for (j = 0; j < MAXBUFFER; j++) {
			terminal[i].new_term_buffer[j] = '\0';
		}

    /* update video memory */
    terminal[i].term_screen_x = 0;
		terminal[i].term_screen_y = 0;

		/* map to address for current terminal (paging)
     * NOTE: can be allocated anywhere, we chose
     * virtual addr between kernel and program image */
		// terminal_paging_user((void*)0x7000000, (void*)(0x7000000 - ((i+1)*FOUR_KB)));

		/* clear video memory - similar to clear in lib.c */
  	// terminal[i].video_mem = (uint8_t*)(0x7000000 - ((i+1)*FOUR_KB));
		for (j = 0; j < (NUM_ROWS * NUM_COLS); j++) {
			terminal[i].video_mem[(j << 1)] = ' ';
		}
	}

  /* set global variables - 1st terminal always launched first */
	terminal[0].flag = 1;
	terminal[0].shell_flag = 1;

	term_buffer_index = terminal[0].term_buffer_index;
	for (i = 0; i < MAXBUFFER; i++) {
		new_term_buffer[i] = terminal[0].new_term_buffer[i];
	}

  update_screen(0);
  memcpy((uint8_t*)VIDEO, (uint8_t*)terminal[0].video_mem, (2 * NUM_ROWS * NUM_COLS));

	curr_terminal = 0;
  /* launch a terminal so one is always running */
	run_shell();
}

/*
 *   uint32_t launch_term(uint8_t switch_to)
 */
uint32_t launch_term(uint32_t switch_to) {

	int i;

  /* check if number of terminal we are launching
   * is out of bounds or already launched */
	if (switch_to < 0 || switch_to > 2)
		return -1;
	if (switch_to == curr_terminal)
		return 0;

	/* check if terminal is already running */
	if (terminal[switch_to].flag == 1) {
    curr_terminal = switch_to;
		for (i = 0; i < MAXBUFFER; i++) {
			new_term_buffer[i] = terminal[switch_to].new_term_buffer[i];
		}

		return 0;
	}

  /* save current keyboard buffer */
	for (i = 0; i < MAXBUFFER; i++) {
		new_term_buffer[i] = terminal[switch_to].new_term_buffer[i];
	}
	terminal[curr_terminal].term_buffer_index = term_buffer_index;
  term_buffer_index = terminal[switch_to].term_buffer_index;

  set_terminal_position(curr_terminal);
  update_screen(switch_to);

  /* save current video memory before switching to new terminal */
  // memcpy((uint8_t*)terminal[curr_terminal].video_mem, (uint8_t*)VIDEO, (2 * NUM_ROWS * NUM_COLS));
  // memcpy((uint8_t*)VIDEO, (uint8_t*)terminal[switch_to].video_mem, (2 * NUM_ROWS * NUM_COLS));
	switch_video_page(switch_to, curr_terminal);

  /* update current terminal number and mark it as running */
  curr_terminal = switch_to;
	for (i = 0; i < 3; i++) {
		terminal[i].flag = 0;
	}
	terminal[curr_terminal].flag = 1;

	/* save current ebp and esp - similar to syscall.c */
  // pcb_t* saved_pcb = (pcb_t*)(EIGHT_MB - (terminal[curr_terminal].curr_process_num + 1) * EIGHT_KB);
	//
	// asm volatile(
  //   "movl %%esp, %0;"
  //   "movl %%ebp, %1;"
  //   :"=g" (saved_pcb->esp), "=g" (saved_pcb->ebp) // outputs
  //   : // inputs
  //   : "memory" // clobbered registers
  // );

	if (terminal[curr_terminal].shell_flag == 0) {
		terminal[curr_terminal].shell_flag = 1;
		run_shell();
	}

	return 0;
}
