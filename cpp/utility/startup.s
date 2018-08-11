	.set		noreorder
	.globl 		_start
	.section	.text.startup
_start:
	# setup exception handler
	la		$t1, _exception
	lui		$t2, 0x0040
	mtc0	$t1, $15, 1 # c0_ebase
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

	.section	.text.ebase
_exception:
    jal exception_handler
    nop
    j _loop0
    nop 
