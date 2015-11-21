/** @file read.S
 *
 * @brief read sycall wrapper
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 07 Oct 2007 01:53:16 -0400
 * 
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-29
 */

#include <asm.h>
#include <bits/swi.h>

	.file "read.S"

FUNC(read)
	swi READ_SWI
	cmp r0, #0	@verify return value
	blt .ERR
.RET:
	mov pc, lr
.ERR:
	ldr r3, =errno	@get errno addr
	neg r0, r0	@2s complement of returned error value
	str r0, [r3]	@store negated return value in ERRNO var
	mov r0, #-1	@set return value as -1
	b .RET		


