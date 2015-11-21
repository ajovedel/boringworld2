@ file swi_handler.S
@ brief This file contains the assembler stub for calling the C SWI handler
@
@ author Alejandro Jove (ajovedel)
@ author Vishnu Gorantla (vishnupg)


	.file "swi_handler.S"
	.text
	.extern c_swi_handler

#include <asm.h>

FUNC(swi_handler)

	STMFD	sp!, {r0-r12, lr}		@store users GP registers and lr_svc in kernel stack
	MOV 	r1, sp				@r1 now contains pointer to parameters on stack
	LDR 	r0, [lr, #-4]			@Extract SWI number
	BIC	r0, r0, #0xFF000000

	LDR	r5, =uboot_r8			@restore uboot address
	LDR	r8, [r5]

	MRS	r3, cpsr			@enable IRQs
	BIC	r3, #0x80
	MSR	cpsr, r3

	BL	c_swi_handler			@Call C_SWI_Handler and pass the SWI number

	ADD 	sp, sp, #4			@add 4 bytes because we do not restore r0
	LDMFD	sp!, {r1-r12, lr}		@restore users GP registers and lr_svc
	MOVS 	pc, lr				@return to user program





