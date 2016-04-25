	.text
	.align	4
	.type	main,@function
.topology	0	0               # @main
HyperOp#0:
.SMD	1	1	0	0
.distcnt	0	0	0	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 0
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_1:
	addi	x5, x0, 9
	lui	x5, 2
	writepm	x5, x5, -68
	lui	x5, 3
	writepm	x5, x5, -68
	lui	x5, 4
	writepm	x5, x5, -68
	lui	x5, 2
	readpm	x5, x5, -68
	lui	x5, 3
	readpm	x5, x5, -68
	lui	x5, 4
	readpm	x5, x5, -68
	lui	x5, 0
	readpm	x6, x5, 0
	lui	x5, 1
	writepm	x5, x5, 4
	lui	x5, 2
	writepm	x5, x5, 0
	lui	x5, 3
	writepm	x5, x5, 4
	blt	x5, x6, .LBB0_9
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	addi	x5, x0, 4
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_3:
	addi	x5, x0, 9
	lui	x5, 2
	writepm	x5, x5, -68
	lui	x5, 3
	writepm	x5, x5, -68
	lui	x5, 4
	writepm	x5, x5, -68
	lui	x5, 2
	readpm	x5, x5, -68
	lui	x5, 3
	readpm	x5, x5, -68
	lui	x5, 4
	readpm	x5, x5, -68
	lui	x5, 0
	readpm	x6, x5, 0
	lui	x5, 1
	writepm	x5, x5, 4
	lui	x5, 2
	writepm	x5, x5, 0
	lui	x5, 3
	writepm	x5, x5, 4
	blt	x5, x6, .LBB0_8
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_4
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_4:
	addi	x5, x0, 8
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_5:
	addi	x5, x0, 9
	lui	x5, 2
	writepm	x5, x5, -68
	lui	x5, 3
	writepm	x5, x5, -68
	lui	x5, 4
	writepm	x5, x5, -68
	lui	x5, 2
	readpm	x5, x5, -68
	lui	x5, 3
	readpm	x5, x5, -68
	lui	x5, 4
	readpm	x5, x5, -68
	lui	x5, 0
	readpm	x6, x5, 0
	lui	x5, 1
	writepm	x5, x5, 4
	lui	x5, 2
	writepm	x5, x5, 0
	lui	x5, 3
	writepm	x5, x5, 4
	blt	x5, x6, .LBB0_7
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_6
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_6:
	lui	x6, 0
	addi	x7, x0, 0
	lui	x5, 1
	writepm	x5, x7, 0
	writepm	x5, x6, 4
	lui	x5, 0
	readpm	x6, x5, 0
	readpm	x8, x5, 4
	mul	x12, x6, x8
	writepm	x5, x12, 8
	readpm	x7, x5, 8
	lw	x7, x7, 0
	lui	x13, 0
	lui	x5, 2
	writepm	x5, x13, 8
	readpm	x14, x5, 12
	lw	x14, x14, 0
	writepm	x5, x14, 12
	readpm	x8, x5, 16
	readpm	x11, x5, 20
	add	x8, x8, x11
	readpm	x6, x5, 24
	add	x6, x6, x7
	writepm	x5, x6, 24
	writepm	x5, x8, 28
	readpm	x6, x5, 28
	readpm	x5, x5, 32
	readpm	x5, x5, 36
	readpm	x5, x5, 40
	sw	x5, x6, 0
	lui	x5, 2
	writepm	x5, x5, -68
	lui	x5, 3
	writepm	x5, x5, -68
	lui	x5, 4
	writepm	x5, x5, -68
	lui	x5, 2
	readpm	x5, x5, -68
	lui	x5, 3
	readpm	x5, x5, -68
	lui	x5, 4
	readpm	x5, x5, -68
	jal 	.LBB0_5
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_7:
	addi	x5, x0, 4
	lui	x5, 1
	writepm	x5, x5, 0
	lui	x5, 3
	writepm	x5, x5, 4
	lui	x5, 2
	writepm	x5, x5, -68
	lui	x5, 3
	writepm	x5, x5, -68
	lui	x5, 4
	writepm	x5, x5, -68
	lui	x5, 2
	readpm	x5, x5, -68
	lui	x5, 3
	readpm	x5, x5, -68
	lui	x5, 4
	readpm	x5, x5, -68
	jal 	.LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_8:
	addi	x5, x0, 0
	lui	x5, 1
	writepm	x5, x5, 0
	lui	x5, 3
	writepm	x5, x5, 4
	lui	x5, 2
	writepm	x5, x5, -68
	lui	x5, 3
	writepm	x5, x5, -68
	lui	x5, 4
	writepm	x5, x5, -68
	lui	x5, 2
	readpm	x5, x5, -68
	lui	x5, 3
	readpm	x5, x5, -68
	lui	x5, 4
	readpm	x5, x5, -68
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_9:
	srli	x0, x0, 6
	fdelete 	x0, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp1
