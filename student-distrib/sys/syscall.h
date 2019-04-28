/**
 * syscall.h
 */
#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "../types.h"

/* some defines to make our code more readable */
#define EXEC_ADDR 0x08048000

/* the number of system calls */
#define NUM_SYSCALLS 6

/* global variable to indicate that we are executing an initial shell */
int executing_initial_shell;

/* functions to help us set up the ability to receive a syscall */
extern int32_t syscall_linker(); // prototype for assembly linkage

// This function is unnecessary since asm linkage works
// int32_t syscall_call(int num, int32_t arg1, int32_t arg2, int32_t arg3);

/**
 * run_shell()
 *
 * DESCRIPTION: literally runs system_execute with argument "shell"
 */
int32_t run_shell();

/* the actual system calls (1-10)*/

/**
 * system_halt
 *
 * DESCRIPTION: corresponds to system call 1. Halts the program and returns
 *              control the parent.
 * INPUTS: status - the return status for the program
 * OUTPUTS: nothing, should jump to execute return.
 */
int32_t system_halt(uint8_t status);

/**
 * system_execute
 *
 * DESCRIPTION: corresponds to system call 2. Executes the given command.
 * INPUTS: command - the command and arguments to the command, space separated.
 * OUTPUTS: the return status, if successful. -1 otherwise.
 */
int32_t system_execute(const uint8_t* command);

/**
 * system_read
 *
 * DESCRIPTION: corresponds to system call 3. Executes the read function of the
 *              specified file descriptor.
 * INPUTS: fd - the file descriptor to read
 *         buf - the buffer to read to
 *         nbytes - the number of bytes to read.
 * OUTPUTS: return value of the specific read call if successful, -1 otherwise.
 */
int32_t system_read(int32_t fd, void* buf, int32_t nbytes);

/**
 * system_write
 *
 * DESCRIPTION: corresponds to system call 4. Executes the write function of the
 *              specified file descriptor.
 * INPUTS: fd - the file descriptor to write to
 *         buf - the buffer to write from
 *         nbytes - the number of bytes to write.
 * OUTPUTS: return value of the specific write call if successful, -1 otherwise.
 */
int32_t system_write(int32_t fd, const void* buf, int32_t nbytes);

/**
 * system_open
 *
 * DESCRIPTION: corresponds to system call 5. Opens the specified file, making
 *              sure to call the open function of the right type of file.
 * INPUTS: filename - the filename of the file to open
 * OUTPUTS: the file descriptor allocated to this file
 */
int32_t system_open(const uint8_t* filename);

/**
 * system_close
 *
 * DESCRIPTION: corresponds to system call 6. Closes the specified file
 *              descriptor.
 * INPUTS: fd - the file descriptor to close
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int32_t system_close(int32_t fd);

/**
 * system_getargs
 *
 * DESCRIPTION: gets program arguments
 * INPUTS: buf- buffer to copy arguments to
 *         nbytes- bytes to copy
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int32_t system_getargs(uint8_t* buf, int32_t nbytes);

/**
 * system_vidmap
 *
 * DESCRIPTION: set virtual memory to video memory
 * INPUTS: screen_start- pointer to starting location in video memorty
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int32_t system_vidmap(uint8_t** screen_start);

int32_t system_sethandler(int32_t signum, void* handler_address);

int32_t system_sigreturn(void);

#endif
