/**
 * syscall.c
 * 
 * This file holds the implementations of the files declared in syscall.h.
 */

#include "syscall.h"

/* declare the array holding the syscall function pointers */
static int32_t (*syscall_table[NUM_SYSCALLS])(int32_t, int32_t, int32_t);

// This function may be totally useless, depending on if the current linkage
// works (if it does, we can simply remove this and the static array above)
int32_t syscall_call(int num, int32_t arg1, int32_t arg2, int32_t arg3) {
  if (num < 1 || num > NUM_SYSCALLS) {
    return -1; // failure case
  }

  num--; // our table is indexed by 0

  // make sure our function call isn't null (not implemented yet)
  if (syscall_table[num]) {
    return (*syscall_table[num])(arg1, arg2, arg3);
  }

  return -1;
}

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

int32_t set_handler(int32_t signum, void* handler_address) {
  return -1;
}

int32_t sigreturn() {
  return -1;
}
