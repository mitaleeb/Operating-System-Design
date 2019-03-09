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
	SET_IDT_ENTRY(idt[0x05], idt_br);
	SET_IDT_ENTRY(idt[0x06], idt_ud);
	SET_IDT_ENTRY(idt[0x07], idt_nm);
	SET_IDT_ENTRY(idt[0x08], idt_df);
	// SET_IDT_ENTRY(idt[0x09], );
	SET_IDT_ENTRY(idt[0x0A], idt_ts);
	SET_IDT_ENTRY(idt[0x0B], idt_np);
	SET_IDT_ENTRY(idt[0x0C], idt_ss);
	SET_IDT_ENTRY(idt[0x0D], idt_gp);
	SET_IDT_ENTRY(idt[0x0E], idt_pf);
	// SET_IDT_ENTRY(idt[0x0F], );
	SET_IDT_ENTRY(idt[0x10], idt_mf);
	SET_IDT_ENTRY(idt[0x11], idt_ac);
	SET_IDT_ENTRY(idt[0x12], idt_mc);
	SET_IDT_ENTRY(idt[0x13], idt_xf);

	
	/* vector for keyboard interrupt */
	SET_IDT_ENTRY(idt[0x21], keyboard_linkage);
	SET_IDT_ENTRY(idt[0x28], rtc_linkage);
	// i think theres supposed to be one for system call too?
}


/* idt_de() - Divide Error. Vector 0x00. */
extern void idt_de() {
    
}

/* idt_db() - RESERVED. Vector 0x01. */
extern void idt_db() {
    
}

/* idt_nmi() - Nonmaskable External Interrupt. Vector 0x02. */
extern void idt_nmi() {
    
}

/* idt_bp() - Breakpoint. Vector 0x03. */
extern void idt_bp() {
    
}

/* idt_of() - Overflow. Vector 0x04. */
extern void idt_of() {
    
}

/* idt_br() - BOUND range exceeded. Vector 0x05. */
extern void idt_br() {
    
}

/* idt_ud() - Undefined Opcode. Vector 0x06. */
extern void idt_ud() {
    
}

/* idt_nm() - Device Not Available. Vector 0x07. */
extern void idt_nm() {
    
}

/* idt_df() - Double Fault. Vector 0x08. */
extern void idt_df() {
    
}

/* idt_ts() - Invalid TSS. Vector 10 (0x0A). */
extern void idt_ts() {
    
}

/* idt_np() - Segment Not Present. Vector 11 (0x0B). */
extern void idt_np() {
    
}

/* idt_ss() - Stack Segment Fault. Vector 12 (0x0C). */
extern void idt_ss() {
    
}

/* idt_gp() - General Protection. Vector 13 (0x0D). */
extern void idt_gp() {
    
}

/* idt_pf() - Page Fault. Vector 14 (0x0E). */
extern void idt_pf() {
    
}

/* idt_mf() - Math Fault. Vector 16 (0x10). */
extern void idt_mf() {
    
}

/* idt_ac() - Alignment Check. Vector 17 (0x11). */
extern void idt_ac() {
    
}

/* idt_mc() - Machine Check. Vector 18 (0x12). */
extern void idt_mc() {
    
}

/* idt_xf() - SIMD float exception. Vector 19 (0x13). */
extern void idt_xf() {
    
}