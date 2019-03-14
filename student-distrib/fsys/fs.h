/**
 * fs.h
 *
 * Header file for fs.c. Declares all functions used in fs.c. Also holds
 * definitions for structs used in the filesystem.
 */

#include "../types.h"
#include "../lib.h"

/* Defines for certain maximums/constants in the filesystem */
#define MAX_DIRNAME_LEN 32
#define BLOCK_SIZE 4096 // 4 KB
#define DENTRY_SIZE 64 // 64 B
#define MAX_DENTRIES 63

/* Defines for file types */
#define FT_RTC 0
#define FT_DIR 1
#define FT_REG 2

/**
 * dentry_t - a struct that holds the data for a dentry (64 B).
 *
 * This data is: file name - 32 B 
 *               file type - 4 B 
 *               inode num - 4 B 
 *               reserved  - 24 B
 *
 * Note: we use int32_t purposely here such that we can be sure we are using 32
 * bit integers regardless of whether that is what an integer is defined as in
 * this system (though in mp3 it should be the same). 
 */
typedef struct dentry_struct {
  uint8_t file_name[MAX_DIRNAME_LEN];
  int32_t file_type;
  int32_t inode_num;
  uint8_t reserved[24]; // 24 reserved bytes
} dentry_t;

/**
 * bootblock_t - a struct that holds the data for the boot block (4KB).
 * 
 * This data is: # dir entries - 4 B
 *               # inodes (N)  - 4 B
 *               # data blocks - 4 B
 *               reserved      - 52 B
 *               dentry_t's    - 63 of them
 */
typedef struct bootblock_struct {
  int32_t num_dentries;
  int32_t num_inodes;
  int32_t num_data_blocks;
  uint8_t reserved[52]; // 52 bytes reserved
  dentry_t dentries[MAX_DENTRIES];
} bootblock_t;

/* Declaration of functions to perform the reading */

/**
 * read_dentry_by_name()
 * 
 * DESCRIPTION: Copies the dentry with name fname into the pointer dentry.
 * INPUTS: fname - the filename of the dentry
 *         dentry - the pointer to a struct to write the dentry data to
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);

/**
 * read_dentry_by_index()
 * 
 * DESCRIPTION: Copies the dentry with index index into the pointer dentry.
 * INPUTS: index - the index in the dentries array
 *         dentry - the pointer to a struct to write the dentry data to
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);

/**
 * read_data()
 * 
 * DESCRIPTION: reads up to length bytes starting from position offset in file
 *              inode, and puts the data into BUFF.
 * INPUTS: inode - inode number of file to read
 *         offset - offset to start reading at
 *         buf - the buffer to write to
 *         length - the number of bytes to read
 * OUTPUTS: number of bytes read (0 means EOF)
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);


