/** @file c_irq_handler.c
 *
 *  @brief This file contains the functionality of the C IRQ handler
 *
 *  @author Alejandro Jove (ajovedel)
 *  @author Vishnu Gorantla (vishnupg)
 *
 *  @bug No known bugs 
 */

#include <exports.h>
#include "syscalls.h"
#include <arm/timer.h>
#include <arm/reg.h>
#include <arm/psr.h>
#include "globals.h"

/** @brief The C IRQ handler
 *
 *  @return None
 */
void c_irq_handler(){

	unsigned long drift;

	// check if timer interrupt has ocurred
	if(reg_read(OSTMR_OSSR_ADDR) && OSTMR_OSSR_M0){

		// calculate drift
		drift = reg_read(OSTMR_OSCR_ADDR) - reg_read(OSTMR_OSMR_ADDR(0));
	
		// the OSMR0 registers has to be reloaded with OSCR + 32500 (OSCR + 10ms)
		reg_write(OSTMR_OSMR_ADDR(0), 
                (reg_read(OSTMR_OSCR_ADDR) + TIME_RES_CYCLES - drift));

		// the OSSR[0] register has to be reset (acknowledged)
		reg_set(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);

		TIME_UNITS_ELAPSED += 1;
	}
	
	// if another type of interrupt has ocurred, we ignore it
	else
		printf("Unknown IRQ triggered\n");
}
