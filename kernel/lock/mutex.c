/**
 * @file mutex.c
 *
 * @brief Implements mutices.
 *
 * @author Alejandro Jove (ajovedel)
 * @author Vishnu Gorantla (vishnupg)
 *
 * 
 */

//#define DEBUG_MUTEX

#include <lock.h>
#include <task.h>
#include <sched.h>
#include <bits/errno.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <globals.h>
#include <syscall.h>
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
 * @brief Acquire the next available mutex
 *
 * @return The status code
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
	if(i == OS_NUM_MUTEX){
        enable_interrupts();
		return -ENOMEM;
    }

	// we use this mutex!
	gtMutex[i].bAvailable = 0;
	enable_interrupts();
	return i;
}

/*
 * @brief Lock the indicated mutex
 *
 * @param mutex - muted ID of the mutex that is attempting to be locked
 *
 * @return The status code
 */
int mutex_lock(int mutex  __attribute__((unused)))
{

	// check mutex id is valid
	if(!(mutex >= 0 && mutex <= OS_NUM_MUTEX)){
		return -EINVAL;
	}

  disable_interrupts();
	
  mutex_t *cur_mutex = &(gtMutex[mutex]);
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


		// send current task to sleep 
		dispatch_sleep();
	}

  else
  {
	  // other tasks do not have this mutex
	  // we can acquire this mutex for the current tcb
        cur_mutex->bLock = 1;
        cur_tcb->sleep_queue = NULL;
        cur_mutex->pHolding_Tcb = cur_tcb;

        // task holds a new mutex
        cur_tcb->holds_lock++;
        
        // we raise the priority to the highest priority as soon as a tcb
        // acquires a mutex. (HLP) 
        cur_tcb->cur_prio = 0;
  }

	enable_interrupts();

	return 0;
}

/*
 * @brief Unlock the indicated mutex
 *
 * @param mutex - mutex ID of the mutex that is trying to be unlocked
 *
 * @return The status code
 */
int mutex_unlock(int mutex  __attribute__((unused)))
{

	// check mutex id is valid
	if(!(mutex >= 0 && mutex <= OS_NUM_MUTEX)){
		enable_interrupts();
		return -EINVAL;
	}

	disable_interrupts();
	mutex_t *cur_mutex = &(gtMutex[mutex]);
	tcb_t *cur_tcb = get_cur_tcb();

	// check current task is holding the lock
	if(cur_tcb != cur_mutex->pHolding_Tcb){
		enable_interrupts();
		return -EPERM;
	}


	// check if other tasks are waiting to use this mutex
	// if so, change mutex ownership, update sleep queue and add task to run queue
	if(cur_mutex->pSleep_queue != NULL){
		cur_mutex->pHolding_Tcb = cur_mutex->pSleep_queue;
		cur_mutex->pSleep_queue = cur_mutex->pSleep_queue->sleep_queue;
		runqueue_add(cur_mutex->pHolding_Tcb, cur_mutex->pHolding_Tcb->cur_prio);
	}	

	// if no other tasks wants this mutex, free the mutex
	else{
		cur_mutex->pHolding_Tcb = NULL;
		cur_mutex->bLock = 0;
	}

	// remove 1 mutex counter
	cur_tcb->holds_lock--;

    // if the task holds no other mutex, we set the priority
    // back to its native priority (HLP)
    if(cur_tcb->holds_lock == 0)
        cur_tcb->cur_prio = cur_tcb->native_prio;    

	enable_interrupts();

	return 0;
}

