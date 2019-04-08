/**
 * pcb.c - definitions of functions in pcb.h
 */

#include "pcb.h"

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
