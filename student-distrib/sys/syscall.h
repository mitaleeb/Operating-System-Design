/**
 * syscall.h
 */

#include "../types.h"

/* some defines to make our code more readable */
#define EXEC_ADDR 0x08048000

/* the number of system calls */
#define NUM_SYSCALLS 6

/* functions to help us set up the ability to receive a syscall */
extern int32_t syscall_linker(); // prototype for assembly linkage

// This function may or may not be necessary, unsure yet.
int32_t syscall_call(int num, int32_t arg1, int32_t arg2, int32_t arg3);

/* the actual system calls (1-10)*/
int32_t system_halt(uint8_t status);

int32_t system_execute(const uint8_t* command);

int32_t system_read(int32_t fd, void* buf, int32_t nbytes);

int32_t system_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t system_open(const uint8_t* filename);

int32_t system_close(int32_t fd);

int32_t system_getargs(uint8_t* buf, int32_t nbytes);

int32_t system_vidmap(uint8_t** screen_start);

int32_t system_sethandler(int32_t signum, void* handler_address);

int32_t system_sigreturn(void);
