/**
 * syscall.c
 * 
 * This file holds the implementations of the files declared in syscall.h.
 */

#include "syscall.h"
#include "pcb.h"

/* declare the array holding the syscall function pointers */
/* unnecessary since the assembly table works (allows variable params) */
// static int32_t (*syscall_table[NUM_SYSCALLS])(int32_t, int32_t, int32_t);

/* hold a pointer to the current pcb */
pcb_t* curr_pcb;

/* the magic numbers at the beginning of executables */
static uint8_t EXEC_IDENTIFIER[4] = {0x7f, 0x45, 0x4c, 0x46};

int32_t system_execute(const uint8_t* command) {
  return -1;
}

int32_t system_halt(uint8_t status) {
  /* restore parent data */

  /* restore parent paging */

  /* close relevant fds */

  /* jump to execute return */
  // asm("jmp EXECUTE_RETURN");

  return -1; // we should never get here
}

int32_t system_read(int32_t fd, void* buf, int32_t nbytes) {
  /* check if the file descriptor is valid */
  if (fd < 0 || fd >= MAX_FDS) {
    // Note that we do not need to check for NULL because some read calls may be
    // okay with NULL pointers. Let the invidiual read call check for NULL.
    return -1;
  }

  /* check if the fd is in use */
  if ((curr_pcb->file_descs[fd]).flags) {
    /* invoke the correct read call */
    return (curr_pcb->file_descs[fd]).fops_table->read(fd, buf, nbytes);
  }

  return -1; // if we get here, we return error
}

int32_t system_write(int32_t fd, const void* buf, int32_t nbytes) {
  /* check if the file descriptor is valid */
  if (fd < 0 || fd >= MAX_FDS) {
    return -1;
  }

  /* check if the fd is in use */
  if ((curr_pcb->file_descs[fd]).flags) {
    /* invoke the correct write call */
    return (curr_pcb->file_descs[fd]).fops_table->write(fd, buf, nbytes);
  }
    
  return -1; // if we get here, we return error
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
