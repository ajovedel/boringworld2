/** @file interrupt_setup.c
 *
 *  @brief This file contains the functionality regarding setting up
 *  the device registers for interrupts
 *
 *  @author Alejandrow Jove (ajovedel)
 *  @author Vishnu Gorantla (vishnupg)
 *  @bug No known bugs
 */

#include <exports.h>
#include <arm/interrupt.h>
#include <arm/reg.h>

void interrupt_setup(){

	// set ICLR[M0] to generate IRQs, instead of FIQs
	reg_clear(INT_ICLR_ADDR, (1 << INT_OSTMR_0));

	// set ICMR to NOT mask interrupts for M0
	reg_set(INT_ICMR_ADDR, (1 << INT_OSTMR_0));
}
