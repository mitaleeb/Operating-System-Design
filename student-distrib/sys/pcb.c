/**
 * pcb.c - definitions of functions in pcb.h
 */

#include "pcb.h"

void init_pcb() {
  // instantiate the process array
  int i;
  for (i = 0; i < MAX_PROCS; i++) {
    process_array[i] = 0;
  }
  // instantiate the terminal pcbs to be not started
  for (i = 0; i < MAX_TERMS; i++) {
    terminal_pcbs[i] = NULL;
  }
  // initialize number of processes
  num_procs = 0;
  // set the first pcb to be null to indicate we have no executed process
  curr_pcb = &root_pcb;
  root_pcb.parent_pcb = NULL;
  root_pcb.pid = -1;
  root_pcb.term_index = -1;
  root_pcb.parent_esp = NULL;
  root_pcb.parent_ebp = NULL;
  //root_pcb.file_descs = NULL;
}

/* define some garbage read/write/open/close functions */

int32_t garbage_read(int32_t fd, void* buf, int32_t nbytes) {
  // avoid warnings
  (void)fd;
  (void)buf;
  (void)nbytes;
  return -1;
}
int32_t garbage_write(int32_t fd, const void* buf, int32_t nbytes) {
  // avoid warnings
  (void)fd;
  (void)buf;
  (void)nbytes;
  return -1;
}
int32_t garbage_open(const uint8_t* filename) {
  // avoid warnings
  (void)filename;
  return -1;
}
int32_t garbage_close(int32_t fd) {
  // avoid warnings
  (void)fd;
  return -1;
}
