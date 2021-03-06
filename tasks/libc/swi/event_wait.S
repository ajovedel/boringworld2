/** @file event_wait.S
 *
 * @brief time sycall wrapper
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-31
 */

#include <asm.h>
#include <bits/swi.h>

	.file "event_wait.S"

FUNC(event_wait)
	@ Add code here.  Put your name at the top.
  swi EVENT_WAIT
  cmp r0, #0
  rsblt r0, r0, #0
  ldrlt r1, =errno
  strlt r0, [r1]
  mvnlt r0, #0

  mov pc, lr
