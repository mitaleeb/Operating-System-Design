/**
 * idt.h
 *
 * h file that holds a lot of definitions of functions, including all the
 * functions in idt.c as well as declarations for all the interrupt handlers in
 * the first 32 IDT locations.
 */

#include "x86_desc.h"

/* Declare functions that we use to set up to the idt */
extern void populate_idt();


/* The following block of code declares functions for all the handlers of the
reserved vectors. We can use an "&" to get the address of this when we use them
to set IDT value */

/* idt_de() - Divide Error. Vector 0x00. */
extern void idt_de();

/* idt_db() - RESERVED. Vector 0x01. */
extern void idt_db();

/* idt_nmi() - Nonmaskable External Interrupt. Vector 0x02. */
extern void idt_nmi();

/* idt_bp() - Breakpoint. Vector 0x03. */
extern void idt_bp();

/* idt_of() - Overflow. Vector 0x04. */
extern void idt_of();

/* idt_br() - BOUND range exceeded. Vector 0x05. */
extern void idt_br();

/* idt_ud() - Undefined Opcode. Vector 0x06. */
extern void idt_ud();

/* idt_nm() - Device Not Available. Vector 0x07. */
extern void idt_nm();

/* idt_df() - Double Fault. Vector 0x08. */
extern void idt_df();

/* idt_ts() - Invalid TSS. Vector 10 (0x0A). */
extern void idt_ts();

/* idt_np() - Segment Not Present. Vector 11 (0x0B). */
extern void idt_np();

/* idt_ss() - Stack Segment Fault. Vector 12 (0x0C). */
extern void idt_ss();

/* idt_gp() - General Protection. Vector 13 (0x0D). */
extern void idt_gp();

/* idt_pf() - Page Fault. Vector 14 (0x0E). */
extern void idt_pf();

/* idt_mf() - Math Fault. Vector 16 (0x10). */
extern void idt_mf();

/* idt_ac() - Alignment Check. Vector 17 (0x11). */
extern void idt_ac();

/* idt_mc() - Machine Check. Vector 18 (0x12). */
extern void idt_mc();

/* idt_xf() - SIMD float exception. Vector 19 (0x13). */
extern void idt_xf();

/* General Exception */
extern void idt_general_exception();
