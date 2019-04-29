/**
 * terminal.h
 *
 * h file that holds definitions of functions, including all the
 * functions in terminal.c
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "keyboard.h"

typedef struct {
  /* flags */
  int32_t is_started;
  int32_t is_visible;
  int32_t read_ready;

  /* similar to keyboard.c */
  uint8_t old_term_buffer[MAXBUFFER];
  uint8_t new_term_buffer[MAXBUFFER];
  uint8_t term_buffer_index;
  uint8_t old_tbi;

  /* position - similar to screen_x, screen_y in lib.c */
  uint32_t term_screen_x;
  uint32_t term_screen_y;
} terminal_t;

/* Global Variables - used to update terminal */
int32_t visible_terminal;
int32_t next_terminal;
terminal_t terminal[3];

/* Function Definitions */

/**
 * init_terminal()
 *
 * DESCRIPTION: constructor for terminal array (holds nultiple terminals)
 * INPUTS: NONE
 * OUTPUTS: NONE
 */
void init_terminal(void);

/**
 * launch_terminal()
 *
 * DESCRIPTION: launches a new terminal (never before opened)
 * INPUTS: NONE
 * OUTPUTS: NONE
 */
void launch_terminal();

/**
 * switch_terminal()
 *
 * DESCRIPTION: switches terminal based on key press
 * INPUTS: the terminal number to switch to
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int32_t switch_terminal(int32_t switch_to);

#endif