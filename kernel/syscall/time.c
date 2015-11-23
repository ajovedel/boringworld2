/** @file time.c
 *
 * @brief Kernel timer based syscall implementations
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 */

#include <types.h>
#include <config.h>
#include <bits/errno.h>
#include <arm/timer.h>
#include <arm/reg.h>
#include <syscall.h>
#include <globals.h>


unsigned long time(void)
{
	return (TIME_UNITS_ELAPSED * TIME_RES_MS +
           ((reg_read(OSTMR_OSCR_ADDR) / OS_CLK_SPEED) % TIME_RES_MS));	
}



/** @brief Waits in a tight loop for atleast the given number of milliseconds.
 *
 * @param millis  The number of milliseconds to sleep.
 *
 * 
 */
void sleep(unsigned long sleep_time  __attribute__((unused)))
{
		unsigned long start_time, end_time;

        // get current OSCR
        start_time = reg_read(OSTMR_OSCR_ADDR);

        // calculate time we sleep
        end_time = start_time + (sleep_time * (TIME_RES_CYCLES * 0.1));

        // sleep
        while(reg_read(OSTMR_OSCR_ADDR) < end_time);

}
