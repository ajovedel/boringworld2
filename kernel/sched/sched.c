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
  idle_tcb->cur_prio = IDLE_PRIO;  

  // Context
  sched_context_t *idle_ctx = &(idle_tcb->context);
  idle_ctx->r4 = (uint32_t)&idle;
  idle_ctx->r5 = 0;
  idle_ctx->r6 = 0;
  idle_ctx->r7 = 0;
  idle_ctx->r8 = global_data;
  idle_ctx->r9 = 0;
  idle_ctx->r10 = 0;
  idle_ctx->r11 = 0;
  idle_ctx->sp = ((char *)(idle_tcb->kstack) + OS_KSTACK_SIZE);
  idle_ctx->lr = (void *)launch_task;
  
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
  size_t i = 1;
  for(; i <= num_tasks; i++)
  {
    tcb_t *task_tcb = &(system_tcb[i]);
    task_tcb->native_prio = i;
    task_tcb->cur_prio = i;  

    // Context
    sched_context_t *ctx = &(task_tcb->context);
    ctx->r4 = (uint32_t) (tasks[i-1]->lambda);

    ctx->r5 = (uint32_t) (tasks[i-1]->data);
    ctx->r6 = (uint32_t) (tasks[i-1]->stack_pos);
    ctx->r8 = global_data;
    ctx->sp = ((char *)(task_tcb->kstack) + OS_KSTACK_SIZE);
    // Easy way to launch task fo first time
    ctx->lr = (void *)launch_task;

    task_tcb->holds_lock = 0;
    task_tcb->sleep_queue = (tcb_t *)0;

  }

  initialize_idle();
  runqueue_init();
  
  runqueue_add(&(system_tcb[IDLE_PRIO]), IDLE_PRIO);
  for(i=0; i < num_tasks; i++)
  {
    runqueue_add(&(system_tcb[i]), i);
  }
  // Set current tcb as IDLE
  dispatch_init(&(system_tcb[IDLE_PRIO]));
  
}


