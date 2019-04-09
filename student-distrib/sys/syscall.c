/**
 * syscall.c
 * 
 * This file holds the implementations of the files declared in syscall.h.
 */

#include "pcb.h"
#include "../fsys/fs.h"
#include "../bootinit/paging.h"
#include "../keyboard.h"
#include "../rtc.h"
#include "../x86_desc.h"
#include "syscall.h"

#define EIGHT_MB    0x00800000
#define FOUR_MB     0x00400000
#define EIGHT_KB    0x00002000
#define MB_128      0x08000000
#define NEW_ESP     (MB_128 + FOUR_MB - 4)
#define PROG_VADDR  0x08048000
#define MAX_PROCS   6 // maximum number of processes

/* declare the array holding the syscall function pointers */
/* unnecessary since the assembly table works (allows variable params) */
// static int32_t (*syscall_table[NUM_SYSCALLS])(int32_t, int32_t, int32_t);
int process_array[MAX_PROCS] = {0, 0, 0, 0, 0, 0};
int num_procs = 0;

/* static definitions of certain file operations */
static fops_t stdin_fops = {&terminal_read, &garbage_write, &terminal_open, &terminal_close};
static fops_t stdout_fops = {&garbage_read, &terminal_write, &terminal_open, &terminal_close};
static fops_t rtc_fops = {&rtc_read, &rtc_write, &rtc_open, &rtc_close};
static fops_t dir_fops = {&dir_read, &dir_write, &dir_open, &dir_close};
static fops_t file_fops = {&file_read, &file_write, &file_open, &file_close};
static fops_t null_fops = {&garbage_read, &garbage_write, &garbage_open, &garbage_close};

/* the magic numbers at the beginning of executables */
static uint8_t ELF[4] = {0x7f, 0x45, 0x4c, 0x46};

int32_t run_shell() {
  char com[6] = "shell";
  return system_execute((uint8_t*)com);
}

int32_t system_execute(const uint8_t* command) {
  if (command == NULL) {
    return -1;
  }

  /***** 1. Parse Command *****/
  int8_t filename[MAX_DIRNAME_LEN];
  int8_t arguments[MAXBUFFER];
  int32_t filename_idx = 0, space_flag = 0;
  int new_pid = -1;
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

  if ((i - filename_idx) > MAX_DIRNAME_LEN) {
    return -1; // filename too long
  }

  // Initialize arguments to the empty string
  arguments[0] = '\0';
  
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
  
  /* check PCB array for next process since
  * we must support up to 6 in kernel */
  for (i = 0; i < MAX_PROCS; i++) {
    if (process_array[i] == 0) {
      process_array[i] = 1;
      new_pid = i;
      break;
    }
  }

  // make sure we found a pid available
  if (new_pid < 0)
    return -1;
    

  /***** 3. Set Up Program Paging *****/
  /* 
   * The program image itself is linked to execute at virtual address
	 * 0x08048000. The way to get this working is to set up a single 4 MB page
	 * directory entry that maps virtual address 0x08000000 (128 MB) to the right
	 * physical memory address (either 8 MB or 12 MB). Then, the program image
	 * must be copied to the correct offset (0x00048000) within that page. 
   */

  int32_t phys_addr = EIGHT_MB + (new_pid * FOUR_MB);
  add_program_page((void*)phys_addr, 1);

  /***** 4. User-Level Program Loader *****/

	/* copy the entire file to memory starting at virtual address 0x08048000 */
  read_data(dir_entry.inode_num, 0, (uint8_t*) PROG_VADDR, FOUR_MB);

  /***** 5. Create Process Control Block (PCB) *****/

  // increment the number of processes
  num_procs++;

  // each pcb starts at the top of an 8KB block in the kernel
  pcb_t* new_pcb = (pcb_t*) (EIGHT_MB - (new_pid + 1) * EIGHT_KB);
  new_pcb->pid = new_pid; // set the pid, indexed at 0

  if (curr_pcb == NULL) {
    // we are executing the first process
    new_pcb->parent_pcb = NULL;
  } else {
    new_pcb->parent_pcb = curr_pcb;
  }

  // set the current pcb to be the new pcb
  curr_pcb = new_pcb;

  // copy parsed argument to the buffer in current PCB
	strcpy((int8_t*) (curr_pcb->arg_buf), arguments);

  /* set up the file descriptor tables */
  
  // first set the stdin/out fops
  curr_pcb->file_descs[0].fops_table = &stdin_fops;
  curr_pcb->file_descs[0].inode = dir_entry.inode_num;
  curr_pcb->file_descs[0].file_position = 0;
  curr_pcb->file_descs[0].flags = FD_IN_USE;
  curr_pcb->file_descs[1].fops_table = &stdout_fops;
  curr_pcb->file_descs[1].inode = dir_entry.inode_num;
  curr_pcb->file_descs[1].file_position = 0;
  curr_pcb->file_descs[1].flags = FD_IN_USE;

  // initialize the rest of the fd tables (starting with entry 2)
  for(i = 2; i < MAX_FDS; i++) {
    curr_pcb->file_descs[i].fops_table = &null_fops;
    curr_pcb->file_descs[i].inode = -1;
    curr_pcb->file_descs[i].file_position = 0;
    curr_pcb->file_descs[i].flags = FD_NOT_IN_USE;
  }

  /***** 6. Context Switch *****/

  // update the tss with the kernel stack info
  tss.ss0 = KERNEL_DS;
  tss.esp0 = EIGHT_MB - (curr_pcb->pid) * EIGHT_KB - 4;

  // save current ebp and esp
  asm volatile(
    "movl %%esp, %0;"
    "movl %%ebp, %1;"
    :"=g" (curr_pcb->parent_esp), "=g" (curr_pcb->parent_ebp) // outputs
    : // inputs
    : "memory" // clobbered registers
  );
  
  // switch to ring 3. This code is based on code from wiki.osdev.org
  asm volatile(
    "cli;"
    "mov %2, %%ax;"
    "mov %%ax, %%ds;"
    "mov %%ax, %%es;"
    "mov %%ax, %%fs;"
    "mov %%ax, %%gs;"
    "pushl %2;" // push user data segment with bottom 2 bits set for ring 3
    "pushl %3;" // push user ESP
    "pushfl;"
    "popl %%eax;"
    "orl $0x200, %%eax;" // set IF bit to 1 (bit 9)
    "pushl %%eax;" // push modified flags
    "pushl %1;" // push user code segment with bottom 2 bits set for ring 3
    "pushl %0;" // push entry point
    "iret;"
    : // output
    : "g" (entry_point), "g" (USER_CS), "g" (USER_DS), "g" (NEW_ESP) // input
    : "%eax", "memory" // clobbered registers
  );

  return -1; // we should never actually reach this specific return statment
}

