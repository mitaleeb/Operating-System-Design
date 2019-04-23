/**
 * terminal.c
 *
 * A file that holds functions that support
 * multiple terminals.
 */

#include "i8259.h"
#include "keyboard.h"
#include "lib.h"
#include "sys/syscall.h"
#include "terminal.h"
#include "types.h"
#include "scheduler.h"

#define EIGHT_MB    0x00800000
#define FOUR_MB     0x00400000
#define EIGHT_KB    0x00002000
#define FOUR_KB     0x00001000

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25

/**
 * init_terminal()
 *
 * DESCRIPTION: constructor for terminal array (holds nultiple terminals)
 * INPUTS: NONE
 * OUTPUTS: NONE
 */
void init_terminal() {
// Initialize the buffers to null characters
	int i, j;
	for (i = 0; i < MAXBUFFER; i++) {
		for(j = 0; j < 3; j++) {
			terminal[j].new_term_buffer[i] = '\0';
			terminal[j].old_term_buffer[i] = '\0';
		}
	}
	// Initialize terminal struct and flags
	for(i = 0; i < MAX_TERMS; i++) {
		terminal[i].term_buffer_index = 0;
		terminal[i].term_screen_x = 0;
		terminal[i].term_screen_y = 0;
		terminal[i].is_started = 0;
		terminal[i].is_visible = 0;
	}
// Initiailize first terminal as visible and started
	terminal[0].is_started = 1;
	terminal[0].is_visible = 1;
	visible_terminal = 0;
}

/**
 * launch_terminal()
 *
 * DESCRIPTION: launches a new terminal (never before opened)
 * INPUTS: NONE
 * OUTPUTS: NONE
 */
void launch_terminal() {
	terminal[visible_terminal].is_started = 1;

	// TODO: context switch function call (double check this)
	// context_switch(curr_pcb->pid, -1);
	
	// instead of doing a context switch to the root process, just go back to the
	// idea of holding a flag and letting execute know that this is an initial
	// process AFTER SAVING THE STACK POINTERS HERE
	asm volatile(
		"movl %%esp, %0;"
		"movl %%ebp, %1;"
		: "=g" (curr_pcb->my_esp), "=g" (curr_pcb->my_ebp) : 
		: "memory"
	);
	executing_initial_shell = 1;
	run_shell();
}

/**
 * switch_terminal()
 *
 * DESCRIPTION: switches terminal based on key press
 * INPUTS: the terminal number to switch to
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int switch_terminal(int32_t switch_to) {
	int i;
	if (switch_to < 0 || switch_to >= MAX_TERMS)
		return -1;
	if (switch_to == visible_terminal)
		return 0;

	/* save current keyboard buffer */
	for (i = 0; i < MAXBUFFER; i++) {
		terminal[visible_terminal].new_term_buffer[i] = new_term_buffer[i];
		new_term_buffer[i] = terminal[switch_to].new_term_buffer[i];
		/* save old term keyboard buffer */
		terminal[visible_terminal].old_term_buffer[i] = old_term_buffer[i];
		old_term_buffer[i] = terminal[switch_to].old_term_buffer[i];
	}
  /* Updating position of buffer position in terminal */
	terminal[visible_terminal].term_buffer_index = term_buffer_index;
  term_buffer_index = terminal[switch_to].term_buffer_index;

  set_terminal_position(visible_terminal);
  update_screen(switch_to);

	// Update video page
	switch_video_page(switch_to, visible_terminal);

	/* Update visibility of terminal */
	cli();
	terminal[visible_terminal].is_visible = 0;
	visible_terminal = switch_to;
	terminal[visible_terminal].is_visible = 1;
	sti();

	/* check if terminal has ever been opened */
	if (terminal[visible_terminal].is_started == 0) {
		launch_terminal();
		return 0;
	}

	return 0;

}
