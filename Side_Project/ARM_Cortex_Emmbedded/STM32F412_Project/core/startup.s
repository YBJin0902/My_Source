.global reset_handler
.global exit
.syntax unified

.section .init, "ax"
.code 16
.align 2
.thumb_func


reset_handler:
    ldr r0, =__data_start__
	ldr r1, =_vectors
	str r1, [r0]

	/***************/
	/* Stack setup */
	/***************/

	ldr r1, =__stack_end__
	mov sp, r1

	/* Setup initial call frame */
	mov r0, #0
	mov lr, r0
	mov r12, sp


@===================================================================
@ start function
@===================================================================
	.type start, function

start:
	/* Jump to C bootstrap function */
	ldr r2, =cstartup @ the cstartup is in system.c
	blx r2

.section .text, "ax"
.code 16
.align 2
.thumb_func


@===================================================================
@ exit function
@===================================================================
	.thumb_func
	/* Returned from application entry point, loop forever. */
exit:
	b exit
