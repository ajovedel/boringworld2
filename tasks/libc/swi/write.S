/** @file write.S
 *
 * @brief write sycall wrapper
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 07 Oct 2007 01:53:46 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-29
 */

#include <asm.h>
#include <bits/swi.h>

	.file "write.S"

FUNC(write)
	swi WRITE_SWI
	cmp r0, #0	@verify return value
	blt .ERR
.RET:
	mov pc, lr
.ERR:
	ldr r3, =errno		@get errno address
	neg r0, r0			@2s complement of error number
	str r0, [r3] 		@store error number in errno
	mov r0, #-1			@return value is -1
	b .RET				@return	
