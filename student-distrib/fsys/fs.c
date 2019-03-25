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

  // Set the start of bootblock struct to the start of file system
  bootblock = (bootblock_t*) file_system_loc;

  // loop through all the dentries until we find one with a name that matches
  int32_t num_dentries = bootblock->num_dentries;

  // sanity check making sure num_dentries is less than max dentries
  if (num_dentries > MAX_DENTRIES){ 
    return -1;
  }
  int i;
  for (i = 0; i < num_dentries; i++) {
    if (!strncmp(bootblock->dentries[i].file_name, fname, strlen(fname))) {
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
  bootblock = (bootblock_t*) file_system_loc;
  int32_t num_dentries = bootblock->num_dentries;

  // Make sure our index is in a valid range
  if (index >= num_dentries || index <0) {
    return -1;
  }

  // find the dentry with index index and mcmpy it into the parameter
  memcpy(dentry, &(bootblock->dentries[index]), DENTRY_SIZE);
  return 0; // success!
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
  int32_t bytes_read = 0;
  bootblock = (bootblock_t*) file_system_loc;
  int32_t num_inodes = bootblock->num_inodes;

  // the starting index, (offset / BLOCK_SIZE, since offset is in bytes)
  int dblock_index = offset / BLOCK_SIZE;

  // Check to make sure we have valid parameters
  if (inode >= num_inodes || buf == NULL) {
    return -1;
  }

  // find the file with inode index "inode"
  // remember to +1 to the inode index because inodes are zero indexed
  inode_t* src_file = (inode_t*) (bootblock + (inode + 1));

  // get the start dblock location
  int32_t dblock_ptr = src_file->dblocks[dblock_index];

  // get the pointer to the starting data block
  void* curr_dblock = (void*) (bootblock + (num_inodes + 1 + dblock_ptr));

  // start the pointer at a location in the data block based on the offset
  curr_dblock += (offset % BLOCK_SIZE);

  // keep track of how far into the file we are, in bytes
  uint32_t file_remaining = length;
  if (src_file->length - offset < length) {
    file_remaining = src_file->length - offset;
  }

  // first deal with the very first block which has the offset
  // if file_remaining is more than block - offset, copy the remaining block
    if (file_remaining > BLOCK_SIZE - (offset % BLOCK_SIZE)) {
      memcpy(buf, curr_dblock + offset, BLOCK_SIZE - (offset % BLOCK_SIZE));
      file_remaining -= BLOCK_SIZE - (offset % BLOCK_SIZE);
      bytes_read += BLOCK_SIZE - (offset % BLOCK_SIZE);
    } else {
      // otherwise, copy the remaining file
      memcpy(buf, curr_dblock, file_remaining);
      file_remaining -= file_remaining;
      bytes_read += file_remaining;
    }

  // now we can loop until we have no more of the file remaining
  while (file_remaining > 0) {

    // if file_remaining is greater than this block, copy the whole block
    if (file_remaining > BLOCK_SIZE) {
      memcpy(buf, curr_dblock, BLOCK_SIZE);
      file_remaining -= BLOCK_SIZE;
      bytes_read += BLOCK_SIZE;
    } else {
      // otherwise, copy the remaining file
      memcpy(buf, curr_dblock, file_remaining);
      file_remaining -= file_remaining;
      bytes_read += file_remaining;
    }

    // check if we're out of range of the curr_dblock
    if ((bytes_read + offset) % (BLOCK_SIZE) == 0) {
      // go to next dblock
      dblock_index++;
      if (dblock_index < DBLOCKS_PER_INODE)
        dblock_ptr = src_file->dblocks[dblock_index];
      curr_dblock = (void*) (bootblock + (num_inodes + 1 + dblock_ptr));
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
  dentry_t dentry;
  if(read_dentry_by_name((uint8_t *)fname, &dentry) == -1)
  {
    return -1;
  }
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
  dentry_t dentry;
  
  if(read_dentry_by_name((uint8_t *)fd, &dentry) == -1)
  {
    return -1;
  }
  /* If the file exists, copy the data into the buffer and returns bytes read of file*/
  //read_dentry_by_name((uint8_t *)fd, &dentry);
  return read_data(dentry.inode_num, 0, buf, nbytes);
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

/**
 * dir_open()
 * 
 * DESCRIPTION:  Opens a directory
 * INPUTS: fname - the filename to open
 * OUTPUTS: 0 if successful, -1 otherwise
 */
int32_t dir_open (const uint8_t* fname){
  return 0;
}

/**
 * dir_read()
 * 
 * DESCRIPTION:  Outputs name of directory to buffer
 * INPUTS: fd -- file descriptor
 *         buf -- starting pointer of copy data buffer
 *         nbytes -- number of bytes to read
 * OUTPUTS: total bytes read if successful, -1 otherwise
 */  
int32_t dir_read (int32_t fd, void* buf, int32_t nbytes){
  /* Initializes Local Variables */
  dentry_t dentry;
  int i;
  curr_directory=0;

  if(read_dentry_by_index(curr_directory, &dentry) == -1){
    return -1;
  }
  if (read_dentry_by_index(curr_directory, &dentry) == 0){
    for (i = 0; i <= 32; i++){
      ((int8_t*)(buf))[i] = '\0';
    }
    int32_t length = strlen((int8_t*)dentry.file_name);
    strncpy((int8_t*)buf, (int8_t*)dentry.file_name, length);
    curr_directory++;
    return length;
  }
  return -1;
}

/**
 * dir_write()
 * 
 * DESCRIPTION:  Writes to a directory
 * INPUTS: fd -- file descriptor
 *         buf -- starting pointer of copy data buffer
 *         nbytes -- number of bytes to read
 * OUTPUTS: Always -1 because this is a read only file system
 */  
int32_t dir_write (int32_t fd, void* buf, int32_t nbytes){
  return -1;
}

/**
 * dir_close()
 * 
 * DESCRIPTION:  Close directory that is open
 * INPUTS: fd -- file descriptor for directory to close
 * OUTPUTS: 0 if successful, -1 otherwise
 */  
int32_t dir_close (int32_t fd){
  return 0;
}



