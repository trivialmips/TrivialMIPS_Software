	.set		noreorder
	.globl 		_start
	.section	.text.startup
_start:
	# setup exception handler
	lui		$t2, 0x0040
	mtc0	$t2, $12 # c0_status
	mtc0	$zero, $13 # c0_cause
	# setup stack pointer
	la 		$sp, _stack
	la		$gp, _gp
	# jump to our code
	jal 	_main
	nop

_loop0:
	b _loop0
	nop

_exception:
    .org 0x380
    jal exception_handler
    nop
    j _loop0
    nop 
