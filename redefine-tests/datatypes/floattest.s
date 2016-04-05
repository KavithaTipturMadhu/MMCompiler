	.file	"/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/datatypes/floattest.ll"
	.text
	.globl	test
	.align	4
	.type	test,@function
HyperOp#0:                              # @test
.SMD	1100

pHyperOp:
.LBB0_0:
	lui	260096, x5
	addi	x5, x0, 0
	addi	x6, x0, 8
	sw	x6, x5, 0
	addi	x5, x0, 0
	lui	266496, x6
	addi	x6, x0, 0
	sw	x5, x6, 0
	addi	x5, x0, 12
	sw	x5, x0, 0
	addi	x5, x0, 4
	addi	x6, x0, 10
	sw	x5, x6, 0
.LBB0_1:
	addi	x5, x0, 4
	lw	x5, x5, 0
	addi	x6, x0, 12
	lw	x6, x6, 0
	bge	x6, x5, .LBB0_4
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	addi	x5, x0, 0
	flw	f0, x5, 0
	addi	x5, x0, 8
	flw	f1, x5, 0
	fmul.s	f0, f1, f0
	fsw	x5, f0, 0
.LBB0_3:
	addi	x5, x0, 12
	lw	x6, x5, 0
	addi	x6, x6, 1
	sw	x5, x6, 0
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_4:
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

	.section	".note.GNU-stack","",@progbits
