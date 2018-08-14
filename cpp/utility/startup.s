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

_loop:
    lui     $t4, 0x8000
	lui     $t3, 0x0000
_loop_body:
	lui		$s1, 0xa400       # $s1 = 0xa4000000 (timer in us)
	lw		$t1, 0($s1)       # read time
	lui     $t2, 0x7          # $t2 = 0x7A120 (500000)
	addiu   $t2, $t2, 0xA120
	add     $t0, $t1, $t2     # $t0 = $t1 + 500000 us

_wait:
    lw		$t1, 0($s1)
	beq		$t0, $t1, _switch_led_status
	nop
	b		_wait
	nop

_switch_led_status:
	lui     $s2, 0xa600      # read 0xa6000008 (segment)
	nor     $t3, $t3, $t3    # $t3 = ~$t3
	or      $t3, $t3, $t4    # disable decoding
	sw      $t3, 0x4($s2)
	b       _loop_body
	nop

_exception:
.org 0x180
    jal _exception_handler
    nop
    j _loop
    nop
