/**
 * @file mutex.c
 *
 * @brief Implements mutices.
 *
 * @author Harry Q Bovik < PUT YOUR NAMES HERE
 *
 * 
 * @date  
 */

//#define DEBUG_MUTEX

#include <lock.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <globals.h>
#ifdef DEBUG_MUTEX
#include <exports.h> // temp
#endif

mutex_t gtMutex[OS_NUM_MUTEX];


/**
 * @brief Initialize all system mutexes
 */
void mutex_init()
{
	int i = 0;
	for(; i < OS_NUM_MUTEX; i++){
		gtMutex[i].bAvailable = 1;
		gtMutex[i].pHolding_Tcb = NULL;
		gtMutex[i].bLock = 0;
		gtMutex[i].pSleep_queue = NULL;
	}	
}

/*
 * @brief Acquire next available mutex
 */
int mutex_create(void)
{
	disable_interrupts();

	int i = 0;

	// check if there are available mutexes
	for(; i < OS_NUM_MUTEX; i++)
		if(gtMutex[i].bAvailable == 1)
			break;
	
	// no mutex available
	if(i == OS_NUM_MUTEX)
		return -ENOMEM;

	// we use this mutex!
	gtMutex[i].bAvailable = 0;
	enable_interrupts();
	return i;
}

/*
 * @brief Lock the indicated mutex
 */
int mutex_lock(int mutex  __attribute__((unused)))
{
	disable_interrupts();

	// check mutex id is valid
	if(!(mutex >= 0 && mutex <= OS_NUM_MUTEX)){
		enable_interrupts();
		return -EINVAL;
	}

	mutex_t *cur_mutex = &(gtMutex[mutex]);

	// check if mutex has been created before
	if(cur_mutex->bAvailable == 1){
		enable_interrupts();
		return -EINVAL;
	}

	tcb_t *cur_tcb = get_cur_tcb();

	// check that the current task NOT holding the lock already
	if(cur_tcb == cur_mutex->pHolding_Tcb){
		enable_interrupts();
		return -EDEADLOCK;
	}

	// check if other task has locked the mutex
	if(cur_mutex->bLock == 1){

		// add lock to the end of the mutex sleep queue
		tcb_t *tmp_tcb = cur_mutex->pSleep_queue;

		// add lock to beginning of sleep queue
		if(cur_mutex->pSleep_queue == NULL)
			cur_mutex->pSleep_queue = cur_tcb;
		
		// add lock to next empty space on the sleep queue
		else{
			while(tmp_tcb->sleep_queue != NULL)
				tmp_tcb = tmp_tcb->sleep_queue;
			
			tmp_tcb->sleep_queue = cur_tcb;
		}

		// sleep queue of the current TCB is the tail
		cur_tcb->sleep_queue = NULL;

		// send current task to sleep 
		dispatch_sleep();
	}

	// other tasks do not have this mutex
	// we can acquire this mutex for the current tcb
	cur_mutex->bLock = 1;
	cur_mutex->pHolding_Tcb = cur_tcb;
	cur_tcb->holds_lock = 1;	//do i need to set holds_lock? what for?
	
	enable_interrupts();

	return 0;
}

int mutex_unlock(int mutex  __attribute__((unused)))
{
	return 1; // fix this to return the correct value
}

