/**
 * pcb.h - defines structs regarding the pcb
 */

#include "../types.h"

/* defines to make our code more readable */
#define MAX_FDS 8

typedef struct {
  int32_t (*read)(int32_t, int32_t, int32_t);
  int32_t (*write)(int32_t, int32_t, int32_t);
  int32_t (*open)(const uint8_t*);
  int32_t (*close)(int32_t);
} fops_t;

typedef struct {
  fops_t* fops_table;
  int32_t inode;
  int32_t file_position;
  int32_t flags;
} fd_entry_t;

typedef struct _pcb {
  fd_entry_t file_descs[MAX_FDS];
  int pid; // the process id, 0 for first shell
  struct _pcb* parent_pcb;
} pcb_t;

/* hold variables regarding processes */
pcb_t* curr_pcb; // pointer to the current pcb
int num_procs; // number of currently open processes