.LBB0_0:
	lui	x5, 1
	readpm	x5, x5, 0
	sw	x5, x0, 0
.LBB0_1:
	addi	x6, x0, 0
	lui	x5, 2
	writepm	x5, x6, 0
	lui	x5, 1
	readpm	x5, x5, -84
	lui	x5, 1
	writepm	x5, x5, -84
	lui	x5, 3
	writepm	x5, x5, -84
	lui	x5, 4
	writepm	x5, x5, -84
	lui	x5, 3
	readpm	x5, x5, -84
	lui	x5, 4
	readpm	x5, x5, -84
	lui	x5, 1
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	blt	x5, x6, .LBB0_9
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	lui	x5, 1
	readpm	x5, x5, 0
	sw	x5, x0, 0
.LBB0_3:
	addi	x6, x0, 4
	lui	x5, 2
	writepm	x5, x6, 0
	lui	x5, 1
	readpm	x5, x5, -84
	lui	x5, 1
	writepm	x5, x5, -84
	lui	x5, 3
	writepm	x5, x5, -84
	lui	x5, 4
	writepm	x5, x5, -84
	lui	x5, 3
	readpm	x5, x5, -84
	lui	x5, 4
	readpm	x5, x5, -84
	lui	x5, 1
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	blt	x5, x6, .LBB0_8
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_4
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_4:
	lui	x5, 1
	readpm	x5, x5, 0
	sw	x5, x0, 0
