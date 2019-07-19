	.globl 		_start
	.section	.text.startup
_start:
.org 0x0

#ifndef CODE_INTO_BOOTROM
	# setup exception handler
	# la		$t1, _text
	mtc0	$zero, $15, 1 # set c0_ebase to 0
	mtc0	$zero, $12 # use ebase and disable interrupts
#endif

	# setup stack pointer
	la 		$sp, _stack
	la		$gp, _gp
	# jump to our code
	jal 	_main
	nop

_loop:
	lui     $t3, 0x0000
_loop_body:
	lui		$s1, 0xbfd0       # $s1 = 0xbfd0e0000 (100 MHz Clock)
	addiu   $s1, $s1, 0xe000
	lw		$t1, 0($s1)       # $t1 = time
	lui     $t2, 0x2fa        # $t2 = 0x02faf080 (50000000)
	addiu   $t2, $t2, 0xf080
	add     $t0, $t1, $t2     # $t0 = $t1 + 0.5s

_wait:
    lw		$t1, 0($s1)
	beq		$t0, $t1, _switch_led_status
	nop
	b		_wait
	nop

_switch_led_status:
	lui     $s2, 0xbfd0      # read 0xbfd0f010 (nums)
	addiu   $s2, $s2, 0xf010
	nor     $t3, $t3, $t3    # $t3 = ~$t3
	sw      $t3, 0($s2)
	b       _loop_body
	nop

_exception:

#ifdef CODE_INTO_BOOTROM
.org 0x380
#else
.org 0x180
#endif

    jal _exception_handler
    nop
    j _loop
    nop
