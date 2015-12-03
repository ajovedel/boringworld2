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
  // the lowest period (T) to the highest.
  for(i=0; i<num_tasks; i++){
      for(j=(i+1); j<num_tasks; j++){
          if((*tasks)[i].T > (*tasks)[j].T){
              temp_task = (*tasks)[i];
              (*tasks)[i] = (*tasks)[j];
              (*tasks)[j] = temp_task;
          }
      }
  }     

	return 1;	
}
