/** @file sched.c
 * 
 * @brief Top level implementation of the scheduler.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-20
 */

#include <types.h>
#include <assert.h>

#include <kernel.h>
#include <config.h>
#include "sched_i.h"

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>

tcb_t system_tcb[OS_MAX_TASKS]; /*allocate memory for system TCBs */

void sched_init(task_t* main_task  __attribute__((unused)))
{
	
}

/**
 * @brief This is the idle task that the system runs when no other task is runnable
 */
 
static void __attribute__((unused)) idle(void)
{
	 enable_interrupts();
	 while(1);
}

void initialize_idle()
{
  tcb_t *idle_tcb = &(system_tcb[IDLE_PRIO]);
  idle_tcb->native_prio = IDLE_PRIO;
  idle_tcb->device_prio = IDLE_PRIO;  

  // Context
  sched_context_t *idle_ctx = &(idle_tcb->context);
  idle_ctx->r4 = (uint32_t)idle;
  idle_ctx->r5 = 0;
  idle_ctx->r6 = 0;
  idle_ctx->r7 = 0;
  idle_ctx->r8 = global_data;
  idle_ctx->r9 = 0;
  idle_ctx->r10 = 0;
  idle_ctx->r11 = 0;
  idle_ctx->sp = (uint32_t)((char *)(idle_tcb->kstack) + OS_KSTACK_SIZE);
  //XXX What about using kstack_high ??
  idle_ctx->lr = (uint32_t)launch_task;
  
  idle_tcb->holds_lock = 0;
  idle_tcb->sleep_queue = (tcb_t *)0;

}

/**
 * @brief Allocate user-stacks and initializes the kernel contexts of the
 * given threads.
 *
 * This function assumes that:
 * - num_tasks < number of tasks allowed on the system.
 * - the tasks have already been deemed schedulable and have been appropriately
 *   scheduled.  In particular, this means that the task list is sorted in order
 *   of priority -- higher priority tasks come first.
 *
 * @param tasks  A list of scheduled task descriptors.
 * @param size   The number of tasks is the list.
 */
void allocate_tasks(task_t** tasks, size_t num_tasks)
{

  int i = 0;
  for(; i < num_tasks; i++)
  {
    //XXX Check lamda and others for valid address ranges
    tcb_t *idle_tcb = &(system_tcb[i]);
    idle_tcb->native_prio = i;
    idle_tcb->device_prio = i;  

    // Context
    sched_context_t *ctx = &(idle_tcb->context);
    ctx->r4 = (uint32_t) (tasks[i]->lambda);
    ctx->r5 = (uint32_t) (tasks[i]->data);
    ctx->r6 = (uint32_t) (tasks[i]->sp);
    ctx->r8 = global_data;
    ctx->sp = (uint32_t)((char *)(ctx->kstack) + OS_KSTACK_SIZE);
    // Easy way to launch task fo first time
    ctx->lr = (uint32_t)launch_task;

  }

  initialize_idle();
  runqueue_init();
  
  for(i=0; i < num_tasks; i++)
  {
    runqueue_add(&(system_tcb[i]), i);
  }
  // Set current tcb as IDLE
  dispatch_init(&(system_tcb[IDLE_PRIO]));
  
}

