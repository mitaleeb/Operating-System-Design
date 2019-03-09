/**
 * idt.c
 * 
 * A file that holds functions that help populate the idt.
 */

#include "idt.h"
#include "devices.h"
#include "linkage.h"

void populate_idt(idt_desc_t* idt) {

	/* load IDT */
	lidt(idt_desc_ptr);
	
    int i;								
	for(i=0; i<NUM_VEC; i++) {
		
		idt[i].present = 0x01;      /* set present bit to 1 */
		idt[i].size = 0x01;
		idt[i].seg_selector = KERNEL_CS;
		
		if (i == 0x80) {
		    /* system call descriptor should have its
		     * descriptor privilge level set to 3 */
			idt[i].dpl = 0x03;
		} else {
		    /* set descriptor privilege level to kernel */
		    idt[i].dpl = 0x00;
		}

		/* set reserved bits depending on vector number */				
		idt[i].reserved0 = 0x00;
		idt[i].reserved1 = 0x01;
		idt[i].reserved2 = 0x01;
		if (i >= 0x20) {
		    idt[i].reserved3 = 0x00;
		} else {
		    idt[i].reserved3 = 0x01;
		}
		idt[i].reserved4 = 0x00;

		
		/* if vector > 32 interrupt, general exception */
		if(i >= 32) {
			SET_IDT_ENTRY(idt[i], general_exception);
		}
	}

	/* set IDT entries with corresponding handler */
	SET_IDT_ENTRY(idt[0x00], idt_de);
	SET_IDT_ENTRY(idt[0x01], idt_db);
	SET_IDT_ENTRY(idt[0x02], idt_nmi);
	SET_IDT_ENTRY(idt[0x03], idt_bp);
	SET_IDT_ENTRY(idt[0x04], idt_of);
	SET_IDT_ENTRY(idt[0x05], /* stuff */);
	SET_IDT_ENTRY(idt[0x06], /* stuff */);
	SET_IDT_ENTRY(idt[0x07], /* stuff */);
	SET_IDT_ENTRY(idt[0x08], /* stuff */);
	SET_IDT_ENTRY(idt[0x09], /* stuff */);
	SET_IDT_ENTRY(idt[0x0A], /* stuff */);
	SET_IDT_ENTRY(idt[0x0B], /* stuff */);
	SET_IDT_ENTRY(idt[0x0C], /* stuff */);
	SET_IDT_ENTRY(idt[0x0D], /* stuff */);
	SET_IDT_ENTRY(idt[0x0E], /* stuff */);
	SET_IDT_ENTRY(idt[0x0F], /* stuff */);
	SET_IDT_ENTRY(idt[0x10], /* stuff */);
	SET_IDT_ENTRY(idt[0x11], /* stuff */);
	SET_IDT_ENTRY(idt[0x12], /* stuff */);
	SET_IDT_ENTRY(idt[0x13], /* stuff */);

	
	/* vector for keyboard interrupt */
	SET_IDT_ENTRY(idt[0x21], keyboard_linkage);
	SET_IDT_ENTRY(idt[0x28], rtc_linkage);
	// i think theres supposed to be one for system call too?
}


extern void idt_de() {
    
}

extern void idt_db() {
    
}

extern void idt_nmi() {
    
}

extern void idt_bp() {
    
}

extern void idt_of() {
    
}

extern void idt_br() {
    
}

extern void idt_ud() {
    
}

extern void idt_kk() {
    
}