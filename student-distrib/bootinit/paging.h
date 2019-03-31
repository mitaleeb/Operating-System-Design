/**
 * paging.h
 * 
 * Header file for the declarations for all things paging.
 */

#include "../lib.h"

/* number of entries in the page directory and page tables this is 4 KB and
 * since each entry is an int (4B) then we have 1024 entries */
#define MAX_ENTRIES 1024

/* definition for alignment at 4KB */
#define PAGE_4KB 4096

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
 * page_flushtlb()
 * 
 * DESCRIPTION: flushes the TLB. Necessary when changes made to paging
 * structures. 
 */
void page_flushtlb();

/**
 * paging_tester()
 *
 * DESCRIPTION: tests whether we have the correct values in our page descriptor
 * and initial page table.
 * INPUTS: none
 * OUTPUTS: 1 if successful, 0 otherwise
 */
int paging_tester();