.LBB0_5:
	addi	x6, x0, 8
	lui	x5, 2
	writepm	x5, x6, 0
	lui	x5, 1
	readpm	x5, x5, -84
	lui	x5, 1
	writepm	x5, x5, -84
	lui	x5, 3
	writepm	x5, x5, -84
	lui	x5, 4
	writepm	x5, x5, -84
	lui	x5, 3
	readpm	x5, x5, -84
	lui	x5, 4
	readpm	x5, x5, -84
	lui	x5, 1
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	blt	x5, x6, .LBB0_7
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_6
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_6:
	addi	x11, x0, 4
	lui	x5, 1
	readpm	x7, x5, 0
	readpm	x6, x5, 4
	add	x7, x6, x7
	lui	x5, 3
	writepm	x5, x11, 0
	readpm	x12, x5, 8
	add	x7, x7, x12
	writepm	x5, x7, 4
	lui	x12, 0
	writepm	x5, x12, 16
	addi	x14, x0, 0
	lui	x5, 2
	writepm	x5, x14, 4
	readpm	x14, x5, 12
	readpm	x8, x5, 16
	mul	x8, x14, x8
	writepm	x5, x8, 12
	writepm	x5, x8, 20
	readpm	x6, x5, 20
	slli	x6, x6, 2
	writepm	x5, x6, 16
	readpm	x6, x5, 24
	readpm	x8, x5, 28
	sw	x8, x6, 0
	lui	x5, 1
	readpm	x5, x5, -84
	lui	x5, 1
	writepm	x5, x5, -84
	lui	x5, 3
	writepm	x5, x5, -84
	lui	x5, 4
	writepm	x5, x5, -84
	lui	x5, 3
	readpm	x5, x5, -84
	lui	x5, 4
	readpm	x5, x5, -84
	jal 	.LBB0_5
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_7:
	lui	x5, 1
	readpm	x5, x5, 0
	lw	x6, x5, 0
	lui	x5, 2
	writepm	x5, x6, 0
	lui	x5, 3
	writepm	x5, x5, 8
	lui	x5, 1
	readpm	x5, x5, -84
	lui	x5, 1
	writepm	x5, x5, -84
	lui	x5, 3
	writepm	x5, x5, -84
	lui	x5, 4
	writepm	x5, x5, -84
	lui	x5, 3
	readpm	x5, x5, -84
	lui	x5, 4
	readpm	x5, x5, -84
	jal 	.LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_8:
	lui	x5, 1
	readpm	x5, x5, 0
	lw	x6, x5, 0
	lui	x5, 2
	writepm	x5, x6, 0
	lui	x5, 3
	writepm	x5, x5, 8
	lui	x5, 1
	readpm	x5, x5, -84
	lui	x5, 1
	writepm	x5, x5, -84
	lui	x5, 3
	writepm	x5, x5, -84
	lui	x5, 4
	writepm	x5, x5, -84
	lui	x5, 3
	readpm	x5, x5, -84
	lui	x5, 4
	readpm	x5, x5, -84
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp2
.LBB0_1:
	lui	x5, 2
	readpm	x6, x5, 0
	lw	x6, x6, 0
	lui	x5, 1
	readpm	x5, x5, -100
	lui	x5, 1
	readpm	x5, x5, -116
	lui	x5, 2
	readpm	x5, x5, -100
	lui	x5, 2
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -100
	lui	x5, 2
	writepm	x5, x5, -100
	lui	x5, 4
	writepm	x5, x5, -100
	lui	x5, 3
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -116
	lui	x5, 2
	writepm	x5, x5, -116
	lui	x5, 3
	writepm	x5, x5, -116
	lui	x5, 4
	readpm	x5, x5, -100
	lui	x5, 0
	writepm	x5, x6, 0
	lui	x5, 1
	writepm	x5, x6, 0
	lui	x5, 3
	writepm	x5, x6, 0
	readpm	x5, x5, 0
	blt	x5, x6, .LBB0_9
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_3:
	lui	x5, 2
	readpm	x6, x5, 0
	lw	x6, x6, 0
	lui	x5, 1
	readpm	x5, x5, -100
	lui	x5, 1
	readpm	x5, x5, -116
	lui	x5, 2
	readpm	x5, x5, -100
	lui	x5, 2
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -100
	lui	x5, 2
	writepm	x5, x5, -100
	lui	x5, 4
	writepm	x5, x5, -100
	lui	x5, 3
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -116
	lui	x5, 2
	writepm	x5, x5, -116
	lui	x5, 3
	writepm	x5, x5, -116
	lui	x5, 4
	readpm	x5, x5, -100
	lui	x5, 0
	writepm	x5, x6, 0
	lui	x5, 1
	writepm	x5, x6, 0
	lui	x5, 3
	writepm	x5, x6, 0
	readpm	x5, x5, 0
	blt	x5, x6, .LBB0_8
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_4
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_5:
	lui	x5, 2
	readpm	x6, x5, 0
	lw	x6, x6, 0
	lui	x5, 1
	readpm	x5, x5, -100
	lui	x5, 1
	readpm	x5, x5, -116
	lui	x5, 2
	readpm	x5, x5, -100
	lui	x5, 2
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -100
	lui	x5, 2
	writepm	x5, x5, -100
	lui	x5, 4
	writepm	x5, x5, -100
	lui	x5, 3
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -116
	lui	x5, 2
	writepm	x5, x5, -116
	lui	x5, 3
	writepm	x5, x5, -116
	lui	x5, 4
	readpm	x5, x5, -100
	lui	x5, 0
	writepm	x5, x6, 0
	lui	x5, 1
	writepm	x5, x6, 0
	lui	x5, 3
	writepm	x5, x6, 0
	readpm	x5, x5, 0
	blt	x5, x6, .LBB0_7
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_6
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_6:
	addi	x5, x0, 8
	lw	x6, x5, 0
	lui	x5, 0
	writepm	x5, x6, 0
	lui	x5, 2
	readpm	x11, x5, 0
	slli	x11, x11, 2
	lui	x5, 3
	writepm	x5, x11, 8
	addi	x13, x0, 0
	writepm	x5, x13, 12
	readpm	x14, x5, 4
	readpm	x13, x5, 8
	add	x13, x13, x14
	readpm	x8, x5, 12
	add	x13, x13, x8
	writepm	x5, x11, 20
	lui	x5, 1
	writepm	x5, x6, 20
	readpm	x6, x5, 16
	add	x6, x13, x6
	writepm	x5, x6, 24
	lw	x6, x5, 0
	writepm	x5, x6, 28
	writepm	x5, x5, 32
	writepm	x5, x5, 36
	writepm	x5, x5, 40
	lui	x5, 1
	readpm	x5, x5, -100
	lui	x5, 2
	readpm	x5, x5, -100
	lui	x5, 1
	writepm	x5, x5, -100
	lui	x5, 2
	writepm	x5, x5, -100
	lui	x5, 4
	writepm	x5, x5, -100
	lui	x5, 4
	readpm	x5, x5, -100
	jal 	.LBB0_5
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_7:
	lui	x5, 2
	readpm	x6, x5, 0
	addi	x6, x6, 1
	lui	x5, 3
	writepm	x5, x6, 0
	lui	x5, 1
	readpm	x5, x5, -100
	lui	x5, 2
	readpm	x5, x5, -100
	lui	x5, 1
	writepm	x5, x5, -100
	lui	x5, 2
	writepm	x5, x5, -100
	lui	x5, 4
	writepm	x5, x5, -100
	lui	x5, 4
	readpm	x5, x5, -100
	jal 	.LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_8:
	lui	x5, 2
	readpm	x6, x5, 0
	addi	x6, x6, 1
	lui	x5, 3
	writepm	x5, x6, 0
	lui	x5, 1
	readpm	x5, x5, -100
	lui	x5, 2
	readpm	x5, x5, -100
	lui	x5, 1
	writepm	x5, x5, -100
	lui	x5, 2
	writepm	x5, x5, -100
	lui	x5, 4
	writepm	x5, x5, -100
	lui	x5, 4
	readpm	x5, x5, -100
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp3
.LBB0_1:
	lui	x5, 3
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	blt	x5, x6, .LBB0_9
	addi	x0, x0, 0
	addi	x0, x0, 0
	lui	x5, 2
	writepm	x5, x5, -68
	lui	x5, 3
	writepm	x5, x5, -68
	lui	x5, 4
	writepm	x5, x5, -68
	lui	x5, 1
	readpm	x5, x5, -84
	lui	x5, 1
	readpm	x5, x5, -100
	lui	x5, 1
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -84
	lui	x5, 3
	writepm	x5, x5, -84
	lui	x5, 4
	writepm	x5, x5, -84
	lui	x5, 2
	readpm	x5, x5, -68
	lui	x5, 2
	readpm	x5, x5, -100
	lui	x5, 2
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -100
	lui	x5, 2
	writepm	x5, x5, -100
	lui	x5, 4
	writepm	x5, x5, -100
	lui	x5, 3
	readpm	x5, x5, -68
	lui	x5, 3
	readpm	x5, x5, -84
	lui	x5, 3
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -116
	lui	x5, 2
	writepm	x5, x5, -116
	lui	x5, 3
	writepm	x5, x5, -116
	lui	x5, 4
	readpm	x5, x5, -68
	lui	x5, 4
	readpm	x5, x5, -84
	lui	x5, 4
	readpm	x5, x5, -100
	jal 	.LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_3:
	lui	x5, 3
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	blt	x5, x6, .LBB0_8
	addi	x0, x0, 0
	addi	x0, x0, 0
	lui	x5, 2
	writepm	x5, x5, -68
	lui	x5, 3
	writepm	x5, x5, -68
	lui	x5, 4
	writepm	x5, x5, -68
	lui	x5, 1
	readpm	x5, x5, -84
	lui	x5, 1
	readpm	x5, x5, -100
	lui	x5, 1
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -84
	lui	x5, 3
	writepm	x5, x5, -84
	lui	x5, 4
	writepm	x5, x5, -84
	lui	x5, 2
	readpm	x5, x5, -68
	lui	x5, 2
	readpm	x5, x5, -100
	lui	x5, 2
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -100
	lui	x5, 2
	writepm	x5, x5, -100
	lui	x5, 4
	writepm	x5, x5, -100
	lui	x5, 3
	readpm	x5, x5, -68
	lui	x5, 3
	readpm	x5, x5, -84
	lui	x5, 3
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -116
	lui	x5, 2
	writepm	x5, x5, -116
	lui	x5, 3
	writepm	x5, x5, -116
	lui	x5, 4
	readpm	x5, x5, -68
	lui	x5, 4
	readpm	x5, x5, -84
	lui	x5, 4
	readpm	x5, x5, -100
	jal 	.LBB0_4
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_5:
	lui	x5, 3
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	blt	x5, x6, .LBB0_7
	addi	x0, x0, 0
	addi	x0, x0, 0
	lui	x5, 2
	writepm	x5, x5, -68
	lui	x5, 3
	writepm	x5, x5, -68
	lui	x5, 4
	writepm	x5, x5, -68
	lui	x5, 1
	readpm	x5, x5, -84
	lui	x5, 1
	readpm	x5, x5, -100
	lui	x5, 1
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -84
	lui	x5, 3
	writepm	x5, x5, -84
	lui	x5, 4
	writepm	x5, x5, -84
	lui	x5, 2
	readpm	x5, x5, -68
	lui	x5, 2
	readpm	x5, x5, -100
	lui	x5, 2
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -100
	lui	x5, 2
	writepm	x5, x5, -100
	lui	x5, 4
	writepm	x5, x5, -100
	lui	x5, 3
	readpm	x5, x5, -68
	lui	x5, 3
	readpm	x5, x5, -84
	lui	x5, 3
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -116
	lui	x5, 2
	writepm	x5, x5, -116
	lui	x5, 3
	writepm	x5, x5, -116
	lui	x5, 4
	readpm	x5, x5, -68
	lui	x5, 4
	readpm	x5, x5, -84
	lui	x5, 4
	readpm	x5, x5, -100
	jal 	.LBB0_6
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_6:
	addi	x8, x0, 40
	lui	x5, 3
	readpm	x11, x5, 0
	lw	x11, x11, 0
	lui	x5, 0
	writepm	x5, x8, 4
	lui	x5, 2
	writepm	x5, x11, 0
	readpm	x7, x5, 4
	readpm	x11, x5, 8
	add	x7, x7, x11
	writepm	x5, x7, 8
	readpm	x13, x5, 12
	readpm	x12, x5, 16
	add	x12, x12, x13
	addi	x14, x0, 0
	writepm	x5, x14, 12
	lui	x5, 1
	writepm	x5, x8, 16
	readpm	x8, x5, 20
	add	x8, x12, x8
	writepm	x5, x8, 16
	readpm	x6, x5, 24
	lw	x6, x6, 0
	writepm	x5, x6, 24
	readpm	x6, x5, 28
	addi	x6, x6, 1
	writepm	x5, x6, 28
	lui	x5, 1
	readpm	x5, x5, -116
	lui	x5, 2
	readpm	x5, x5, -116
	lui	x5, 3
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -116
	lui	x5, 2
	writepm	x5, x5, -116
	lui	x5, 3
	writepm	x5, x5, -116
	jal 	.LBB0_5
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_7:
	lui	x5, 3
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	readpm	x5, x5, 8
	sw	x5, x6, 0
	lui	x5, 1
	readpm	x5, x5, -116
	lui	x5, 2
	readpm	x5, x5, -116
	lui	x5, 3
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -116
	lui	x5, 2
	writepm	x5, x5, -116
	lui	x5, 3
	writepm	x5, x5, -116
	jal 	.LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_8:
	lui	x5, 3
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	readpm	x5, x5, 8
	sw	x5, x6, 0
	lui	x5, 1
	readpm	x5, x5, -116
	lui	x5, 2
	readpm	x5, x5, -116
	lui	x5, 3
	readpm	x5, x5, -116
	lui	x5, 1
	writepm	x5, x5, -116
	lui	x5, 2
	writepm	x5, x5, -116
	lui	x5, 3
	writepm	x5, x5, -116
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0
.isStaticHyperOp	1
.isValid	1
.isActive	1
.isIntrinsic	0
.depthHEG	0
.launchCnt	0
.OperandValidity	000000000000000000000000000000000000
.OpWaitCnt	0
.isnextHyOpInstValid	0
.nextHyperOpInst	0
.data	3
i	0
i	16
i	64
