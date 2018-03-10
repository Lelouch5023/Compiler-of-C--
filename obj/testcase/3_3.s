.data
_T0: .word 0
_T1: .word 0
_T2: .word 0
_T3: .word 0
_T4: .word 0
_T5: .word 0
_T6: .word 0
_T7: .word 0
_T8: .word 0
_T9: .word 0
_T10: .word 0
_T11: .word 0
_T12: .word 0
_T13: .word 0
_T14: .word 0
_T15: .word 0
_T16: .word 0
_T17: .word 0
_T18: .word 0
_T19: .word 0
_T20: .word 0
_T21: .word 0
_T22: .word 0
_T23: .word 0
_T24: .word 0
_T25: .word 0
_T26: .word 0
_T27: .word 0
_T28: .word 0
_T29: .word 0
_T30: .word 0
_T31: .word 0
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
_read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

_write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra


_power:
  move	$s1, $a1
  move	$s2, $a0
  li	$t0, 1
  sw	$t0, _T1
  lw	$s3, _T1
label1:
  move	$t0, $s2
  sw	$t0, _T2
  li	$t0, 0
  sw	$t0, _T3
  lw  	$t0, _T2
  lw  	$t1, _T3
  bgt 	$t0, $t1, label2
  j   	label3
label2:
  move	$t0, $s2
  sw	$t0, _T8
  li	$t0, 2
  sw	$t0, _T9
  lw  	$t0, _T9
  lw  	$t1, _T8
  div 	$t1, $t0
  mflo	$t2
  sw  	$t2, _T6
  li	$t0, 2
  sw	$t0, _T7
  lw  	$t0, _T7
  lw  	$t1, _T6
  mul 	$t2, $t1, $t0
  sw  	$t2, _T4
  move	$t0, $s2
  sw	$t0, _T5
  lw  	$t0, _T4
  lw  	$t1, _T5
  bne 	$t0, $t1, label4
  j   	label5
label4:
  move	$t0, $s3
  sw	$t0, _T11
  move	$t0, $s1
  sw	$t0, _T12
  lw  	$t0, _T12
  lw  	$t1, _T11
  mul 	$t2, $t1, $t0
  sw  	$t2, _T10
  lw	$s3, _T10
label5:
  move	$t0, $s1
  sw	$t0, _T14
  move	$t0, $s1
  sw	$t0, _T15
  lw  	$t0, _T15
  lw  	$t1, _T14
  mul 	$t2, $t1, $t0
  sw  	$t2, _T13
  lw	$s1, _T13
  move	$t0, $s2
  sw	$t0, _T17
  li	$t0, 2
  sw	$t0, _T18
  lw  	$t0, _T18
  lw  	$t1, _T17
  div 	$t1, $t0
  mflo	$t2
  sw  	$t2, _T16
  lw	$s2, _T16
  j   	label1
label3:
  move	$t0, $s3
  sw	$t0, _T19
  lw  	$v0, _T19
  jr  	$ra

_sub:
  move	$s4, $a1
  move	$s5, $a0
  move	$t0, $s4
  sw	$t0, _T21
  move	$t0, $s5
  sw	$t0, _T22
  lw  	$t0, _T22
  lw  	$t1, _T21
  sub 	$t2, $t1, $t0
  sw  	$t2, _T20
  lw  	$v0, _T20
  jr  	$ra

main:
  addi	$sp, $sp, -4
  sw  	$ra, 0($sp)
  jal 	_read
  lw  	$ra, 0($sp)
  addi	$sp, $sp, 4
  sw  	$v0, _T23
  lw	$s6, _T23
  addi	$sp, $sp, -4
  sw  	$ra, 0($sp)
  jal 	_read
  lw  	$ra, 0($sp)
  addi	$sp, $sp, 4
  sw  	$v0, _T24
  lw	$s7, _T24
  move	$t0, $s6
  sw	$t0, _T26
  move	$t0, $s7
  sw	$t0, _T27
  lw	$a0, _T27
  lw	$a1, _T26
  addi	$sp, $sp, -4
  sw  	$ra, 0($sp)
  jal 	_power
  lw  	$ra, 0($sp)
  addi	$sp, $sp, 4
  sw  	$v0, _T25
  lw  	$a0, _T25
  addi	$sp, $sp, -4
  sw  	$ra, 0($sp)
  jal 	_write
  lw  	$ra, 0($sp)
  addi	$sp, $sp, 4
  move	$t0, $s6
  sw	$t0, _T29
  move	$t0, $s7
  sw	$t0, _T30
  lw	$a0, _T30
  lw	$a1, _T29
  addi	$sp, $sp, -4
  sw  	$ra, 0($sp)
  jal 	_sub
  lw  	$ra, 0($sp)
  addi	$sp, $sp, 4
  sw  	$v0, _T28
  lw  	$a0, _T28
  addi	$sp, $sp, -4
  sw  	$ra, 0($sp)
  jal 	_write
  lw  	$ra, 0($sp)
  addi	$sp, $sp, 4
  li	$t0, 0
  sw	$t0, _T31
  lw  	$v0, _T31
  jr  	$ra
