/*
 * void cswitch(struct kctx *from, struct kctx *to);
 */

	.globl cswitch
cswitch:
	// eax <- old kernel contex starting address
	movl	  4(%esp), %eax	/* %eax <- from */
	// edx <- new kernel contex starting address
	movl	  8(%esp), %edx	/* %edx <- to */

	movl	%esp,	(%eax)
	movl	%edi,	4(%eax)
	movl	%esi,	8(%eax)
	movl	%ebx,	12(%eax)
	movl	%ebp,	16(%eax)
	movl	(%esp),	%ebx
	movl	%ebx,	20(%eax)
	/* TODO: save the old kernel context */

	movl    (%edx),     %esp
	movl	20(%edx),	%ebx
	movl	%ebx,		(%esp)
	movl	4(%edx),	%edi
	movl	8(%edx),	%esi
	movl	12(%edx),	%ebx
	movl	16(%edx),	%ebp
	/* TODO: load the new kernel context */

	xor     %eax, %eax
	ret
