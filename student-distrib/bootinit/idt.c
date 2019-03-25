/**
 * idt.c
 *
 * A file that holds functions that help populate the idt.
 */

#include "idt.h"
#include "../devices.h"
#include "../rtc.h"
#include "../lib.h"
#include "../linkage.h"

void populate_idt() {
  /* load IDT */
  lidt(idt_desc_ptr);

  int i;
  for (i = 0; i < NUM_VEC; i++) {
    idt[i].present = 0x01; /* set present bit to 1 */
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
    if (i >= 32) {
      SET_IDT_ENTRY(idt[i], &idt_general_exception);
    }
  }

  /* set IDT entries with corresponding handler */
  SET_IDT_ENTRY(idt[0x00], &idt_de);
  SET_IDT_ENTRY(idt[0x01], &idt_db);
  SET_IDT_ENTRY(idt[0x02], &idt_nmi);
  SET_IDT_ENTRY(idt[0x03], &idt_bp);
  SET_IDT_ENTRY(idt[0x04], &idt_of);
  SET_IDT_ENTRY(idt[0x05], &idt_br);
  SET_IDT_ENTRY(idt[0x06], &idt_ud);
  SET_IDT_ENTRY(idt[0x07], &idt_nm);
  SET_IDT_ENTRY(idt[0x08], &idt_df);
  SET_IDT_ENTRY(idt[0x09], &idt_general_exception);
  SET_IDT_ENTRY(idt[0x0A], &idt_ts);
  SET_IDT_ENTRY(idt[0x0B], &idt_np);
  SET_IDT_ENTRY(idt[0x0C], &idt_ss);
  SET_IDT_ENTRY(idt[0x0D], &idt_gp);
  SET_IDT_ENTRY(idt[0x0E], &idt_pf);
  SET_IDT_ENTRY(idt[0x0F], &idt_general_exception);
  SET_IDT_ENTRY(idt[0x10], &idt_mf);
  SET_IDT_ENTRY(idt[0x11], &idt_ac);
  SET_IDT_ENTRY(idt[0x12], &idt_mc);
  SET_IDT_ENTRY(idt[0x13], &idt_xf);

  for (i = 20; i < 32; i++) {
    SET_IDT_ENTRY(idt[i], &idt_general_exception);
  }

  /* vector for keyboard interrupt */
  SET_IDT_ENTRY(idt[0x21], keyboard_linkage);
  /* vector for rtc interrupt */
  SET_IDT_ENTRY(idt[0x28], rtc_linkage);
  /* vector for system call interrupt */
  SET_IDT_ENTRY(idt[0x80], systemcall_linkage);
}

/* idt_de() - Divide Error. Vector 0x00. */
extern void idt_de() {
  printf("Divide by zero error, (Vec 0x00) \n");
  while (1)
    ;
}

/* idt_db() - RESERVED. Vector 0x01. */
extern void idt_db() {
  printf("RESERVED, (Vec 0x01) \n");
  while (1)
    ;
}

/* idt_nmi() - Nonmaskable External Interrupt. Vector 0x02. */
extern void idt_nmi() {
  printf("Nonmaskable External Interrupt, (Vec 0x02) \n");
  while (1)
    ;
}

/* idt_bp() - Breakpoint. Vector 0x03. */
extern void idt_bp() {
  printf("Breakpoint, (Vec 0x03) \n");
  while (1)
    ;
}

/* idt_of() - Overflow. Vector 0x04. */
extern void idt_of() {
  printf("Overflow, (Vec 0x04) \n");
  while (1)
    ;
}

/* idt_br() - BOUND range exceeded. Vector 0x05. */
extern void idt_br() {
  printf("bound range exceeded, (Vec 0x05) \n");
  while (1)
    ;
}

/* idt_ud() - Undefined Opcode. Vector 0x06. */
extern void idt_ud() {
  printf("undefined opcode, (Vec 0x06) \n");
  while (1)
    ;
}

/* idt_nm() - Device Not Available. Vector 0x07. */
extern void idt_nm() {
  printf("Device not available, (Vec 0x07) \n");
  while (1)
    ;
}

/* idt_df() - Double Fault. Vector 0x08. */
extern void idt_df() {
  printf("Double Fault, (Vec 0x08) \n");
  while (1)
    ;
}

/* idt_ts() - Invalid TSS. Vector 10 (0x0A). */
extern void idt_ts() {
  printf("Invalid TSS, (Vec 0x0A) \n");
  while (1)
    ;
}

/* idt_np() - Segment Not Present. Vector 11 (0x0B). */
extern void idt_np() {
  printf("Segment Not Present, (Vec 0x0B) \n");
  while (1)
    ;
}

/* idt_ss() - Stack Segment Fault. Vector 12 (0x0C). */
extern void idt_ss() {
  printf("Stack Segment Fault (Vec 0x0C) \n");
  while (1)
    ;
}

/* idt_gp() - General Protection. Vector 13 (0x0D). */
extern void idt_gp() {
  printf("General Protection, (Vec 0x0D) \n");
  while (1)
    ;
}

/* idt_pf() - Page Fault. Vector 14 (0x0E). */
extern void idt_pf() {
  printf("Page Fault, (Vec 0x0E) \n");
  while (1)
    ;
}

/* idt_mf() - Math Fault. Vector 16 (0x10). */
extern void idt_mf() {
  printf("Math fault, (Vec 0x10) \n");
  while (1)
    ;
}

/* idt_ac() - Alignment Check. Vector 17 (0x11). */
extern void idt_ac() {
  printf("Alignment Check, (Vec 0x11) \n");
  while (1)
    ;
}

/* idt_mc() - Machine Check. Vector 18 (0x12). */
extern void idt_mc() {
  printf("Machine Check, (Vec 0x12) \n");
  while (1)
    ;
}

/* idt_xf() - SIMD float exception. Vector 19 (0x13). */
extern void idt_xf() {
  printf("SIMD float exception, (Vec 0x13) \n");
  while (1)
    ;
}

extern void idt_general_exception() {
  printf("General Exception \n");
  while (1)
    ;
}
