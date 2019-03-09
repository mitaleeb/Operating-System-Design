/**
 * idt.c
 * 
 * A file that holds functions that help populate the idt.
 */

#include "idt.h"
#include "devices.h"

void populate_idt(idt_desc_t* idt) {

	/* load IDT */
	lidt(idt_desc_ptr);
	
    int i;								
	for(i=0; i<NUM_VEC; i++) {
		
		idt[i].present = 0x01;      /* set present bit to 1 */
		idt[i].dpl = 0x00;          /* set descriptor privilege level to kernel */
		idt[i].size = 0x01;
		idt[i].seg_selector = KERNEL_CS;

		/* set reserved bits depending on vector number */				
		idt[i].reserved0 = 0x00;
		idt[i].reserved1 = 0x01;
		idt[i].reserved2 = 0x01;
		if (i < 32) {
		    idt[i].reserved3 = 0x01;
		} else {
		    idt[i].reserved3 = 0x00;
		}
		idt[i].reserved4 = 0x00;

		
		/* if vector > 32 interrupt, general exception */
		if(i >= 32) {
			SET_IDT_ENTRY(idt[i], general_exception);
		}
		
		if(SYSCALL_INT == i) {
			idt[i].dpl = 0x3;
		}
		
	}

	/* set IDT entries */

}
