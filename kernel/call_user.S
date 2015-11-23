@ file call_user.S
@ 
@ Save the kernel context, setup user space and call user process
@
@ author Alejandro Jove (ajovedel)
@ author Vishnu Gorantla (vishnupg)
@

	.file "call_user_app"
	.text

#include <asm.h>

FUNC(call_user)
	@r0 = argc
	@r1	= argv

	STMFD	sp!, {r0-r12, lr}		@store kernels GP registers, sp and lr				
	LDR		r5, =uboot_r8		@store uboot address in uboot_r8
	STR 		r8, [r5]

	LDR		r5, =.kernel_ret	@store kernel return address for exit to use
	LDR		r6, =kernel_ret_addr
	STR		r5, [r6]

	MRS		r3, cpsr		@disables FIQs and change to USR mode
	ORR		r3, r3, #0x40
	BIC		r3, r3, #0x8F
	ORR		r3, r3, #0x10

	@-----START OF USER MODE-----
	MSR		cpsr, r3
	MOV		sp, #0xA3000000		@setup the full descending user mode stack
	STMFD		sp!, {r0-r1}		@push U-boots argc & argc on the user stack	
	MOV		pc, #0xA0000000		@jump to a loaded user program


.kernel_ret:
	MRS		r3, cpsr		@enable IRQs and FIQs
	BIC		r3,	r3, #0xC0
	MSR		cpsr, r3

	LDMFD		sp!, {r0-r12, lr}	@restore kernels GP registers, sp and lr
	MOV		pc, lr			@return to kernel
