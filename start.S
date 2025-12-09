.section .text.boot
.global init_bss
.global trap_entry
.global main

init_bss:
	la t0,__bss
	la t1,__bss_end
	
	beq t0,t1, init_stack
	li t2,0

zero_:
	sw t2,0(t0)
	addi t0,t0,4
	blt t0,t1, zero_

init_stack:
	la sp,__stack_top

	la t0,trap_entry
	csrw mtvec, t0

	call main
stop:
	j stop
