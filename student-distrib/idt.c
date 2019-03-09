/**
 * idt.c
 * 
 * A file that holds functions that help populate the idt.
 */

#include "idt.h"

void populate_idt(idt_desc_t* idt) {

}

void handle_keyboard_interrupt() {
    char c = 0x00;
    
    while (c != 0x00) {
        c = inb(0x60);
    }
}