int32_t system_halt(uint8_t status) {
  /* close relevant fds */
  int i;
  for (i = 0; i < MAX_FDS; i++) {
    // close the halted pcb's fds
    system_close(i);
    (curr_pcb->file_descs[i]).fops_table = &null_fops;
  }

  /* restore parent data */
  int32_t saved_esp = curr_pcb->parent_esp;
  int32_t saved_ebp = curr_pcb->parent_ebp;
  process_array[curr_pcb->pid] = 0;
  curr_pcb = curr_pcb->parent_pcb;
  num_procs--;
  
  // if we are out of processes, execute another shell
  if (num_procs == 0) {
    run_shell();
  }

  /* restore parent paging */
  int32_t phys_addr = EIGHT_MB + (curr_pcb->pid * FOUR_MB);
  add_program_page((void*)phys_addr, 1);

  /* jump to execute return */
  // set the tss esp0 to the current pcb's kernel stack
  tss.esp0 = EIGHT_MB - (curr_pcb->pid) * EIGHT_KB - 4;

  // restore the saved stack, and perform execute's return
  asm volatile(
    "movl %2, %%esp;"
    "movl %1, %%ebp;"
    "movl %0, %%eax;"
    "leave;" // execute's return
    "ret;"
    : // output
    : "g" ((uint32_t)status), "g" (saved_ebp), "g" (saved_esp)
    : "%eax" // clobbered registers
  );

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
  // find the correspondingly named file
  dentry_t dir_entry;
  if(read_dentry_by_name(filename, &dir_entry) < 0) {
    return -1; // fail
  }

  // allocate an unused file descriptor. make sure to start after stdin/out.
  int i;
  for (i = 2; i < MAX_FDS; i++) {
    if (curr_pcb->file_descs[i].flags == FD_NOT_IN_USE) {
      // open the file descriptor
      curr_pcb->file_descs[i].flags = FD_IN_USE;
      break;
    }
  }
  
  // make sure we found an empty file descriptor
  if (i == MAX_FDS) {
    return -1;
  }

  // initialize the file descriptor
  curr_pcb->file_descs[i].inode = dir_entry.inode_num;
  curr_pcb->file_descs[i].file_position = 0;
  
  // set up the proper jump table and open the file
  switch(dir_entry.file_type) {
    case FT_RTC: // rtc
      if (rtc_open(filename) < 0) {
        return -1;
      }
      curr_pcb->file_descs[i].fops_table = &rtc_fops;
      break;
    case FT_DIR: // directory
      if (dir_open(filename) < 0) {
        return -1;
      }
      curr_pcb->file_descs[i].fops_table = &dir_fops;
      break;
    case FT_REG: // file
      if (file_open(filename) < 0) {
        return -1;
      }
      curr_pcb->file_descs[i].fops_table = &file_fops;
      break;
    default: return -1;
  }
  return i; // return the file descriptor
}

int32_t system_close(int32_t fd) {

  /* check if valid file descriptor. note that the user should never be allowed
  to close the defaults descriptors (0 and 1). */
  if (fd < 2 || fd >= MAX_FDS) {
    return -1;
  }

  // check fd table to fd not in use
  if (curr_pcb->file_descs[fd].flags == FD_NOT_IN_USE) {
    return -1; // already closed!
  }

  // call the close function for that file
  if (curr_pcb->file_descs[fd].fops_table->close(fd) < 0) {
    return -1; // the close failed
  }

  // close the fd by setting it to not in use
  curr_pcb->file_descs[fd].flags = FD_NOT_IN_USE;

  return 0;
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
