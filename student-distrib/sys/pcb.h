/**
 * pcb.h - defines structs regarding the pcb
 */
#ifndef _PCB_H
#define _PCB_H

#include "../types.h"

/* defines to make our code more readable */
#define MAX_FDS       8    // maximum number of file descriptors
#define FD_IN_USE     1
#define FD_NOT_IN_USE 0
#define ARG_BUF_SIZE  128  // size in characters of arg buff
#define MAX_PROCS     6    // maximum number of processes
#define MAX_TERMS (MAX_PROCS / 2) // each term needs to run at least 2 procs

/**
 * fops_t - a struct to hold the file operation jump table
 *
 * The data is:
 *    read - a function pointer to the read function
 *    write - a function pointer to the write function
 *    open - a function pointer to the open function
 *    close - a function pointer to the close function
 */
typedef struct {
  int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
  int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
  int32_t (*open)(const uint8_t* filename);
  int32_t (*close)(int32_t fd);
} fops_t;

/**
 * fd_entry_t - a struct to hold the data for a file descriptor
 *
 * The data is:
 *    fops_table - a pointer to the fops_t struct for this fd
 *    inode - the inode number of this file
 *    file_position - the position we are in the file, initialize to 0
 *    flags - flags about the file descriptor. Currently 0 when not in use.
 */
typedef struct {
  fops_t* fops_table;
  int32_t inode;
  int32_t file_position;
  int32_t flags;
} fd_entry_t;

/**
 * pcb_t - a struct to hold data for the pcb
 *
 * The data is:
 *    file_descs - the array of MAX_FDS file descriptors
 *    pid - the process id of this process
 *    parent_pcb - a pointer to the parent pcb
 *    parent_esp - the esp to return to upon halting
 *    parent_ebp - the ebp to return to upon halting
 *    my_esp - the pcb's current esp upon leaving it's context
 *    my_ebp - the pcb's curren ebp upon leaving it's context
 *    arg_buf - a buffer to hold the text arguments (space separated)
 */
typedef struct _pcb {
  fd_entry_t file_descs[MAX_FDS];
  int pid; // the process id, 0 for first shell
  int term_index; // which terminal this process is executing in
  struct _pcb* parent_pcb;
  uint32_t parent_esp;
  uint32_t parent_ebp;
  uint32_t my_esp;
  uint32_t my_ebp;
  int8_t arg_buf[ARG_BUF_SIZE];
} pcb_t;

/* hold variables regarding processes */
pcb_t* curr_pcb; // pointer to the current pcb
int num_procs; // the total number of running processes

pcb_t root_pcb; // root of pcb tree which will hvae terminal_pcbs as children
int process_array[MAX_PROCS]; // an array that holds the pid statuses
pcb_t* terminal_pcbs[MAX_TERMS];


/**
 * init_pcb()
 *
 * DESCRIPTION: the function called during the boot process to initialize the
 * pcb structures.
 */
void init_pcb();

/* declare some garbage operations that return -1 */
int32_t garbage_read(int32_t fd, void* buf, int32_t nbytes);

int32_t garbage_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t garbage_open(const uint8_t* filename);

int32_t garbage_close(int32_t fd);

#endif
