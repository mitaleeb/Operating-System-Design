.globl syscall_linker

RETVAL:
  .long 0             # the return value of the syscall

syscall_linker:
  pushal
  pushfl

  cmpl $10, %eax      # support system calls 1 to 10
  jg syscall_invalid
  cmpl $1, %eax
  jl syscall_invalid

  pushl %edx
  pushl %ecx
  pushl %ebx
  pushl %eax          # push arguments to stack and call
  call syscall_call
  addl $16, %esp      # pop the 4 arguments off the stack
  jmp syscall_end

syscall_invalid:
  movl $-1, %eax      # return value of -1
  
syscall_end:
  movl %eax, RETVAL
  popfl
  popal
  movl RETVAL, %eax   # store the retrun value into eax
  iret