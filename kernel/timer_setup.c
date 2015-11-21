/** @file timer_setup.c
 *
 *  @brief This file contains the functionality regarding setting
 *  up timer specific registers
 *
 *  @author Alejandro Jove (ajovedel)
 *  @author Vishnu Gorantla (vishnupg)
 *  @bug No known bugs
 */
#include <exports.h>
#include <arm/timer.h>
#include <arm/reg.h>
#include "globals.h"

void timer_setup(){

	// make sure OSSR[M0] is not set
	reg_clear(OSTMR_OSSR_ADDR, OSTMR_OSSR_M0);

	// OSCR is set to 0
	reg_clear(OSTMR_OSCR_ADDR, 0xFFFFFFFF);

	// set the match register for 10ms or 32500 cycles
	reg_write(OSTMR_OSMR_ADDR(0), TIME_RES_CYCLES);

	// set OIER[E0], so if a match occurs, OSSR[M0] is set
	reg_set(OSTMR_OIER_ADDR, OSTMR_OIER_E0);
}
