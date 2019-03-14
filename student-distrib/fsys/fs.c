/**
 * fs.c
 * 
 * This file holds the functions to read/parse the read-only filesystem.
 */

#include "fs.h"

/* @TODO: get the bootblock pointer in some way. Right now I'm just assuming
it's a pointer because that makes sense to me... */
bootblock_t* bootblock;

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
  
  // Check null pointers
  if (fname == NULL || dentry == NULL) {
    return -1;
  }

  // loop through all the dentries until we find one with a name that matches
  int32_t num_dentries = bootblock->num_dentries;

  // sanity check making sure num_dentries is less than max dentries
  if (num_dentries > MAX_DENTRIES) 
    return -1;
  
  int i;
  for (i = 0; i < num_dentries; i++) {
    if (bootblock->dentries[i].file_name == fname) { // @TODO: this is a check for equality, which is not correct for strings
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

  return bytes_read;
}

