/** @file proc.c
 * 
 * @brief Implementation of `process' syscalls
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 14 Oct 2007 00:07:38 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-11-12
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
    return -EFAULT; // XXX Some different error ??

  size_t i = 0;
  for(; i < num_tasks; i++)
  {
    task_ptrs[i] = tasks + i;
  }

  allocate_tasks(task_ptrs, num_tasks);
  dev_init();

  dispatch_nosave();
  
  return 1; /* remove this line after adding your code */
}

int event_wait(unsigned int dev)
{
  //printf("Inside event wait on device %d\n", dev);
  disable_interrupts();
  if(dev > NUM_DEVICES)
    return -EINVAL;

  dev_wait(dev);
  //printf("Resuming from event wait on device %d\n",dev);
  return 0;
}

/* An invalid syscall causes the kernel to exit. */
void invalid_syscall(unsigned int call_num  __attribute__((unused)))
{
	printf("Kernel panic: invalid syscall -- 0x%08x\n", call_num);

	disable_interrupts();
	while(1);
}
