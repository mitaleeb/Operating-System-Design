/**
 * fs.c
 * 
 * This file holds the functions to read/parse the read-only filesystem.
 */

#include "fs.h"

/* @TODO: get the bootblock pointer in some way. Right now I'm just assuming
it's a pointer we get somehow. */
bootblock_t* bootblock;

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
  
  // Check null pointers
  if (fname == NULL || dentry == NULL) {
    return -1;
  }

  // loop through all the dentries until we find one with a name that matches
  int32_t num_dentries = bootblock->num_dentries;

  // sanity check making sure num_dentries is less than max dentries
  if (num_dentries > MAX_DENTRIES){ 
    return -1;
  }
  int i;
  for (i = 0; i < num_dentries; i++) {
    if (strncmp(bootblock->dentries[i].file_name, fname, strlen(fname))) {
      // copy the dentry struct from here into the dentry_t parameter
      // we know the dentry struct is DENTRY_SIZE bytes, so we can just use memcpy
      memcpy(dentry, &(bootblock->dentries[i]), DENTRY_SIZE);
      
      // we found and executed the copy, we can just return now
      return 0;
    }
  }

  // did not find a matching file, return error
  return -1;
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
  // Check for null pointer
  if (dentry == NULL) {
    return -1;
  }

  int32_t num_dentries = bootblock->num_dentries;

  // Make sure our index is in a valid range
  if (index >= num_dentries) {
    return -1;
  }

  // find the dentry with index index and mcmpy it into the parameter
  memcpy(dentry, &(bootblock->dentries[index]), DENTRY_SIZE);
  return 0; // success!
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
  int32_t bytes_read = 0;
  int32_t num_inodes = bootblock->num_inodes;

  // the starting index, (offset / BLOCK_SIZE, since offset is in bytes)
  int dblock_index = offset / BLOCK_SIZE;

  // Check to make sure we have valid parameters
  if (inode >= num_inodes || buf == NULL) {
    return -1;
  }

  // find the file with inode index "inode"
  // remember to +1 to the inode index because inodes are zero indexed
  inode_t* src_file = (inode_t*) (bootblock + (inode + 1) * BLOCK_SIZE);

  // get the start dblock location
  int32_t dblock_ptr = src_file->dblocks[dblock_index];

  // get the pointer to the starting data block
  void* curr_dblock = (void*) (bootblock + (num_inodes + 1 + dblock_ptr) * BLOCK_SIZE);

  // start the pointer at a location in the data block based on the offset
  curr_dblock += (offset % BLOCK_SIZE);

  // keep track of how far into the file we are, in bytes
  uint32_t file_progress = offset;

  // loop until we've copied at most "length" bytes, or reached EOF
  while (bytes_read < length || file_progress >= src_file->length) {
    memcpy(buf, curr_dblock, 1); // copy a byte
    
    // increment all loop variables
    bytes_read++;
    file_progress++;

    // check if we're out of range of the curr_dblock
    if ((bytes_read + offset) % (BLOCK_SIZE) == 0) {
      // go to next dblock
      dblock_index++;
      dblock_ptr = src_file->dblocks[dblock_index];
      curr_dblock = (void*) (bootblock + (num_inodes + 1 + dblock_ptr) * BLOCK_SIZE);
    }
  }

  return bytes_read;
}


/**
 * file_open()
 * 
 * DESCRIPTION:  “Opening” these files consists of storing appropriate jump tables in these two locations in the file array,
 *                and marking the files as in-use
 * INPUTS: fname - the filename to open
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int32_t file_open (const uint8_t* fname){
  return 0;
}

/**
 * file_read()
 * 
 * DESCRIPTION:  Reads bytes from a file and copies the bytes read into a buffer
 * INPUTS: fd -- file descriptor
 *         buf -- starting pointer of copy data buffer
 *         num_b -- number of bytes to read
 * OUTPUTS: total bytes read if successful, -1 otherwise
 */  
int32_t file_read (int32_t fd, void* buf, int32_t nbytes){
  // call some helper function in order to read the data in the file
  return 0;
}

/**
 * file_write()
 * 
 * DESCRIPTION:  Writes to a file from buffer
 * INPUTS: fd -- file descriptor
 *         buf -- starting pointer of copy data buffer
 *         num_b -- number of bytes to read
 * OUTPUTS: Always -1 because this is a read only file system
 */  
int32_t file_write (int32_t fd, void* buf, int32_t nbytes){
  return -1;
}

/**
 * file_close()
 * 
 * DESCRIPTION:  Close file in use
 * INPUTS: fd -- file descriptor for file to close
 * OUTPUTS: 0 if successful, -1 otherwise
 */  
int32_t file_close (int32_t fd){
  return 0;
}

