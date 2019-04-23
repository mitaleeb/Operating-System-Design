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

  /* similar to keyboard.c */
  uint8_t old_term_buffer[MAXBUFFER];
  uint8_t new_term_buffer[MAXBUFFER];
  uint8_t term_buffer_index;

  /* position - similar to screen_x, screen_y in lib.c */
  uint32_t term_screen_x;
  uint32_t term_screen_y;
} terminal_t;

/* Global Variables - used to update terminal */
int32_t visible_terminal;
int32_t next_terminal;
terminal_t terminal[3];

/*Function Definitions */
void init_terminal(void);
void launch_terminal();
int32_t switch_terminal(int32_t switch_to);

#endif
