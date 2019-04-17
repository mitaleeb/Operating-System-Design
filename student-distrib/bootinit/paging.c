/**
 * paging.c
 * 
 * Implementation of the functions required for paging.
 */

#include "paging.h"

/* definitions for the flags for page table and directory entires */
#define GLOBAL              0x00000100
#define PAGE_4MB            0x00000080
#define PAGE_CACHE_DISABLE  0x00000010
#define USER_LEVEL          0x00000004
#define READ_WRITE          0x00000002
#define PRESENT             0x00000001

/* definitions for the beginning of certain segments */
#define KERNEL_ADDR 0x400000
#define VIDEO_ADDR  0xB8000
#define VIDEO_ADDR1 (VIDEO_ADDR)
#define VIDEO_ADDR2 (VIDEO_ADDR1 + PAGE_4KB)
#define VIDEO_ADDR3 (VIDEO_ADDR2 + PAGE_4KB)
#define PROG_VADDR  0x08000000


/* definitions for some useful macros for indexing the page directory */
#define PD_IDX(x) (x >> 22)
#define PT_IDX(x) ((x >> 12) & 0x03FF)

/* static variables to hold certain paging items */
static page_directory_t page_directory __attribute__ ((aligned (PAGE_4KB)));
static page_table_t page_table_1 __attribute__ ((aligned (PAGE_4KB)));
static page_table_t page_table_2 __attribute__ ((aligned (PAGE_4KB)));

/* static function declarations */
static void add_page_dir_entry(void* phys_addr, void* virtual_addr, uint32_t flags);
static void add_page_table_entry(page_table_t * page_table, void* phys_addr, void* virt_addr, uint32_t flags);
static void page_flushtlb();

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
        page_table_2.page_table_entries[i] = 0;
    }

    /* add the first page table (governing pages 0 - 4MB) to the directory */
    uint32_t flags = READ_WRITE | PRESENT; 
    add_page_dir_entry(&(page_table_1), 0, flags);

    /* add the 4MB Kernel page to the page directory */
    flags = GLOBAL | PAGE_4MB | READ_WRITE | PRESENT;
    add_page_dir_entry((void*)KERNEL_ADDR, (void*)KERNEL_ADDR, flags);

    /* create a user-level page table that maps to video memory */
    flags = READ_WRITE | PRESENT | USER_LEVEL| PAGE_CACHE_DISABLE; 
    add_page_dir_entry(&(page_table_2), (void*)VIRT_VIDEO_ADDR, flags);

    /* set up paging for video memory */
    switch_video_page(1); // initially start in terminal 1

    /* Turn on Paging in assembly. This is done in the following steps: */
    /* 1. Copy the page directory into cr3 */
    /* 2. Enable PSE (4 MB Pages) */
    /* 3. Set the paging and protection bits of cr0 */
    asm volatile (
        "movl %0, %%eax;"
        "movl %%eax, %%cr3;"
        "movl %%cr4, %%eax;"
        "orl $0x00000010, %%eax;"
        "movl %%eax, %%cr4;"
        "movl %%cr0, %%eax;"
        "orl $0x80000001, %%eax;"
        "movl %%eax, %%cr0;"
        : 
        : "g" (&page_directory)
        : "%eax", "memory"
    );
}

/**
 * add_program_page()
 * 
 * DESCRIPTION: adds or removes a 4 MB page to our page directory for a program
 * (shell or executed by shell) Maps to 128 MB
 * INPUTS: phys_addr - pointer to the physical address to be mapped
 *         adding - 1 if we want to add the page, 0 if we want to remove
 * OUTPUTS: none
 */
void add_program_page(void* phys_addr, int adding) {
    uint32_t flags = USER_LEVEL | PAGE_4MB | READ_WRITE;
    if (adding) {
        // we are adding the page, so set it to present
        flags = flags | PRESENT;
    }

    // call our static helper function to allocate the page dir entry
    add_page_dir_entry(phys_addr, (void*) PROG_VADDR, flags);

    page_flushtlb(); // flush the tlb
}

