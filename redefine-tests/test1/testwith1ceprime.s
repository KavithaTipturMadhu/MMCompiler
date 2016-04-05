	.file	"/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/test1/test1.ll"
	.text
	.globl	test
	.align	4
	.type	test,@function
test:                                   # @test

pHyperOp:
.LBB0_0:
	addi	x6, x0, 28
	addi	x12, x0, 24
	addi	x11, x0, 4
	sw	x11, x1, 0
	addi	x7, x0, 8
	sw	x7, x2, 0
	lw	x14, x7, 0
	addi	x7, x0, 16
	sw	x7, x3, 0
	lw	x7, x7, 0
	addi	x10, x0, 20
	lw	x11, x11, 0
	addi	x5, x0, 12
	sw	x5, x4, 0
	lw	x15, x5, 0
	addi	x13, x0, 0
	sw	x13, x5, 0
	lw	x5, x13, 0
	add	x5, x5, x14
	add	x5, x5, x15
	sw	x12, x5, 0
	lw	x12, x13, 0
	add	x11, x12, x11
	sw	x10, x11, 0
	lw	x10, x10, 0
	mul	x7, x7, x10
	add	x5, x7, x5
	sw	x6, x5, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

	.section	".note.GNU-stack","",@progbits
