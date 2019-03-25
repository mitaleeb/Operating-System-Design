#include "tests.h"
#include "bootinit/idt.h"
#include "bootinit/paging.h"
#include "lib.h"
#include "x86_desc.h"
#include "i8259.h"
#include "devices.h"
#include "rtc.h"
#include "fsys/fs.h"

#define PASS 1
#define FAIL 0
#define IRQ_RTC 8

/* format these macros as you see fit */
#define TEST_HEADER                                                     \
  printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, \
         __FILE__, __LINE__)
#define TEST_OUTPUT(name, result) \
  printf("\n[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure() {
  /* Use exception #15 for assertions, otherwise
     reserved by Intel */
  asm volatile("int $15");
}

/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test() {
  TEST_HEADER;

  int i;
  int result = PASS;
  for (i = 0; i < 10; ++i) {
    if ((idt[i].offset_15_00 == NULL) && (idt[i].offset_31_16 == NULL)) {
      assertion_failure();
      result = FAIL;
    }
  }

  return result;
}

/* IDT Test 2
 *
 * Asserts that the 32 entries in IDT array match the desired addresses
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT contents
 * Files: x86_desc.h/S, idt.c/h
 */
int idt_test2() {
  TEST_HEADER;

  int result = PASS;
  int handler_15_0;
  int handler_16_31;
  int i;
  /* check to ensure that every index of idt has been filled with appropriate
   * handler address*/
  handler_15_0 = (uint32_t)&idt_de & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_de) & 0xFFFF0000) >> 16;
  if (idt[0].offset_15_00 != handler_15_0 &&
      idt[0].offset_31_16 != handler_16_31) {
    printf("Entry idt[0] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_db & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_db) & 0xFFFF0000) >> 16;
  if (idt[1].offset_15_00 != handler_15_0 &&
      idt[1].offset_31_16 != handler_16_31) {
    printf("Entry idt[1] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_nmi & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_nmi) & 0xFFFF0000) >> 16;
  if (idt[2].offset_15_00 != handler_15_0 &&
      idt[2].offset_31_16 != handler_16_31) {
    printf("Entry idt[2] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_bp & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_bp) & 0xFFFF0000) >> 16;
  if (idt[3].offset_15_00 != handler_15_0 &&
      idt[3].offset_31_16 != handler_16_31) {
    printf("Entry idt[3] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_of & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_of) & 0xFFFF0000) >> 16;
  if (idt[4].offset_15_00 != handler_15_0 &&
      idt[4].offset_31_16 != handler_16_31) {
    printf("Entry idt[4] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_br & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_br) & 0xFFFF0000) >> 16;
  if (idt[5].offset_15_00 != handler_15_0 &&
      idt[5].offset_31_16 != handler_16_31) {
    printf("Entry idt[5] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_ud & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_ud) & 0xFFFF0000) >> 16;
  if (idt[6].offset_15_00 != handler_15_0 &&
      idt[6].offset_31_16 != handler_16_31) {
    printf("Entry idt[6] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_nm & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_nm) & 0xFFFF0000) >> 16;
  if (idt[7].offset_15_00 != handler_15_0 &&
      idt[7].offset_31_16 != handler_16_31) {
    printf("Entry idt[7] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_df & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_df) & 0xFFFF0000) >> 16;
  if (idt[8].offset_15_00 != handler_15_0 &&
      idt[8].offset_31_16 != handler_16_31) {
    printf("Entry idt[8] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_general_exception & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_general_exception) & 0xFFFF0000) >> 16;
  if (idt[9].offset_15_00 != handler_15_0 &&
      idt[9].offset_31_16 != handler_16_31) {
    printf("Entry idt[9] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_ts & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_ts) & 0xFFFF0000) >> 16;
  if (idt[10].offset_15_00 != handler_15_0 &&
      idt[10].offset_31_16 != handler_16_31) {
    printf("Entry idt[10] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_np & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_np) & 0xFFFF0000) >> 16;
  if (idt[11].offset_15_00 != handler_15_0 &&
      idt[11].offset_31_16 != handler_16_31) {
    printf("Entry idt[11] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_ss & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_ss) & 0xFFFF0000) >> 16;
  if (idt[12].offset_15_00 != handler_15_0 &&
      idt[12].offset_31_16 != handler_16_31) {
    printf("Entry idt[12] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_gp & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_gp) & 0xFFFF0000) >> 16;
  if (idt[13].offset_15_00 != handler_15_0 &&
      idt[13].offset_31_16 != handler_16_31) {
    printf("Entry idt[13] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_pf & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_pf) & 0xFFFF0000) >> 16;
  if (idt[14].offset_15_00 != handler_15_0 &&
      idt[14].offset_31_16 != handler_16_31) {
    printf("Entry idt[14] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_general_exception & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_general_exception) & 0xFFFF0000) >> 16;
  if (idt[15].offset_15_00 != handler_15_0 &&
      idt[15].offset_31_16 != handler_16_31) {
    printf("Entry idt[15] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_mf & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_mf) & 0xFFFF0000) >> 16;
  if (idt[16].offset_15_00 != handler_15_0 &&
      idt[16].offset_31_16 != handler_16_31) {
    printf("Entry idt[16] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_ac & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_ac) & 0xFFFF0000) >> 16;
  if (idt[17].offset_15_00 != handler_15_0 &&
      idt[17].offset_31_16 != handler_16_31) {
    printf("Entry idt[17] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_mc & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_mc) & 0xFFFF0000) >> 16;
  if (idt[18].offset_15_00 != handler_15_0 &&
      idt[18].offset_31_16 != handler_16_31) {
    printf("Entry idt[18] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  handler_15_0 = (uint32_t)&idt_xf & 0xFFFF;
  handler_16_31 = ((uint32_t)(&idt_xf) & 0xFFFF0000) >> 16;
  if (idt[19].offset_15_00 != handler_15_0 &&
      idt[19].offset_31_16 != handler_16_31) {
    printf("Entry idt[19] is incorrect!");
    assertion_failure();
    return FAIL;
  }

  for (i = 20; i < 32; i++) {
    handler_15_0 = (uint32_t)&idt_general_exception & 0xFFFF;
    handler_16_31 = ((uint32_t)(&idt_general_exception) & 0xFFFF0000) >> 16;
    if (idt[i].offset_15_00 != handler_15_0 &&
        idt[i].offset_31_16 != handler_16_31) {
      printf("Entry idt[%d] is incorrect!", i);
      assertion_failure();
      return FAIL;
    }
  }

  /* force system call interrupt */
  asm volatile("int $0x80");

  /* if all cases passed, return success */
  return result;
}

/**
 * page_value_test()
 *
 * DESCRIPTION: Tests whether the page directory and table entries are as
 * expected.
 */
int page_value_test() {
  TEST_HEADER;
  int result = PASS;

  // call the paging tester to see if it outputs 0
  if (paging_tester()) {
    // assertion_failure();
    result = FAIL;
  }

  return result;
}

/**
 * page_deref_test()
 *
 * DESCRIPTION: Tests the dereference of certain areas in the page to see if our
 * page table performs correctly.
 */
int page_deref_test() {
  TEST_HEADER;
  int result = PASS;

  // Dereference some value in the kernel
  int t = 0x400001;
  int derefed = *((int*)t);

  printf("Contents of address 0x400001 is: %x                  \n", derefed);

  // This section should generate a page fault. Comment it out to boot the OS.
  // t = 0x000003;
  // derefed = *((int*) t);
  // printf("This should have made a page fault! \n");
  // End section

  return result;
}

/**
 * except_test()
 *
 * DESCRIPTION: purposefully "blue screen"s the os to test critical exceptions.
 */
int except_test() {
  TEST_HEADER;
  int result = PASS;

  // Divide by Zero
  int m = 5;
  int n = 0;
  int g = m / n;
  printf("Should never get here! %d \n", g);

  result = FAIL;
  return result;
}

/* CHECKPOINT 2 TESTS */

/**
 * rtc_read_test()
 *
 * DESCRIPTION: Checks if rtc_read waits for interrupt,
 *              does not get stuck in infinite loop
 */
int rtc_read_test() {
  enable_irq(IRQ_RTC);

  rtc_read();

  disable_irq(IRQ_RTC);

  printf("Finished RTC Read Test                                  \n");
  return PASS;
}

/**
 * rtc_write_test()
 *
 * DESCRIPTION: Checks if rtc_write returns correct value
 *              for valid/invalid frequency
 */
int rtc_write_test() {
  enable_irq(IRQ_RTC);
  int32_t i, j, val;
  int result = PASS;


   for (i=2; i<=1024; i*=2) {
     val = rtc_write(i);
     if(val < 0){
      printf("Invalid RTC frequency = %d \n", i);
      disable_irq(IRQ_RTC);
       result = FAIL;
     } else {
        printf("RTC frequency = %d \n", i);
        for(j = 0; j < 10; j++) {
            rtc_read();
          printf("a");
        }
     }
     printf("\n");
   }

  printf("Finished RTC Write Test \n");
  return result;
}

/**
 * terminal_test()
 *
 * DESCRIPTION: Tests whether terminal_read and terminal_write works as expected
 *
 */
int terminal_test() {
  TEST_HEADER;
  int result = PASS;
  int len = 128;
  int output = 0;
  int output2 = 0;
  uint8_t k[128];

  printf("Enter text for terminal buffer: \n");

  output = terminal_read(k,len);
  output2 = terminal_write(k, output);
  if(output != output2) {
    assertion_failure();
    result = FAIL;
  }

  return result;
}
/**
 * int file_system_file_output()
 *
 * DESCRIPTION: Test to write file contents out to terminal 
 */
int file_system_file_output(){
  TEST_HEADER;
  int i;
  uint8_t test_buf[BLOCK_SIZE];
  for (i = 0; i <= BLOCK_SIZE; i++){
      ((int8_t*)(test_buf))[i] = '\0';
  }
  // SPECIFY WHICH FILE YOU WANT TO OUTPUT
  int8_t* file = "frame0.txt";
  // Check if file to be read exists and if so put it in buffer
  if(file_read((uint32_t) ((uint8_t*) file), test_buf, BLOCK_SIZE) < 0){
    return FAIL;
  }
  // Output Contents of File from test buffer to screen
  puts((int8_t*)test_buf);
  //file_close(file);
  return PASS;
}
/**
 * int file_system_dir_output()
 *
 * DESCRIPTION: Test to list out all the files in a directory
 */

int file_system_dir_output(){
  TEST_HEADER;
  // Initialize local variables
  int i;
  uint8_t test_buf[4096];
  // Clear buffer
  for (i = 0; i <= 4096; i++){
      ((int8_t*)(test_buf))[i] = '\0';
  }
  // Specify that this is current directory but not relevant for checkpoint 2
  int8_t* dir = ".";

  // Open directory
  dir_open(((uint8_t*) dir));

  if (dir_read((uint32_t)((uint8_t*) dir), test_buf, 4096) < 0){
    return FAIL;
  }
  // Loop through Directory Entries and output all file names to screen
  for(i=0; i< MAX_DENTRIES; i++){
    if(dir_read((uint32_t)((uint8_t*) dir), test_buf, 4096) < 0){
      return PASS;
    }
    puts((int8_t*)test_buf);
    putc('\n');
  }
  // Close current directory 
  dir_close((uint32_t) ((uint8_t*) dir));
  return PASS;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests() {
  TEST_OUTPUT("idt_test", idt_test());
  printf("Finished IDT Test 1 \n");

  // launch your tests here
  TEST_OUTPUT("idt_test2", idt_test2());
  printf("Finished IDT Test 2 \n");

  TEST_OUTPUT("page test", page_value_test());
  printf("Finished Page Value Test \n");

  TEST_OUTPUT("page deref test", page_deref_test());
  printf("Finished Page Dereference Test \n");

  TEST_OUTPUT("rtc write test", rtc_read_test());
  printf("Finished RTC Read Test \n");

  TEST_OUTPUT("rtc write test", rtc_write_test());
  printf("Finished RTC Write Test \n");

  TEST_OUTPUT("file system file contents test ", file_system_file_output());
  printf("Finished File System File Output Test                          \n"); 

  TEST_OUTPUT("file system directory test ", file_system_dir_output());
  printf("Finished File System Directory Output Test                          \n");

  TEST_OUTPUT("terminal test", terminal_test());
  printf("Finished Terminal Read and Write Test \n");


  // Test that purposefully puts the system into an unusable state by forcing
  // one of the first 32 exceptions to happen. Comment it out to boot the OS.
  // TEST_OUTPUT("Intentional Bluescreen Test", except_test());
  // printf("We should never get to this line! \n");
}
