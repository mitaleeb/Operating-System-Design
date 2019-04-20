/**
 * paging.h
 * 
 * Header file for the declarations for all things paging.
 */

#include "../lib.h"

/* 4 megabytes */
#define FOUR_MB 0x00400000

/* number of entries in the page directory and page tables this is 4 KB and
 * since each entry is an int (4B) then we have 1024 entries */
#define MAX_ENTRIES 1024

/* definition for alignment at 4KB */
#define PAGE_4KB 4096

/* definition for certain addresses in virtual memory */
#define VIRT_VIDEO_ADDR 0x08400000

/**
 * page_directory_t
 * 
 * DESCRIPTION: a struct that holds entries of page tables.
 */
typedef struct page_directory_struct {
    int page_directory_entries[MAX_ENTRIES];
} page_directory_t;

/**
 * page_table_t
 * 
 * DESCRIPTION: a struct that holds entries of pages.
 */
typedef struct page_table_struct {
    int page_table_entries[MAX_ENTRIES];
} page_table_t;

/**
 * page_init()
 * 
 * DESCRIPTION: Initializes our pages. See the c file for more details
 * INPUTS: none
 * OUTPUTS: none
 */
void page_init();

/**
 * add_program_page()
 * 
 * DESCRIPTION: adds or removes a 4 MB page to our page directory for a program
 * (shell or executed by shell) Maps to 128 MB
 * INPUTS: phys_addr - pointer to the physical address to be mapped
 *         adding - 1 if we want to add the page, 0 if we want to remove
 * OUTPUTS: none
 */
void add_program_page(void* phys_addr, int adding);

/**
 * switch_video_page()
 * 
 * DESCRIPTION: switches the kernel's video memory page to be pointing to the
 *              specified terminal's physical video memory.
 * INPUTS: term_to - the terminal number to switch to (0-2)
 *         term_from - the terminal number switching from (0-2)
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int switch_video_page(int term_to, int term_from);

/**
 * request_user_video()
 * 
 * DESCRIPTION: returns the user-level pointer to video memory for the specified
 *              terminal index.
 * INPUTS: term_index - the terminal number
 * OUTPUTS: the user-level video pointer. 0 if unsuccessful.
 */
uint8_t* request_user_video(int term_index);

/**
 * paging_tester()
 *
 * DESCRIPTION: tests whether we have the correct values in our page descriptor
 * and initial page table.
 * INPUTS: none
 * OUTPUTS: 1 if successful, 0 otherwise
 */
int paging_tester();
