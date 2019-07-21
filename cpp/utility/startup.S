	.globl 		_start
	.section	.text.startup
	.set        noreorder
_start:
.org 0x0

	# write the segment to indicate boot progress
    li      $s2, 0xbff0f010
    li      $t3, 0x00000001
    sw      $t3, 0($s2)

#ifndef CODE_INTO_BOOTROM
	# setup exception handler
	# la		$t1, _text
    mtc0    $zero, $15, 1 # set c0_ebase to 0
    mtc0    $zero, $12 # use ebase and disable interrupts
#endif

	# progress: 2
	li      $t3, 0x00000002
    sw      $t3, 0($s2)

	# setup stack pointer
    la 		$sp, _stack
    la		$gp, _gp

	# progress: 3
	li      $t3, 0x00000003
    sw      $t3, 0($s2)

	# jump to our code
    jal 	_main
    nop

_loop:
    li      $t3, 0x00000000
_loop_body:
	li		$s1, 0xbff0e000   # $s1 = 0xbfd0e0000 (100 MHz Clock)
	lw		$t1, 0($s1)       # $t1 = time
	li      $t2, 0x02faf080    # $t2 = 0x02faf080 (50000000)
	add     $t0, $t1, $t2     # $t0 = $t1 + 0.5s

_wait:
    lw      $t1, 0($s1)
    beq     $t0, $t1, _switch_led_status
    nop
    b       _wait
    nop

_switch_led_status:
    li      $s2, 0xbff0f010      # write 0xbfd0f010 (numbers)
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
