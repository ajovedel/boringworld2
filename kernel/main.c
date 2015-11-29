/** @file main.c
 *
 * @brief kernel main
 *
 * @author 
 *	   
 *	   
 * @date   
 */
 
#include <kernel.h>
#include <task.h>
#include <sched.h>
#include <device.h>
#include <assert.h>
#include <globals.h>
#include <lock.h>

uint32_t global_data;

/* uboot jump table addr */
unsigned *uboot_r8;

/* kernel return addr from usr */
unsigned *kernel_ret_addr;

/* keep track of time elapsed of the system */
volatile unsigned long TIME_UNITS_ELAPSED = 0;

int kmain(int argc __attribute__((unused)), char** argv  __attribute__((unused)), uint32_t table)
{
	unsigned *swi_install_addr;
	unsigned *irq_install_addr;
	unsigned swi_saved_inst[2];
	unsigned irq_saved_inst[2];

	app_startup();
	global_data = table;
	/* add your code up to assert statement */

	
	/* install SWI handler */
	swi_install_addr = install_handler(SWI_VECTOR_ADDR, swi_saved_inst, 
			(unsigned) &swi_handler);

	if(!swi_install_addr)
		return BAD_CODE;

	/* install IRQ handler */
	irq_install_addr = install_handler(IRQ_VECTOR_ADDR, irq_saved_inst,
			(unsigned) &irq_handler);

	if(!irq_install_addr)
		return BAD_CODE;

	// setup interrupts
	interrupt_setup();

	// setup timer
    timer_setup();

    // initialize mutexes
    mutex_init();

	// call user program
	call_user(argc, argv);

	// restore SWI handler
	*swi_install_addr = swi_saved_inst[0];
	*(swi_install_addr + 1) = swi_saved_inst[1];

	// restore the IRQ handler
	*irq_install_addr = irq_saved_inst[0];
	*(irq_install_addr + 1) = irq_saved_inst[1];
	
	//assert(0);        /* should never get here */
	return 0;
}
