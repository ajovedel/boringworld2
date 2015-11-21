/** @file c_swi_handler
 *
 *  @brief This file contains the functionality of the C SWI handler
 *
 *  @author Alejandro Jove (ajovedel)
 *  @author VishnuGorantla (vishnupg)
 *
 *  @bug No known bugs
 */
#include <exports.h>
#include "syscalls.h"
#include <bits/swi.h>

/*  @brief The C SWI handler
 *  call the appropriate syscall, pass the right arguments and return the value.
 *  exit is special and will not return in this function if called
 *
 *
 *  @param SWI_number The swi number of the syscall
 *  @param regs Pointer to the list of all the user mode registers 
 *  return The return status of the system call.
 */
unsigned c_swi_handler(unsigned SWI_number, unsigned *regs){

	unsigned return_code = 0;

	switch(SWI_number){

		// call read syscall
		case READ_SWI:
			return_code = read((int)regs[0], (void *)regs[1], (size_t)regs[2]);
			break;

		// call write syscall
		case WRITE_SWI:
			return_code = write((int)regs[0], (void *)regs[1], (size_t)regs[2]);
			break;
		
		// call time syscall
		case TIME_SWI:
			return_code = time();
			break;

		// call sleep syscall
		case SLEEP_SWI:
			sleep((unsigned long)regs[0]);
			break;

		// unknown SWI code
		default:
			printf("Unknown SWI number\n");
			break;
		}

	return return_code;
}
