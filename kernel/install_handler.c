/** @file install_handler.c
 *
 *  @brief This file contains the functionality of irq and swi
 *  handler installing.
 *
 *  @author Alejandro Jove (ajovedel)
 *  @author Vishnu Gorantla (vishnupg)
 *
 *  @bug No known bugs
 */


#include <exports.h>
#include "globals.h"

#define LDR_POS_OPCODE    0xe59ff000u
#define LDR_NEG_OPCODE    0xe51ff000u
#define LDR_OPCODE_MASK   0xfffff000u
#define DEFAULT_PREFETCH  0x8
#define NEXT_INSTR_OFFSET 0x4

/** @brief SWI and IRQ handler installer
 *
 *  This function nstalls the handler using the specified vector 
 *  address The replaced instructions are returned so the caller can
 *  restore the instructions
 *
 *  @param vector_addr The offset at which handler instruction is present
 *  @param saved_inst Pointer to store the first two instructions of 
 *                    original handler.
 *  @param install_handler The handler address to be placed at.
 *  @return Address of the original handler
 */
unsigned *install_handler(unsigned *vector_addr, unsigned *saved_inst, 
                              unsigned install_handler){

	unsigned *handler_addr;

  unsigned swi_opcode = (*vector_addr) & LDR_OPCODE_MASK;

  // Check the swi opcode, could be offset or negative offset
  if(swi_opcode != LDR_POS_OPCODE && swi_opcode != LDR_NEG_OPCODE)
  {
    printf("Unrecognized instruction\n");
    printf("Opcode is 0x%x\n", swi_opcode);
    return (unsigned *)0;
  }

  // Find the offset to add to the swi instr address. Change sign accordingly
  int ld_offset = (*vector_addr) & ~(LDR_OPCODE_MASK);
  if(swi_opcode == LDR_NEG_OPCODE)
    ld_offset = -ld_offset;

  handler_addr = *(unsigned **)((unsigned)vector_addr + DEFAULT_PREFETCH +
                             ld_offset);


	// Save the first two instructions that we are going to be modyfing
	// in the SWI handler	
	saved_inst[0] = *handler_addr;
	saved_inst[1] = *(handler_addr + 1);

	// install "ldr pc, [pc, #-4]
  *handler_addr = LDR_NEG_OPCODE | NEXT_INSTR_OFFSET;

  //install the handler
  *(handler_addr + 1) = install_handler;

	return handler_addr;
}

