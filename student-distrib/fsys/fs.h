/**
 * fs.h
 *
 * Header file for fs.c. Declares all functions used in fs.c. Also holds
 * definitions for structs used in the filesystem.
 */

#include "../lib.h"

/* Defines for certain maximums/constants in the filesystem */
#define MAX_DIRNAME_LEN 32
#define BLOCK_SIZE 4096 // 4 KB
#define DENTRY_SIZE 64 // 64 B
#define MAX_DENTRIES 63
#define DBLOCKS_PER_INODE 1023

/* Defines for file types */
#define FT_RTC 0
#define FT_DIR 1
#define FT_REG 2
/* Global variable for file system location */
unsigned int file_system_loc;
int curr_directory;

/**
 * inode_t - a struct that holds the data for an inode (4 KB).
 * 
 * This data is: length in bytes - 4 B
 *               index of data blocks - 1023 B
 * 
 * Note: we use int32_t purposely here such that we can be sure we are using 32
 * bit integers regardless of whether that is what an integer is defined as in
 * this system (though in mp3 it should be the same). Same with the following
 * declarations.
 */
typedef struct {
  int32_t length;
  int32_t dblocks[DBLOCKS_PER_INODE];
} inode_t;

/**
 * dentry_t - a struct that holds the data for a dentry (64 B).
 *
 * This data is: file name - 32 B 
 *               file type - 4 B 
 *               inode num - 4 B 
 *               reserved  - 24 B
 */
typedef struct {
  uint8_t file_name[MAX_DIRNAME_LEN];
  int32_t file_type;
  int32_t inode_num;
  uint8_t reserved[24]; // 24 bytes reserved
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
typedef struct {
  int32_t num_dentries;
  int32_t num_inodes;
  int32_t num_data_blocks;
  uint8_t reserved[52]; // 52 bytes reserved
  dentry_t dentries[MAX_DENTRIES];
} bootblock_t;

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

/**
 * file_open()
 * 
 * DESCRIPTION:  “Opening” these files consists of storing appropriate jump tables in these two locations in the file array,
 *                and marking the files as in-use
 * INPUTS: fname - the filename to open
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int32_t file_open (const uint8_t* fname);

/**
 * file_read()
 * 
 * DESCRIPTION:  Reads bytes from a file and copies the bytes read into a buffer
 * INPUTS: fd -- file descriptor
 *         buf -- starting pointer of copy data buffer
 *         nbytes -- number of bytes to read
 * OUTPUTS: total bytes read if successful, -1 otherwise
 */  
int32_t file_read (int32_t fd, void* buf, int32_t nbytes);

/**
 * file_write()
 * 
 * DESCRIPTION:  Writes to a file from buffer
 * INPUTS: fd -- file descriptor
 *         buf -- starting pointer of copy data buffer
 *         nbytes -- number of bytes to read
 * OUTPUTS: Always -1 because this is a read only file system
 */  
int32_t file_write (int32_t fd, void* buf, int32_t nbytes);

/**
 * file_close()
 * 
 * DESCRIPTION:  Close file in use
 * INPUTS: fd -- file descriptor for file to close
 * OUTPUTS: 0 if successful, -1 otherwise
 */  
int32_t file_close (int32_t fd);

/**
 * dir_open()
 * 
 * DESCRIPTION:  Opens a directory
 * INPUTS: fname - the filename to open
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int32_t dir_open (const uint8_t* fname);

/**
 * dir_read()
 * 
 * DESCRIPTION:  Outputs name of directory to buffer
 * INPUTS: fd -- file descriptor
 *         buf -- starting pointer of copy data buffer
 *         nbytes -- number of bytes to read
 * OUTPUTS: total bytes read if successful, -1 otherwise
 */  
int32_t dir_read (int32_t fd, void* buf, int32_t nbytes);

/**
 * dir_write()
 * 
 * DESCRIPTION:  Writes to a directory
 * INPUTS: fd -- file descriptor
 *         buf -- starting pointer of copy data buffer
 *         nbytes -- number of bytes to read
 * OUTPUTS: Always -1 because this is a read only file system
 */  
int32_t dir_write (int32_t fd, void* buf, int32_t nbytes);

/**
 * dir_close()
 * 
 * DESCRIPTION:  Close directory that is open
 * INPUTS: fd -- file descriptor for directory to close
 * OUTPUTS: 0 if successful, -1 otherwise
 */  
int32_t dir_close (int32_t fd);

