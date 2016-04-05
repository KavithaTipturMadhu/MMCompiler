	.file	"/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/test5/test5.ll"
	.text
	.globl	test
	.align	4
	.type	test,@function
test:                                   # @test

pHyperOp:
.LBB0_0:
	addi	x5, x0, 4
	sw	x5, x0, 0
	addi	x5, x0, 8
	addi	x6, x0, 12
	sw	x6, x5, 0
.LBB0_1:
	addi	x5, x0, 4
	lw	x5, x5, 0
	blt	x0, x5, .LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	addi	x5, x0, 12
	lw	x6, x5, 0
	addi	x5, x0, 4
	lw	x7, x5, 0
	slli	x10, x7, 2
	add	x6, x6, x10
	addi	x13, x0, 0
	lw	x12, x5, 0
	slli	x11, x12, 2
	add	x11, x13, x11
	add	x10, x13, x10
	mul	x12, x12, x12
	sw	x11, x12, 0
	lw	x10, x10, 0
	add	x7, x10, x7
	sw	x6, x7, 0
	lw	x6, x5, 0
	addi	x6, x6, 1
	sw	x5, x6, 0
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_3:
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

	.section	".note.GNU-stack","",@progbits
