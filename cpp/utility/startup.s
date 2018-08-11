	.globl 		_start
	.section	.text.startup
_start:
.org 0x0
	# setup exception handler
	# la		$t1, _text
	mtc0	$zero, $15, 1 # set c0_ebase to 0
	mtc0	$zero, $12 # use ebase and disable interrupts
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
.org 0x180
    jal _exception_handler
    nop
    j _loop0
    nop
