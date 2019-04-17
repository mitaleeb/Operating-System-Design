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
  uint8_t term_num;
	uint8_t curr_process_num;
  uint8_t flag;
  uint8_t shell_flag;

  /* similar to keyboard.c */
  uint8_t old_term_buffer[MAXBUFFER];
  uint8_t new_term_buffer[MAXBUFFER];
  uint8_t term_buffer_index;

  /* position - similar to screen_x, screen_y in lib.c */
  uint32_t term_screen_x;
  uint32_t term_screen_y;

  /* video memory pointer */
  uint8_t video_mem[4000];
} terminal_t;

/* Global Variables - used to update terminal */
uint8_t curr_terminal;
uint8_t next_terminal;
terminal_t terminal[3];

/*Function Definitions */
extern void init_term(void);
extern uint32_t launch_term(uint32_t num);

#endif
