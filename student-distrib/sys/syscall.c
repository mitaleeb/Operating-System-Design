/**
 * syscall.c
 * 
 * This file holds the implementations of the files declared in syscall.h.
 */

#include "syscall.h"
#include "pcb.h"
#include "../fsys/fs.h"
#include "../bootinit/paging.h"

#define EIGHT_MB    0x00800000
#define FOUR_MB     0x00400000
#define EIGHT_KB    0x00002000

/* declare the array holding the syscall function pointers */
/* unnecessary since the assembly table works (allows variable params) */
// static int32_t (*syscall_table[NUM_SYSCALLS])(int32_t, int32_t, int32_t);

/* the magic numbers at the beginning of executables */
static uint8_t ELF[4] = {0x7f, 0x45, 0x4c, 0x46};

int32_t system_execute(const uint8_t* command) {
  if (command == NULL) {
    return -1;
  }

  /***** 1. Parse Command *****/
  int8_t filename[32];
  int8_t arguments[128];
  int filename_idx = 0, space_flag = 0;
  int cmd_len = (int) strlen((int8_t*)command);

  // skip the leading spaces in command
  while (command[filename_idx] == ' ') {
    filename_idx++;
  }

  int i;
  // find the first space
  for (i = filename_idx; i < cmd_len; i++) {
    if (command[i] == ' ') {
      space_flag = 1;
      break; // finished finding filename
    }
  }

  // copy the info into our local variables
  if (space_flag) {
    // copy the filename
    strncpy(filename, (int8_t*) (command + filename_idx), (i - filename_idx));
    filename[i] = '\0'; // null terminate the filename
    i++;

    /* put the rest of the arguments into a different string */
    strcpy(arguments, (int8_t*)(command + i));
  } else {
    // just copy the filename
    strcpy(filename, (int8_t*) (command + filename_idx));
  }

  /***** 2. Executable Check *****/
  
  dentry_t dir_entry;
	/* check if valid file */
  if(read_dentry_by_name((uint8_t *) filename, &dir_entry) < 0)
		return -1;

	// if file is valid, check if executable by checking first 4 bytes
	uint8_t buf[4];
	read_data(dir_entry.inode_num, 0, buf, 4);
	if(buf[0] !=  ELF[0] || buf[1] != ELF[1] || buf[2] != ELF[2] || buf[3] != ELF[3])
		return -1;
  
  /* find entry point by getting 4-byte unsigned integer in bytes 24-27 */
  read_data(dir_entry.inode_num, 24, buf, 4);
  uint32_t entry_point = *((uint32_t*)buf);

  /***** 3. Set Up Program Paging *****/
  /* 
   * The program image itself is linked to execute at virtual address
	 * 0x08048000. The way to get this working is to set up a single 4 MB page
	 * directory entry that maps virtual address 0x08000000 (128 MB) to the right
	 * physical memory address (either 8 MB or 12 MB). Then, the program image
	 * must be copied to the correct offset (0x00048000) within that page. 
   */

  int32_t phys_addr = EIGHT_MB + (num_procs * FOUR_MB);
  add_program_page(phys_addr, 1);

  /***** 4. User-Level Program Loader *****/

	/* copy the entire file to memory starting at virtual address 0x08048000 */
  read_data(dir_entry.inode_num, 0, (uint8_t*) 0x08048000, FOUR_MB);

  /***** 5. Create Process Control Block (PCB) *****/

  // increment the number of processes
  num_procs++;

  // each pcb starts at the top of an 8KB block in the kernel
  pcb_t* new_pcb = (pcb_t*) (EIGHT_MB - (num_procs * EIGHT_KB));
  new_pcb->pid = num_procs - 1; // set the pid, indexed at 0

  if (curr_pcb == NULL) {
    // we are executing the first process
    new_pcb->parent_pcb = NULL;
  } else {
    new_pcb->parent_pcb = curr_pcb;
  }

  // set the current pcb to be the new pcb
  curr_pcb = new_pcb;

  // copy parsed argument to the buffer in current PCB
	// strcpy(curr_pcb->argument_buf, arguments);


  /* set up the file descriptor tables */

  /***** 6. Context Switch *****/


  return 34;
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
