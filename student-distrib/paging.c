
#include "paging.h"
#define NOT_PRESENT       0x00000002
#define PRESENT           0x00000003

/* Function for initializing paging */
void paging_init() {
  /** given in doc - align pages in C:
   * int some_variable __attribute__((aligned (BYTES_TO_ALIGN_TO))); */

  /* split 0 – 4 MB into 4 KB page
   * since int is 4 bytes 1024 indeces - 4096 bytes should suffice */
  // align page directory
  int pages[1024] __attribute__((aligned (4096)));
  // align page table
  int table[1024] __attribute__((aligned (4096)));


  int i;
  for (i=0; i<1024; i++) {
    /* set first 4 MB and unused pages as not present */
    pages[i] = NOT_PRESENT;
  }

  /* add page table to the directory and mark it as present */
  pages[0] = ((unsigned int)table | PRESENT);
  /* doc says everything that isn't page for video memory
   * should be marked as not present - not sure if this is
   * video mem */


  /* turn on paging */
  asm volatile (
        pushl %ebp
        movl %esp, %ebp
        /*** CODE TO FILL HERE ***/
  );

}
