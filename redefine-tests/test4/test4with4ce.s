	.file	"/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/test4/test4merged.ll"
	.text
	.globl	test
	.align	4
	.type	test,@function
test:                                   # @test

pHyperOp:
.LBB0_0:
	addi	x5, x0, 16
	lui	2, x6
	addi	x6, x0, 4
	sw	x5, x6, 0
	addi	x5, x0, 4
	lui	2, x6
	addi	x6, x0, 0
	sw	x5, x6, 0
	addi	x5, x0, 12
	addi	x6, x0, 2
	sw	x5, x6, 0
	addi	x5, x0, 32
	addi	x6, x0, 1
	sw	x5, x6, 0
	addi	x5, x0, 5
	addi	x6, x0, 8
	sw	x6, x5, 0
	addi	x5, x0, 36
	lw	x6, x6, 0
	sw	x5, x6, 0
.LBB0_1:
	addi	x5, x0, 36
	lw	x5, x5, 0
	addi	x6, x0, 32
	lw	x6, x6, 0
	bge	x6, x5, .LBB0_8
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	addi	x6, x0, 2
	addi	x5, x0, 32
	lw	x5, x5, 0
	sw	x5, x0, 40
	beq	x5, x6, .LBB0_5
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_3:
	addi	x6, x0, 1
	addi	x5, x0, 40
	bne	x5, x6, .LBB0_6
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_4
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_4:
	addi	x5, x0, 20
	addi	x6, x0, 32
	addi	x7, x0, 4
	lw	x6, x6, 0
	lw	x7, x7, 0
	mul	x6, x6, x7
	sw	x5, x6, 0
	jal 	.LBB0_7
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_5:
	addi	x5, x0, 20
	addi	x7, x0, 32
	addi	x6, x0, 3
	lw	x7, x7, 0
	div	x6, x7, x6
	sw	x5, x6, 0
	jal 	.LBB0_7
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_6:
	addi	x5, x0, 32
	lw	x6, x5, 0
	addi	x5, x0, 20
	lw	x7, x5, 0
	mul	x6, x7, x6
	sw	x5, x6, 0
.LBB0_7:
	addi	x6, x0, 24
	addi	x7, x0, 8
	addi	x5, x0, 32
	lw	x7, x7, 0
	lw	x10, x5, 0
	add	x7, x10, x7
	sw	x6, x7, 0
	addi	x6, x0, 28
	addi	x10, x0, 20
	lw	x10, x10, 0
	add	x7, x10, x7
	sw	x6, x7, 0
	lw	x6, x5, 0
	addi	x6, x6, 1
	sw	x5, x6, 0
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_8:
	addi	x5, x0, 21
	addi	x6, x0, 28
	lw	x6, x6, 0
	blt	x6, x5, .LBB0_10
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_9
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_9:
	addi	x5, x0, 0
	addi	x6, x0, 28
	lw	x6, x6, 0
	sw	x5, x6, 0
.LBB0_10:
	addi	x5, x0, 20
	addi	x6, x0, 0
	sw	x6, x5, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

	.section	".note.GNU-stack","",@progbits
