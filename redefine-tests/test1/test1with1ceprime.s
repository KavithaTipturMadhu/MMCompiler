	.file	"/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/test1/test1.ll"
	.text
	.globl	test
	.align	4
	.type	test,@function
test:                                   # @test

pHyperOp:
.LBB0_0:
	addi	x6, x0, 28
	lui	x5, 0
	readpm	x5, x5, 0
	readpm	x11, x5, 4
	sw	x11, x5, 0
	readpm	x7, x5, 8
	readpm	x5, x5, 12
	lw	x12, x7, 0
	readpm	x7, x5, 16
	readpm	x5, x5, 20
	lw	x5, x7, 0
	lui	x5, 2
	writepm	x5, x5, 0
	lui	x5, 1
	writepm	x5, x1, 0
	writepm	x5, x2, 8
	lui	x5, 3
	writepm	x5, x12, 16
	readpm	x12, x5, 24
	readpm	x14, x5, 28
	add	x12, x12, x14
	writepm	x5, x12, 16
	readpm	x10, x5, 32
	readpm	x7, x5, 36
	sw	x7, x10, 0
	writepm	x5, x5, 20
	writepm	x5, x5, 32
	writepm	x5, x12, 36
	readpm	x5, x5, 40
	sw	x6, x5, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

pHyperOp:
.LBB0_0:
	addi	x10, x0, 24
	addi	x7, x0, 8
	lui	x5, 3
	writepm	x5, x7, 4
	lui	x5, 0
	writepm	x5, x7, 8
	addi	x7, x0, 16
	writepm	x5, x7, 12
	writepm	x5, x7, 16
	addi	x7, x0, 20
	lui	x5, 1
	readpm	x1, x5, 0
	readpm	x13, x5, 4
	sw	x13, x1, 0
	lui	x5, 2
	writepm	x5, x5, 8
	readpm	x2, x5, 8
	readpm	x13, x5, 12
	sw	x13, x2, 0
	writepm	x5, x5, 16
	readpm	x12, x5, 16
	sw	x10, x12, 0
	writepm	x5, x5, 24
	writepm	x5, x7, 36
	readpm	x5, x5, 20
	lw	x7, x7, 0
	writepm	x5, x7, 28
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

pHyperOp:
.LBB0_0:
	addi	x11, x0, 4
	lui	x5, 0
	writepm	x5, x11, 4
	lui	x5, 3
	writepm	x5, x3, 0
	writepm	x5, x4, 8
	lui	x5, 2
	readpm	x5, x5, 0
	lw	x11, x11, 0
	readpm	x13, x5, 4
	readpm	x5, x5, 8
	lw	x14, x13, 0
	readpm	x13, x5, 12
	readpm	x5, x5, 16
	lw	x15, x13, 0
	writepm	x5, x15, 20
	writepm	x5, x14, 28
	readpm	x13, x5, 20
	readpm	x5, x5, 24
	lw	x10, x13, 0
	writepm	x5, x11, 24
	writepm	x5, x10, 28
	readpm	x7, x5, 28
	readpm	x5, x5, 32
	mul	x5, x5, x7
	writepm	x5, x5, 32
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

pHyperOp:
.LBB0_0:
	lui	x5, 0
	writepm	x5, x5, 0
	lui	x5, 3
	readpm	x3, x5, 0
	readpm	x7, x5, 4
	sw	x7, x3, 0
	writepm	x5, x5, 12
	readpm	x4, x5, 8
	readpm	x7, x5, 12
	sw	x7, x4, 0
	writepm	x5, x5, 20
	addi	x13, x0, 12
	lui	x5, 1
	writepm	x5, x13, 4
	lui	x5, 2
	writepm	x5, x13, 4
	addi	x13, x0, 0
	writepm	x5, x13, 12
	writepm	x5, x13, 12
	readpm	x12, x5, 16
	readpm	x15, x5, 20
	add	x12, x15, x12
	writepm	x5, x12, 24
	writepm	x5, x13, 20
	readpm	x11, x5, 24
	readpm	x10, x5, 28
	add	x10, x10, x11
	writepm	x5, x10, 32
	readpm	x5, x5, 32
	readpm	x12, x5, 36
	add	x5, x5, x12
	writepm	x5, x5, 40
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

	.section	".note.GNU-stack","",@progbits
