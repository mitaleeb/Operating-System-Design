.text

.global keyboard_linkage, rtc_linkage, pit_linkage

keyboard_linkage:
    cld
	pushf							/* save registers on stack */
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	pushl %eax
	pushl %esi
	pushl %edi
	pushl %ebp
	pushl %edx 		
	pushl %ecx
	pushl %ebx
	
	call handle_keyboard_interrupt			
	
	popl %ebx		 				/* restore registers from stack */
	popl %ecx
	popl %edx
	popl %ebp
	popl %edi
	popl %esi
	popl %eax
	popl %gs
	popl %fs
	popl %es
	popl %ds
	popf
	
	iret

rtc_linkage:
	cld 							/* save registers on stack */
	pushf			
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	pushl %eax
	pushl %esi
	pushl %edi
	pushl %ebp
	pushl %edx 		
	pushl %ecx
	pushl %ebx
	
	call handle_rtc_interrupt
	
	popl %ebx						/* restore registers from stack */
	popl %ecx		 
	popl %edx
	popl %ebp
	popl %edi
	popl %esi
	popl %eax
	popl %gs
	popl %fs
	popl %es
	popl %ds
	popf
	
	iret
