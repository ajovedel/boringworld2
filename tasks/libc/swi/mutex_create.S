/** @file mutex_create.S
 *
 * @brief mutex_create sycall wrapper
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-10-31
 */

#include <asm.h>
#include <bits/swi.h>

	.file "mutex_create.S"

FUNC(mutex_create)
    swi MUTEX_CREATE
    cmp r0, #0
    blt .ERR
.RET:
    mov pc, lr
.ERR:
    ldr r3, =errno
    neg r0, r0
    str r0, [r3]
    mov r0, #-1
    b   .RET
