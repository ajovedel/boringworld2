/** @file ub_test.c
 * 
 * @brief The UB Test for basic schedulability
 *
 * @author Alejandro Jove (ajovedel)
 * @author Vishnu Gorantla (vishnupg)
 *
 */

//#define DEBUG 0

#include <sched.h>
#ifdef DEBUG
#include <exports.h>
#endif

static double ub_values[63] = {1.000000, 0.828427, 0.779763, 0.756828, 0.743492, 0.734772, 0.728627, 0.724062, 0.720538, 0.717735, 0.715452, 0.713557, 0.711959, 0.710593, 0.709412, 0.708381, 0.707472, 0.706666, 0.705946, 0.705298, 0.704713, 0.704182, 0.703698, 0.703254, 0.702846, 0.702469, 0.702121, 0.701798, 0.701497, 0.701217, 0.700955, 0.700709, 0.700478, 0.700261, 0.700056, 0.699863, 0.699681, 0.699508, 0.699343, 0.699188, 0.69904, 0.698898, 0.698764, 0.698636, 0.698513, 0.698396, 0.698284, 0.698176, 0.698073, 0.697974, 0.697879, 0.697788, 0.6977, 0.697615, 0.697533, 0.697455, 0.697379, 0.697306, 0.697235, 0.697166, 0.6971, 0.697036, 0.696974}; 
 
/**
 * @brief Perform UB Test and reorder the task list.
 *
 * The task list at the end of this method will be sorted in order is priority
 * -- from highest priority (lowest priority number) to lowest priority
 * (highest priority number).
 *
 * @param tasks  An array of task pointers containing the task set to schedule.
 * @param num_tasks  The number of tasks in the array.
 *
 * @return 0  The test failed.
 * @return 1  Test succeeded.  The tasks are now in order.
 */
int assign_schedule(task_t** tasks, size_t num_tasks)
{
  unsigned i, j;
  task_t temp_task;

  // basic sort that orders that swaps the task and places them from 
  // the lowest period (T) to the highest. This is rate-monotonic scheduling.
  for(i=0; i<num_tasks; i++){
      for(j=(i+1); j<num_tasks; j++){
          if((*tasks)[i].T > (*tasks)[j].T){
              temp_task = (*tasks)[i];
              (*tasks)[i] = (*tasks)[j];
              (*tasks)[j] = temp_task;
          }
      }
  }
  
  task_t *cur_task;
  double task_run_time = 0.0;
  double total_preemption = 0.0;

  // do the UB test to check if given tasks are schedulable or not
  for(i=0; i<num_tasks; i++){
    cur_task = &(*tasks)[i];
    task_run_time = (double)(cur_task->C + cur_task->B / cur_task->T);
  
    // verify if current task is schedulable  
    if((task_run_time + total_preemption) > ub_values[i]) 
      return 0;

    total_preemption += (double)(cur_task->C / cur_task->T);

    }

	return 1;	
}
