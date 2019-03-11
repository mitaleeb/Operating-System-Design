.text

.global keyboard_linkage, rtc_linkage, systemcall_linkage

keyboard_linkage:
  #cld
	pushf              /* save registers on stack */
	pushl %gs
	pushl %fs
	pushl %es
	pushl %ds
	pushl %esi         /* save callee saved regs too, since callee func does not */
	pushl %edi
	pushl %ebp
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax

	call handle_keyboard_interrupt

	popl %eax          /* restore registers from stack */
	popl %ebx
	popl %ecx
	popl %edx
	popl %ebp
	popl %edi
	popl %esi
	popl %ds
	popl %es
	popl %fs
	popl %gs
	popf

	iret

rtc_linkage:
	#cld                /* save registers on stack */
	pushf
	pushl %gs
	pushl %fs
	pushl %es
	pushl %ds
	pushl %esi         /* save callee saved regs too, since callee func does not */
	pushl %edi
	pushl %ebp
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax

	call handle_rtc_interrupt

	popl %eax          /* restore registers from stack */
	popl %ebx
	popl %ecx
	popl %edx
	popl %ebp
	popl %edi
	popl %esi
	popl %ds
	popl %es
	popl %fs
	popl %gs
	popf

	iret

  systemcall_linkage:
  	#cld                /* save registers on stack */
  	pushf
  	pushl %gs
  	pushl %fs
  	pushl %es
  	pushl %ds
  	pushl %esi         /* save callee saved regs too, since callee func does not */
  	pushl %edi
  	pushl %ebp
  	pushl %edx
  	pushl %ecx
  	pushl %ebx
  	pushl %eax

  	call handle_systemcall_interrupt

  	popl %eax          /* restore registers from stack */
  	popl %ebx
  	popl %ecx
  	popl %edx
  	popl %ebp
  	popl %edi
  	popl %esi
  	popl %ds
  	popl %es
  	popl %fs
  	popl %gs
  	popf

  	iret
