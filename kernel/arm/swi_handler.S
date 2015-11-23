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

  STMFD sp, {sp, lr}^       @Store User registers as these might change on witch
  ADD   sp, sp, #-8

  ADD   sp, sp, #-4          @ For spsr
	STMFD	sp!, {r0-r12, lr}		@Store users GP registers and lr_svc in kernel stack

	MRS	  r1, spsr			      @Store svc sp at top of stack
	STR	  r1, [sp, #56]

	MOV 	r1, sp				      @r1 now contains pointer to parameters on stack
	LDR 	r0, [lr, #-4]			  @Extract SWI number
	BIC	  r0, r0, #0xFF000000

	LDR	  r2, =uboot_r8			  @restore uboot address
	LDR	  r8, [r2]

	MRS	  r3, cpsr			      @enable IRQs
	BIC	  r3, #0x80
	MSR	  cpsr, r3

	BL	  c_swi_handler			  @Call C_SWI_Handler and pass the SWI number

	LDR	  r1, [sp, #56]			  @restore svc spsr
	MSR	  spsr, r1

	ADD 	sp, sp, #4			    @add 4 bytes because we do not restore r0
	LDMFD	sp!, {r1-r12, lr}		@restore users GP registers and lr_svc

  ADD   sp, sp, #4          @ For spsr
  LDMFD sp, {sp, lr}^       @Restore User lr and sp
  ADD   sp, sp, #8

	MOVS 	pc, lr				      @return to user program

