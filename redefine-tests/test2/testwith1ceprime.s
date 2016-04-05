	.file	"/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/test2/test2.ll"
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
	lw	x13, x7, 0
	readpm	x7, x5, 16
	readpm	x5, x5, 20
	lw	x5, x7, 0
	lui	x5, 2
	writepm	x5, x5, 0
	lui	x5, 1
	writepm	x5, x1, 0
	lui	x5, 3
	writepm	x5, x13, 20
	readpm	x11, x5, 24
	readpm	x10, x5, 28
	sw	x10, x11, 0
	readpm	x10, x5, 32
	readpm	x5, x5, 36
	lw	x10, x10, 0
	writepm	x5, x10, 12
	readpm	x7, x5, 40
	mul	x5, x5, x7
	writepm	x5, x5, 16
	writepm	x5, x6, 24
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
	readpm	x11, x5, 4
	sw	x11, x1, 0
	lui	x5, 2
	writepm	x5, x5, 8
	writepm	x5, x10, 28
	addi	x10, x0, 0
	writepm	x5, x10, 28
	writepm	x5, x10, 32
	readpm	x12, x5, 8
	readpm	x10, x5, 12
	add	x10, x10, x12
	writepm	x5, x10, 12
	writepm	x5, x7, 16
	writepm	x5, x7, 32
	readpm	x5, x5, 16
	readpm	x11, x5, 20
	add	x5, x5, x11
	writepm	x5, x5, 20
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

pHyperOp:
.LBB0_0:
	addi	x11, x0, 4
	lui	x5, 0
	writepm	x5, x11, 4
	lui	x5, 3
	writepm	x5, x2, 0
	writepm	x5, x3, 8
	lui	x5, 2
	readpm	x5, x5, 0
	lw	x12, x11, 0
	readpm	x11, x5, 4
	readpm	x5, x5, 8
	lw	x11, x11, 0
	writepm	x5, x11, 16
	writepm	x5, x4, 24
	lui	x5, 1
	writepm	x5, x12, 8
	readpm	x10, x5, 12
	readpm	x7, x5, 16
	sw	x7, x10, 0
	writepm	x5, x5, 36
	readpm	x5, x5, 20
	readpm	x6, x5, 24
	sw	x6, x5, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

pHyperOp:
.LBB0_0:
	lui	x5, 0
	writepm	x5, x5, 0
	lui	x5, 3
	readpm	x2, x5, 0
	readpm	x7, x5, 4
	sw	x7, x2, 0
	writepm	x5, x5, 12
	readpm	x3, x5, 8
	readpm	x7, x5, 12
	sw	x7, x3, 0
	writepm	x5, x5, 20
	addi	x11, x0, 12
	lui	x5, 1
	writepm	x5, x11, 4
	lui	x5, 2
	writepm	x5, x11, 4
	readpm	x11, x5, 16
	readpm	x13, x5, 20
	add	x11, x13, x11
	writepm	x5, x11, 24
	readpm	x4, x5, 24
	readpm	x10, x5, 28
	sw	x10, x4, 0
	writepm	x5, x5, 36
	readpm	x7, x5, 32
	readpm	x5, x5, 36
	lw	x7, x7, 0
	writepm	x5, x7, 40
	writepm	x5, x11, 20
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

	.section	".note.GNU-stack","",@progbits
