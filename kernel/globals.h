/** @file globals.h
 *
 *  @brief This file contatins the global vars required for the kernel.
 *
 *  @author Alejandro Jove (ajovedel)
 *  @author Vishnu Gorantla (vishnupg)
 *  @bug No known bugs
 */

#ifndef __GLOBALS_H_
#define __GLOBALS_H_

#define SWI_VECTOR_ADDR (unsigned *) 0x08
#define IRQ_VECTOR_ADDR (unsigned *) 0x18
#define OS_CLK_SPEED (unsigned long) 3250000
#define TIME_RES_MS (unsigned long) 10
#define TIME_RES_CYCLES (unsigned long) (OS_CLK_SPEED * (0.001 * TIME_RES_MS))
#define BAD_CODE          0xbadc0de

void *swi_handler();
void *irq_handler();
unsigned *install_handler(unsigned *vector_addr, unsigned *saved_inst,
                               unsigned handler_addr);
void call_user(int argc, char *argv[]);
void irq_setup();
void timer_setup();
void interrupt_setup();

extern volatile unsigned long TIME_UNITS_ELAPSED;


#endif
