# syscall_asm.S - Set up system call linkage

.data                 # section declaration (see blink.S)

syscall_table:
  .long 0             # put a 0 in to make our indexing from 1
  .long system_halt
  .long system_execute
  .long system_read
  .long system_write
  .long system_open
  .long system_close
  .long system_getargs
  .long system_vidmap
  .long system_sethandler
  .long system_sigreturn

RETVAL:
  .long 0             # the return value of the syscall


.text                 # section declaration (see blink.S)

.globl syscall_linker

syscall_linker:
  pushal
  pushfl

  cmpl $10, %eax       # support system calls 1 to 10
  jg syscall_invalid
  cmpl $1, %eax
  jl syscall_invalid

  pushl %edx
  pushl %ecx
  pushl %ebx          # push arguments to stack and call     
  call *syscall_table(,%eax,4)
  addl $12, %esp      # pop the 3 arguments off the stack
  jmp syscall_end

syscall_invalid:
  movl $-1, %eax      # return value of -1
  
syscall_end:
  movl %eax, RETVAL
  popfl
  popal
  movl RETVAL, %eax   # store the return value into eax
  iret
