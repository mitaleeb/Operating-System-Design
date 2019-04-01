/**
 * syscall.c
 * 
 * This file holds the implementations of the files declared in syscall.h.
 */

#include "syscall.h"

/* declare the array holding the syscall function pointers */
/* unnecessary since the assembly table works (allows variable params) */
// static int32_t (*syscall_table[NUM_SYSCALLS])(int32_t, int32_t, int32_t);

/* the magic numbers at the beginning of executables */
static uint8_t EXEC_IDENTIFIER[4] = {0x7f, 0x45, 0x4c, 0x46};

int32_t system_execute(const uint8_t* command) {
  return -1;
}

int32_t system_halt(uint8_t status) {
  return -1;
}

int32_t system_read(int32_t fd, void* buf, int32_t nbytes) {
  return -1;
}

int32_t system_write(int32_t fd, const void* buf, int32_t nbytes) {
  return -1;
}

int32_t system_open(const uint8_t* filename) {
  return -1;
}

int32_t system_close(int32_t fd) {
  return -1;
}

int32_t system_getargs(uint8_t* buf, int32_t nbytes) {
  return -1;
}

int32_t system_vidmap(uint8_t** screen_start) {
  return -1;
}

int32_t system_sethandler(int32_t signum, void* handler_address) {
  return -1;
}

int32_t system_sigreturn() {
  return -1;
}