/**
 * switch_video_page()
 * 
 * DESCRIPTION: switches both the user and kernel's video memory pages to be
 *              pointing to the specified terminal's physical video memory.
 * INPUTS: term_index - the terminal number to switch to (1-3)
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int switch_video_page(int term_index) {
    // figure out which terminal we are switching to
    uint32_t vaddr;
    if (term_index == 1) {
        vaddr = VIDEO_ADDR1;
    } else if (term_index == 2) {
        vaddr = VIDEO_ADDR2;
    } else if (term_index == 3) {
        vaddr = VIDEO_ADDR3;
    } else {
        return -1;
    }

    // switch the kernel-level page to point to the correct terminal's video
    uint32_t flags = PAGE_CACHE_DISABLE | READ_WRITE | PRESENT;
    add_page_table_entry(&(page_table_1), (void*)vaddr, (void*)VIDEO_ADDR, flags);

    // switch the user-level page to point to the correct terminal's video
    flags = READ_WRITE | PRESENT | USER_LEVEL| PAGE_CACHE_DISABLE ;
    add_page_table_entry(&(page_table_2), (void*)vaddr, (void*)VIRT_VIDEO_ADDR, flags);
    
    page_flushtlb(); // flush the tlb
    return 0; // success
}

/**
 * page_flushtlb()
 * 
 * DESCRIPTION: flushes the TLB. Necessary when changes made to paging
 */
static void page_flushtlb() {
    /* according to wiki.osdev.org/TLB, we can flush the TLB by simply writing
       to the PDBR (CR3) */
    asm volatile ("movl %%cr3, %%eax;"
                  "movl %%eax, %%cr3;"
                  : : : "%eax");
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
    page_directory.page_directory_entries[pd_idx] = ((int)phys_addr | flags);
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
static void add_page_table_entry(page_table_t* page_table, void* phys_addr, void* virt_addr, uint32_t flags) {
    // Make sure we check if our physical address was null
    if (phys_addr == NULL)
        return;
    
    // get the index in the page table
    int pt_idx = PT_IDX((uint32_t) virt_addr);

    // add the entry
    page_table->page_table_entries[pt_idx] = ((int)phys_addr | flags);
}

/**
 * paging_tester()
 *
 * DESCRIPTION: tests whether we have the correct values in our page descriptor
 * and initial page table.
 * INPUTS: none
 * OUTPUTS: 0 if successful, > 0 otherwise
 */
int paging_tester() {
    int result = 0;

    // define the flags expected for our directory/table values
    uint32_t dir_ent_0_flags = READ_WRITE | PRESENT;
    uint32_t dir_ent_1_flags = GLOBAL | PAGE_4MB | READ_WRITE | PRESENT;
    uint32_t tab_ent_vid_flags = PAGE_CACHE_DISABLE | READ_WRITE | PRESENT;

    // check the directory and table values
    if (page_directory.page_directory_entries[0] != (((int)(&page_table_1)) | dir_ent_0_flags) && 
        page_directory.page_directory_entries[0] != (((int)(&page_table_1)) | dir_ent_0_flags | 0x60) &&
        page_directory.page_directory_entries[0] != (((int)(&page_table_1)) | dir_ent_0_flags | 0x40) &&
        page_directory.page_directory_entries[0] != (((int)(&page_table_1)) | dir_ent_0_flags | 0x20)) {
        result++;
    }

    if (page_directory.page_directory_entries[1] != (int)(KERNEL_ADDR | dir_ent_1_flags) && 
        page_directory.page_directory_entries[1] != (int)(KERNEL_ADDR | dir_ent_1_flags | 0x60) &&
        page_directory.page_directory_entries[1] != (int)(KERNEL_ADDR | dir_ent_1_flags | 0x40) &&
        page_directory.page_directory_entries[1] != (int)(KERNEL_ADDR | dir_ent_1_flags | 0x20)) {
        result++;
    }

    if (page_table_1.page_table_entries[PT_IDX(VIDEO_ADDR1)] != (int)(VIDEO_ADDR1 | tab_ent_vid_flags) &&
        page_table_1.page_table_entries[PT_IDX(VIDEO_ADDR1)] != (int)(VIDEO_ADDR1 | tab_ent_vid_flags | 0x60) &&
        page_table_1.page_table_entries[PT_IDX(VIDEO_ADDR1)] != (int)(VIDEO_ADDR1 | tab_ent_vid_flags | 0x40) &&
        page_table_1.page_table_entries[PT_IDX(VIDEO_ADDR1)] != (int)(VIDEO_ADDR1 | tab_ent_vid_flags | 0x20)) {
        result++;
    }

    return result;
}
