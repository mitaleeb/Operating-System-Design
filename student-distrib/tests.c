#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
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
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
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
int idt_test2(){
	TEST_HEADER;

	int result = PASS;
	int handler_15_0;
	int handler_16_31;
	int i;
	/* check to ensure that every index of idt has been filled with appropriate handler address*/
	handler_15_0 = (uint32_t)&idt_de & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_de) & 0xFFFF0000) >> 16;
	if(idt[0].offset_15_00 != handler_15_0 &&
		 idt[0].offset_31_16 != handler_16_31) {
		printf("Entry idt[0] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_db & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_db) & 0xFFFF0000) >> 16;
	if(idt[1].offset_15_00 != handler_15_0 &&
		 idt[1].offset_31_16 != handler_16_31) {
		printf("Entry idt[1] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_nmi & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_nmi) & 0xFFFF0000) >> 16;
	if(idt[2].offset_15_00 != handler_15_0 &&
		 idt[2].offset_31_16 != handler_16_31) {
		printf("Entry idt[2] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_bp & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_bp) & 0xFFFF0000) >> 16;
	if(idt[3].offset_15_00 != handler_15_0 &&
		 idt[3].offset_31_16 != handler_16_31) {
		printf("Entry idt[3] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_of & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_of) & 0xFFFF0000) >> 16;
	if(idt[4].offset_15_00 != handler_15_0 &&
		 idt[4].offset_31_16 != handler_16_31) {
		printf("Entry idt[4] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_br & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_br) & 0xFFFF0000) >> 16;
	if(idt[5].offset_15_00 != handler_15_0 &&
		 idt[5].offset_31_16 != handler_16_31) {
		printf("Entry idt[5] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_ud & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_ud) & 0xFFFF0000) >> 16;
	if(idt[6].offset_15_00 != handler_15_0 &&
		 idt[6].offset_31_16 != handler_16_31) {
		printf("Entry idt[6] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_nm & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_nm) & 0xFFFF0000) >> 16;
	if(idt[7].offset_15_00 != handler_15_0 &&
		 idt[7].offset_31_16 != handler_16_31) {
		printf("Entry idt[7] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_df & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_df) & 0xFFFF0000) >> 16;
	if(idt[8].offset_15_00 != handler_15_0 &&
		 idt[8].offset_31_16 != handler_16_31) {
		printf("Entry idt[8] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_general_exception & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_general_exception) & 0xFFFF0000) >> 16;
	if(idt[9].offset_15_00 != handler_15_0 &&
		 idt[9].offset_31_16 != handler_16_31) {
		printf("Entry idt[9] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_ts & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_ts) & 0xFFFF0000) >> 16;
	if(idt[10].offset_15_00 != handler_15_0 &&
		 idt[10].offset_31_16 != handler_16_31) {
		printf("Entry idt[10] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_np & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_np) & 0xFFFF0000) >> 16;
	if(idt[11].offset_15_00 != handler_15_0 &&
		 idt[11].offset_31_16 != handler_16_31) {
		printf("Entry idt[11] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_ss & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_ss) & 0xFFFF0000) >> 16;
	if(idt[12].offset_15_00 != handler_15_0 &&
		 idt[12].offset_31_16 != handler_16_31) {
		printf("Entry idt[12] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_gp & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_gp) & 0xFFFF0000) >> 16;
	if(idt[13].offset_15_00 != handler_15_0 &&
		 idt[13].offset_31_16 != handler_16_31) {
		printf("Entry idt[13] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_pf & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_pf) & 0xFFFF0000) >> 16;
	if(idt[14].offset_15_00 != handler_15_0 &&
		 idt[14].offset_31_16 != handler_16_31) {
		printf("Entry idt[14] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_general_exception & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_general_exception) & 0xFFFF0000) >> 16;
	if(idt[15].offset_15_00 != handler_15_0 &&
		 idt[15].offset_31_16 != handler_16_31) {
		printf("Entry idt[15] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_mf & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_mf) & 0xFFFF0000) >> 16;
	if(idt[16].offset_15_00 != handler_15_0 &&
		 idt[16].offset_31_16 != handler_16_31) {
		printf("Entry idt[16] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_ac & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_ac) & 0xFFFF0000) >> 16;
	if(idt[17].offset_15_00 != handler_15_0 &&
		 idt[17].offset_31_16 != handler_16_31) {
		printf("Entry idt[17] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_mc & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_mc) & 0xFFFF0000) >> 16;
	if(idt[18].offset_15_00 != handler_15_0 &&
		 idt[18].offset_31_16 != handler_16_31) {
		printf("Entry idt[18] is incorrect!");
		assertion_failure();
		return FAIL;
	}

	handler_15_0 = (uint32_t)&idt_xf & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_xf) & 0xFFFF0000) >> 16;
	if(idt[19].offset_15_00 != handler_15_0 &&
		 idt[19].offset_31_16 != handler_16_31) {
		printf("Entry idt[19] is incorrect!");
		assertion_failure();
		return FAIL;
	}

for(i = 20; i < 32; i++)
{
	handler_15_0 = (uint32_t)&idt_general_exception & 0xFFFF;
	handler_16_31 = ((uint32_t)(&idt_general_exception) & 0xFFFF0000) >> 16;
	if(idt[i].offset_15_00 != handler_15_0 &&
		 idt[i].offset_31_16 != handler_16_31) {
		printf("Entry idt[%d] is incorrect!", i);
		assertion_failure();
		return FAIL;
	}
}
/* if all cases passed, return success */
return result;
}


/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	TEST_OUTPUT("idt_test2", idt_test2());
}
