void init_keyboard() {
    /* keyboard is on IR1 of master PIC */
    unsigned int irq_keyboard = 1;
    
    enable_irq(irq_keyboard);
}

void handle_keyboard_interrupt() {
    char c = 0x00;
    
    while (c != 0x00) {
        c = inb(0x60);
    }
}