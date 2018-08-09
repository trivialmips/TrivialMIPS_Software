	.set	nomips16
	.set	nomicromips
	.text
	.extern _Z6_entryv
	.section .text.startup
	.globl _start
	.type _start, @function
_start:
	# setup exception handler
	lui		$t2, 0x0040
	mtc0	$t2, $12 # c0_status
	mtc0	$zero, $13 # c0_cause
	# setup stack pointer
	la 		$sp, _stack
	la		$gp, _gp
	# jump to our code
	jal 	_Z6_entryv
	nop

_loop0:
	b _loop0
	nop

_exception:
    .org 0x380
    jal exception
    nop
    j _loop0
    nop 
