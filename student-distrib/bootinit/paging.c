/**
 * paging.c
 * 
 * Implementation of the functions required for paging.
 */

#include "paging.h"

/* definitions for the flags for page table and directory entires */
#define GLOBAL              0x00000100
#define PAGE_4MB            0x00000080
#define PAGE_CACHE_ENABLE   0x00000010
#define USER_LEVEL          0x00000004
#define READ_WRITE          0x00000002
#define PRESENT             0x00000001

/* definitions for the beginning of certain segments */
#define KERNEL_ADDR 0x400000
#define VIDEO_ADDR 0xB8000

/* definitions for some useful macros for indexing the page directory */
#define PD_IDX(x) (x >> 22)
#define PT_IDX(x) (x >> 12 & 0x03FF)

/* static variables to hold certain paging items */
static page_directory_t page_directory __attribute__ ((aligned (PAGE_4KB)));
static page_table_t page_table_1 __attribute__ ((aligned (PAGE_4KB)));

/* static function declarations */
static void add_page_dir_entry(void* phys_addr, void* virtual_addr, uint32_t flags);
static void add_page_table_entry(void* phys_addr, void* virt_addr, uint32_t flags);

/**
 * page_init()
 *
 * DESCRIPTION: Initializes the page table according to our specifications. This
 * means allocating 4KB pages for addresses 0-4 MB, and one 4 MB page for the
 * kernel at address 4 MB. The rest will be set to be uninitialized for now. 
 */
void page_init()
{
    // clear all pages in the directory and the table
    int i;
    for (i = 0; i < MAX_ENTRIES; i++) {
        page_directory.page_directory_entries[i] = 0;
        page_table_1.page_table_entries[i] = 0;
    }

    /* add the first page table (governing pages 0 - 4MB) to the directory */
    uint32_t flags = READ_WRITE | PRESENT;
    add_page_dir_entry(&(page_table_1), 0, flags);

    /* add the 4MB Kernel page to the page directory */
    flags = GLOBAL | PAGE_4MB | PAGE_CACHE_ENABLE | READ_WRITE | PRESENT;
    add_page_dir_entry((void*)KERNEL_ADDR, (void*)KERNEL_ADDR, flags);

    /* add a page table entry for video memory into the page table */
    // TODO: NEEDS TO BE DONE STILL (Where is video memory?)
    flags = READ_WRITE | PRESENT;
    // add_page_table_entry((void*)VIDEO_ADDR, (void*) , flags);

    /* Turn on Paging in assembly. This is done in the following steps: */
    /* 1. Copy the page directory into cr3 */
    /* 2. Enable PSE (4 MB Pages) */
    /* 3. Set the paging and protection bits of cr0 */
    asm(
        "movl %0, %%eax;"
        "movl %%eax, %%cr3;"
        "movl %%cr4, %%eax;"
        "orl $0x00000010, %%eax;"
        "movl %%eax, %%cr4;"
        "movl %%cr0, %%eax"
        "orl $0x80000001, %%eax;"
        "movl %%eax, %%cr0;"
        : : "r" (page_directory)
        : "%eax"
    );
}

/**
 * add_page_dir_entry()
 * 
 * DESCRIPTION: static helper function to help us add a page directory entry to
 * the page directory. Parts of the code were referenced from
 * wiki.osdev.org/Paging. 
 * INPUTS: phys_addr - the physical address to map (usually a ptr to a table)
 *         virt_addr - the virtual address to map
 *         flags - a combination of the flags to pass into the lower 9 bits of
 *                 entry
 * SIDE-EFFECTS: writes to the page_directory structure present in this file.
 */
static void add_page_dir_entry(void* phys_addr, void* virt_addr, uint32_t flags) {
    // Make sure we check if our physical address was null
    if (phys_addr == NULL)
        return;
    
    // get the index in the page directory
    int pd_idx = PD_IDX((uint32_t) virt_addr);
    
    // add the entry
    page_directory.page_directory_entries[pd_idx] = ((int)virt_addr | flags);
}

/**
 * add_page_table_entry()
 * 
 * DESCRIPTION: static helper function to help us add a page table entry to the
 * page directory. Like add_page_dir_entry, most of this code is based off
 * wiki.osdev.org/paging.
 * INPUTS: phys_addr - the physical address to map
 *         virt_addr - the virtual address to map
 *         flags - a combination of the flags to pass into the lower 9 bits of
 *                 entry
 * SIDE-EFFECTS: writes to the page_table_1 structure present in this file.
 */
static void add_page_table_entry(void* phys_addr, void* virt_addr, uint32_t flags) {
    // Make sure we check if our physical address was null
    if (phys_addr == NULL)
        return;
    
    // get the index in the page table
    int pt_idx = PT_IDX((uint32_t) virt_addr);

    // add the entry
    page_table_1.page_table_entries[pt_idx] = ((int) virt_addr | flags);
}
