/** @file proc.c
 * 
 * @brief Implementation of `process' syscalls
 *
 * @author Alejandro Jove (ajovedel)
 * @author Vishnu Gorantla (vishnupg)
 *
 */

#include <exports.h>
#include <bits/errno.h>
#include <config.h>
#include <kernel.h>
#include <syscall.h>
#include <sched.h>

#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>
#include <arm/physmem.h>
#include <device.h>

/*
 * @brief Allocates the tasks and initiates the execution of tasks
 *
 * @param tasks List of tasks to execute
 * @param num_tasks number of tasks in the task list
 *
 * return Status code
 */
int task_create(task_t* tasks , size_t num_tasks )
{
  if(num_tasks > OS_AVAIL_TASKS)
  {
    return -EINVAL;
  }
  if(!valid_addr(tasks, num_tasks * sizeof(task_t), USR_START_ADDR,
                          USR_END_ADDR))
  {
    return -EFAULT;
  }

  // Init tcbs
  disable_interrupts();

  task_t **task_ptrs = malloc(num_tasks * sizeof(task_t*));
  if(!task_ptrs)
    return -EFAULT; 

  size_t i = 0;
  for(; i < num_tasks; i++)
  {
    task_ptrs[i] = tasks + i;
  }

  // Sort the tasks
  assign_schedule(task_ptrs, num_tasks);

  allocate_tasks(task_ptrs, num_tasks);
  dev_init();

  dispatch_nosave();
  
  return 1;
}


/*
 * @brief Causes a task to be suspended until an event puts it back in the run queue
 *
 * @param dev Device number of the task
 *
 * @return Status code
 */
int event_wait(unsigned int dev)
{
  //printf("Inside event wait on device %d\n", dev);
  disable_interrupts();
  if(dev > NUM_DEVICES)
    return -EINVAL;

  dev_wait(dev);
  return 0;
}

/* An invalid syscall causes the kernel to exit. */
void invalid_syscall(unsigned int call_num  __attribute__((unused)))
{
	printf("Kernel panic: invalid syscall -- 0x%08x\n", call_num);

	disable_interrupts();
	while(1);
}
