.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
_T1: .word 0
_T2: .word 0
_T3: .word 0
.globl main
.text
read:
  li	$v0, 4
  la	$a0, _prompt
  syscall
  li	$v0, 5
  syscall
  jr	$ra

write:
  li	$v0, 1
  syscall
  li	$v0, 4
  la	$a0, _ret
  syscall
  move	$v0, $0
  jr	$ra

main:
 
  li	$t0, 1
  sw	$t0, _T1

  li	$t0, 2
  sw	$t0, _T2

  lw	$t0, _T1
  lw	$t1, _T2
  add	$t2, $t1, $t0
  sw	$t2, _T3

  lw	$t0, _T3
  move	$a0, $t0
  addi	$sp, $sp, -4
  sw	$ra, 0($sp)
  jal write
  lw	$ra, 0($sp)
  addi	$sp, $sp, 4
  
 
  move $v0, $0
  jr $ra



