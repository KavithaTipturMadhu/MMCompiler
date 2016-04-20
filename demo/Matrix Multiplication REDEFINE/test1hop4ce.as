	.text
	.align	4
	.type	main,@function
.topology	2	2               # @main
HyperOp#0:
.SMD	1	0	0	0
.distcnt	0	0	0	0	
.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 3
	lui	x5, 2
	writepm	x5, x5, 0
	lui	x5, 0
	readpm	x5, x5, 0
	sw	x5, x0, 0
	addi	x5, x0, 1
	writepm	x5, x5, 8
	addi	x6, x0, 0
	addi	x7, x0, 0
	writecm x7, x6, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0
	
.code	;pHyperOp1
.LBB0_0:
	addi	x6, x0, 12
	lui	x5, 2
	writepm	x5, x6, 4
	addi	x5, x0, 2
	lui	x5, 3
	writepm	x5, x5, 0
	addi	x6, x0, 4
	writepm	x5, x6, 12
	addi	x6, x0, 1
	addi	x7, x0, 104
	writecm x7, x6, 0	
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp2
.LBB0_0:
	lui	x5, 2
	readpm	x5, x5, 0
	readpm	x6, x5, 4
	sw	x6, x5, 0
	addi	x6, x0, 8
	lui	x5, 3
	writepm	x5, x6, 4
	readpm	x5, x5, 8
	readpm	x6, x5, 12
	sw	x6, x5, 0
	addi	x6, x0, 2
	addi	x7, x0, 156
	writecm x7, x6, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp3
.LBB0_0:
	addi	x5, x0, 0
	lui	x5, 0
	writepm	x5, x5, 0
	lui	x5, 3
	readpm	x5, x5, 0
	readpm	x6, x5, 4
	sw	x6, x5, 0
	srli	x0, x0, 6
	fdelete 	x0, 0
	addi	x6, x0, 3
	addi	x7, x0, 208
	writecm x7, x6, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

	.text
	.align	4
	.type	main0,@function
HyperOp#1:
.SMD	0	0	0	0
.distcnt	1	0	0	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 8
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_1:
	addi	x5, x0, 3
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
	addi	x5, x0, 0
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_3:
	addi	x5, x0, 3
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
	addi	x5, x0, 4
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_5:
	addi	x5, x0, 3
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
	addi	x5, x0, 0
	lui	x5, 2
	writepm	x5, x5, 0
	addi	x5, x0, 4
	writepm	x5, x5, 8
	lui	x5, 0
	readpm	x11, x5, 0
	readpm	x7, x5, 4
	add	x7, x7, x11
	writepm	x5, x7, 12
	addi	x11, x0, 0
	writepm	x5, x11, 20
	addi	x13, x0, 0
	lui	x5, 3
	writepm	x5, x13, 8
	readpm	x13, x5, 8
	slli	x13, x13, 4
	lui	x5, 1
	writepm	x5, x13, 8
	writepm	x5, x13, 36
	readpm	x8, x5, 12
	lw	x11, x8, 0
	readpm	x7, x5, 16
	readpm	x6, x5, 20
	mul	x6, x6, x7
	writepm	x5, x6, 16
	writepm	x5, x11, 20
	writepm	x5, x5, 56
	writepm	x5, x5, 24
	readpm	x6, x5, 24
	addi	x6, x6, 1
	writepm	x5, x6, 24
	writepm	x5, x5, 28
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
	jal 	.LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_8:
	addi	x5, x0, 8
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

.code	;pHyperOp1
.LBB0_0:
	lui	x5, 1
	readpm	x5, x5, 0
	sw	x5, x0, 0
.LBB0_1:
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
	blt	x5, x6, .LBB0_7
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_6
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_6:
	lui	x6, 0
	lui	x5, 2
	writepm	x5, x6, 4
	lui	x5, 1
	readpm	x6, x5, 0
	lw	x6, x6, 0
	lui	x5, 3
	writepm	x5, x6, 0
	readpm	x8, x5, 4
	slli	x8, x8, 2
	writepm	x5, x8, 16
	lui	x12, 0
	writepm	x5, x12, 24
	addi	x14, x0, 0
	writepm	x5, x14, 28
	readpm	x13, x5, 8
	readpm	x12, x5, 12
	add	x12, x12, x13
	writepm	x5, x8, 16
	slli	x6, x6, 2
	writepm	x5, x12, 40
	writepm	x5, x6, 44
	readpm	x6, x5, 16
	readpm	x11, x5, 20
	add	x6, x11, x6
	writepm	x5, x6, 48
	readpm	x6, x5, 24
	readpm	x5, x5, 28
	readpm	x5, x5, 32
	readpm	x5, x5, 36
	sw	x5, x6, 0
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
	lui	x5, 2
	readpm	x5, x5, 0
	readpm	x6, x5, 4
	add	x7, x6, x5
	readpm	x5, x5, 8
	lw	x8, x5, 0
	lui	x5, 0
	writepm	x5, x7, 4
	lui	x5, 1
	writepm	x5, x8, 4
	readpm	x7, x5, 12
	readpm	x8, x5, 16
	add	x7, x7, x8
	lui	x5, 3
	writepm	x5, x7, 4
	readpm	x11, x5, 20
	readpm	x12, x5, 24
	add	x11, x12, x11
	readpm	x14, x5, 28
	readpm	x12, x5, 32
	add	x12, x12, x14
	writepm	x5, x12, 12
	readpm	x13, x5, 36
	add	x11, x11, x13
	writepm	x5, x11, 12
	readpm	x12, x5, 40
	readpm	x6, x5, 44
	add	x6, x12, x6
	writepm	x5, x6, 20
	readpm	x6, x5, 48
	readpm	x8, x5, 52
	readpm	x5, x5, 56
	sw	x8, x6, 0
	writepm	x5, x5, 36
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
	addi	x6, x0, 8
	lui	x5, 1
	writepm	x5, x6, 0
	lui	x5, 3
	readpm	x6, x5, 0
	slli	x11, x6, 4
	lui	x5, 0
	writepm	x5, x11, 0
	readpm	x7, x5, 4
	lw	x7, x7, 0
	lui	x12, 0
	lui	x5, 2
	writepm	x5, x12, 32
	readpm	x13, x5, 8
	lw	x13, x13, 0
	writepm	x5, x13, 8
	readpm	x11, x5, 12
	readpm	x8, x5, 16
	add	x8, x11, x8
	writepm	x5, x8, 12
	readpm	x6, x5, 20
	lw	x6, x6, 0
	writepm	x5, x7, 16
	writepm	x5, x6, 20
	writepm	x5, x8, 52
	readpm	x5, x5, 24
	lw	x6, x5, 0
	writepm	x5, x6, 24
	writepm	x5, x5, 32
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

	.text
	.align	4
	.type	main1,@function
HyperOp#2:
.SMD	0	0	0	0
.distcnt	1	0	0	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 8
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_1:
	addi	x5, x0, 3
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
	addi	x5, x0, 0
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_3:
	addi	x5, x0, 3
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
	addi	x5, x0, 4
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_5:
	addi	x5, x0, 3
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
	addi	x5, x0, 0
	lui	x5, 2
	writepm	x5, x5, 0
	addi	x5, x0, 4
	writepm	x5, x5, 8
	lui	x5, 0
	readpm	x11, x5, 0
	readpm	x7, x5, 4
	add	x7, x7, x11
	writepm	x5, x7, 12
	addi	x11, x0, 0
	writepm	x5, x11, 20
	addi	x13, x0, 0
	lui	x5, 3
	writepm	x5, x13, 8
	readpm	x13, x5, 8
	slli	x13, x13, 4
	lui	x5, 1
	writepm	x5, x13, 8
	writepm	x5, x13, 36
	readpm	x8, x5, 12
	lw	x11, x8, 0
	readpm	x7, x5, 16
	readpm	x6, x5, 20
	mul	x6, x6, x7
	writepm	x5, x6, 16
	writepm	x5, x11, 20
	writepm	x5, x5, 56
	writepm	x5, x5, 24
	readpm	x6, x5, 24
	addi	x6, x6, 1
	writepm	x5, x6, 24
	writepm	x5, x5, 28
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
	jal 	.LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_8:
	addi	x5, x0, 8
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
	blt	x5, x6, .LBB0_7
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_6
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_6:
	lui	x6, 0
	lui	x5, 2
	writepm	x5, x6, 4
	lui	x5, 1
	readpm	x6, x5, 0
	lw	x6, x6, 0
	lui	x5, 3
	writepm	x5, x6, 0
	readpm	x8, x5, 4
	slli	x8, x8, 2
	writepm	x5, x8, 16
	lui	x12, 0
	writepm	x5, x12, 24
	addi	x14, x0, 0
	writepm	x5, x14, 28
	readpm	x13, x5, 8
	readpm	x12, x5, 12
	add	x12, x12, x13
	writepm	x5, x8, 16
	slli	x6, x6, 2
	writepm	x5, x12, 40
	writepm	x5, x6, 44
	readpm	x6, x5, 16
	readpm	x11, x5, 20
	add	x6, x11, x6
	writepm	x5, x6, 48
	readpm	x6, x5, 24
	readpm	x5, x5, 28
	readpm	x5, x5, 32
	readpm	x5, x5, 36
	sw	x5, x6, 0
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
	lui	x5, 2
	readpm	x5, x5, 0
	readpm	x6, x5, 4
	add	x7, x6, x5
	readpm	x5, x5, 8
	lw	x8, x5, 0
	lui	x5, 0
	writepm	x5, x7, 4
	lui	x5, 1
	writepm	x5, x8, 4
	readpm	x7, x5, 12
	readpm	x8, x5, 16
	add	x7, x7, x8
	lui	x5, 3
	writepm	x5, x7, 4
	readpm	x11, x5, 20
	readpm	x12, x5, 24
	add	x11, x12, x11
	readpm	x14, x5, 28
	readpm	x12, x5, 32
	add	x12, x12, x14
	writepm	x5, x12, 12
	readpm	x13, x5, 36
	add	x11, x11, x13
	writepm	x5, x11, 12
	readpm	x12, x5, 40
	readpm	x6, x5, 44
	add	x6, x12, x6
	writepm	x5, x6, 20
	readpm	x6, x5, 48
	readpm	x8, x5, 52
	readpm	x5, x5, 56
	sw	x8, x6, 0
	writepm	x5, x5, 36
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
	addi	x6, x0, 8
	lui	x5, 1
	writepm	x5, x6, 0
	lui	x5, 3
	readpm	x6, x5, 0
	slli	x11, x6, 4
	lui	x5, 0
	writepm	x5, x11, 0
	readpm	x7, x5, 4
	lw	x7, x7, 0
	lui	x12, 0
	lui	x5, 2
	writepm	x5, x12, 32
	readpm	x13, x5, 8
	lw	x13, x13, 0
	writepm	x5, x13, 8
	readpm	x11, x5, 12
	readpm	x8, x5, 16
	add	x8, x11, x8
	writepm	x5, x8, 12
	readpm	x6, x5, 20
	lw	x6, x6, 0
	writepm	x5, x7, 16
	writepm	x5, x6, 20
	writepm	x5, x8, 52
	readpm	x5, x5, 24
	lw	x6, x5, 0
	writepm	x5, x6, 24
	writepm	x5, x5, 32
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
	
	.text
	.align	4
	.type	main2,@function
HyperOp#3:
.SMD	0	0	0	0
.distcnt	1	0	0	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 8
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_1:
	addi	x5, x0, 3
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
	addi	x5, x0, 0
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_3:
	addi	x5, x0, 3
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
	addi	x5, x0, 4
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_5:
	addi	x5, x0, 3
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
	addi	x5, x0, 0
	lui	x5, 2
	writepm	x5, x5, 0
	addi	x5, x0, 4
	writepm	x5, x5, 8
	lui	x5, 0
	readpm	x11, x5, 0
	readpm	x7, x5, 4
	add	x7, x7, x11
	writepm	x5, x7, 12
	addi	x11, x0, 0
	writepm	x5, x11, 20
	addi	x13, x0, 0
	lui	x5, 3
	writepm	x5, x13, 8
	readpm	x13, x5, 8
	slli	x13, x13, 4
	lui	x5, 1
	writepm	x5, x13, 8
	writepm	x5, x13, 36
	readpm	x8, x5, 12
	lw	x11, x8, 0
	readpm	x7, x5, 16
	readpm	x6, x5, 20
	mul	x6, x6, x7
	writepm	x5, x6, 16
	writepm	x5, x11, 20
	writepm	x5, x5, 56
	writepm	x5, x5, 24
	readpm	x6, x5, 24
	addi	x6, x6, 1
	writepm	x5, x6, 24
	writepm	x5, x5, 28
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
	jal 	.LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_8:
	addi	x5, x0, 8
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
	blt	x5, x6, .LBB0_7
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_6
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_6:
	lui	x6, 0
	lui	x5, 2
	writepm	x5, x6, 4
	lui	x5, 1
	readpm	x6, x5, 0
	lw	x6, x6, 0
	lui	x5, 3
	writepm	x5, x6, 0
	readpm	x8, x5, 4
	slli	x8, x8, 2
	writepm	x5, x8, 16
	lui	x12, 0
	writepm	x5, x12, 24
	addi	x14, x0, 0
	writepm	x5, x14, 28
	readpm	x13, x5, 8
	readpm	x12, x5, 12
	add	x12, x12, x13
	writepm	x5, x8, 16
	slli	x6, x6, 2
	writepm	x5, x12, 40
	writepm	x5, x6, 44
	readpm	x6, x5, 16
	readpm	x11, x5, 20
	add	x6, x11, x6
	writepm	x5, x6, 48
	readpm	x6, x5, 24
	readpm	x5, x5, 28
	readpm	x5, x5, 32
	readpm	x5, x5, 36
	sw	x5, x6, 0
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
	lui	x5, 2
	readpm	x5, x5, 0
	readpm	x6, x5, 4
	add	x7, x6, x5
	readpm	x5, x5, 8
	lw	x8, x5, 0
	lui	x5, 0
	writepm	x5, x7, 4
	lui	x5, 1
	writepm	x5, x8, 4
	readpm	x7, x5, 12
	readpm	x8, x5, 16
	add	x7, x7, x8
	lui	x5, 3
	writepm	x5, x7, 4
	readpm	x11, x5, 20
	readpm	x12, x5, 24
	add	x11, x12, x11
	readpm	x14, x5, 28
	readpm	x12, x5, 32
	add	x12, x12, x14
	writepm	x5, x12, 12
	readpm	x13, x5, 36
	add	x11, x11, x13
	writepm	x5, x11, 12
	readpm	x12, x5, 40
	readpm	x6, x5, 44
	add	x6, x12, x6
	writepm	x5, x6, 20
	readpm	x6, x5, 48
	readpm	x8, x5, 52
	readpm	x5, x5, 56
	sw	x8, x6, 0
	writepm	x5, x5, 36
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
	addi	x6, x0, 8
	lui	x5, 1
	writepm	x5, x6, 0
	lui	x5, 3
	readpm	x6, x5, 0
	slli	x11, x6, 4
	lui	x5, 0
	writepm	x5, x11, 0
	readpm	x7, x5, 4
	lw	x7, x7, 0
	lui	x12, 0
	lui	x5, 2
	writepm	x5, x12, 32
	readpm	x13, x5, 8
	lw	x13, x13, 0
	writepm	x5, x13, 8
	readpm	x11, x5, 12
	readpm	x8, x5, 16
	add	x8, x11, x8
	writepm	x5, x8, 12
	readpm	x6, x5, 20
	lw	x6, x6, 0
	writepm	x5, x7, 16
	writepm	x5, x6, 20
	writepm	x5, x8, 52
	readpm	x5, x5, 24
	lw	x6, x5, 0
	writepm	x5, x6, 24
	writepm	x5, x5, 32
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

	.text
	.align	4
	.type	main2,@function
HyperOp#4:
.SMD	0	0	0	0
.distcnt	1	0	0	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 8
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_1:
	addi	x5, x0, 3
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
	addi	x5, x0, 0
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_3:
	addi	x5, x0, 3
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
	addi	x5, x0, 4
	lui	x5, 1
	writepm	x5, x5, 0
.LBB0_5:
	addi	x5, x0, 3
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
	addi	x5, x0, 0
	lui	x5, 2
	writepm	x5, x5, 0
	addi	x5, x0, 4
	writepm	x5, x5, 8
	lui	x5, 0
	readpm	x11, x5, 0
	readpm	x7, x5, 4
	add	x7, x7, x11
	writepm	x5, x7, 12
	addi	x11, x0, 0
	writepm	x5, x11, 20
	addi	x13, x0, 0
	lui	x5, 3
	writepm	x5, x13, 8
	readpm	x13, x5, 8
	slli	x13, x13, 4
	lui	x5, 1
	writepm	x5, x13, 8
	writepm	x5, x13, 36
	readpm	x8, x5, 12
	lw	x11, x8, 0
	readpm	x7, x5, 16
	readpm	x6, x5, 20
	mul	x6, x6, x7
	writepm	x5, x6, 16
	writepm	x5, x11, 20
	writepm	x5, x5, 56
	writepm	x5, x5, 24
	readpm	x6, x5, 24
	addi	x6, x6, 1
	writepm	x5, x6, 24
	writepm	x5, x5, 28
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
	jal 	.LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_8:
	addi	x5, x0, 8
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

.code	;pHyperOp1
.LBB0_0:
	lui	x5, 1
	readpm	x5, x5, 0
	sw	x5, x0, 0
.LBB0_1:
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
	blt	x5, x6, .LBB0_7
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_6
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_6:
	lui	x6, 0
	lui	x5, 2
	writepm	x5, x6, 4
	lui	x5, 1
	readpm	x6, x5, 0
	lw	x6, x6, 0
	lui	x5, 3
	writepm	x5, x6, 0
	readpm	x8, x5, 4
	slli	x8, x8, 2
	writepm	x5, x8, 16
	lui	x12, 0
	writepm	x5, x12, 24
	addi	x14, x0, 0
	writepm	x5, x14, 28
	readpm	x13, x5, 8
	readpm	x12, x5, 12
	add	x12, x12, x13
	writepm	x5, x8, 16
	slli	x6, x6, 2
	writepm	x5, x12, 40
	writepm	x5, x6, 44
	readpm	x6, x5, 16
	readpm	x11, x5, 20
	add	x6, x11, x6
	writepm	x5, x6, 48
	readpm	x6, x5, 24
	readpm	x5, x5, 28
	readpm	x5, x5, 32
	readpm	x5, x5, 36
	sw	x5, x6, 0
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
	lui	x5, 2
	readpm	x5, x5, 0
	readpm	x6, x5, 4
	add	x7, x6, x5
	readpm	x5, x5, 8
	lw	x8, x5, 0
	lui	x5, 0
	writepm	x5, x7, 4
	lui	x5, 1
	writepm	x5, x8, 4
	readpm	x7, x5, 12
	readpm	x8, x5, 16
	add	x7, x7, x8
	lui	x5, 3
	writepm	x5, x7, 4
	readpm	x11, x5, 20
	readpm	x12, x5, 24
	add	x11, x12, x11
	readpm	x14, x5, 28
	readpm	x12, x5, 32
	add	x12, x12, x14
	writepm	x5, x12, 12
	readpm	x13, x5, 36
	add	x11, x11, x13
	writepm	x5, x11, 12
	readpm	x12, x5, 40
	readpm	x6, x5, 44
	add	x6, x12, x6
	writepm	x5, x6, 20
	readpm	x6, x5, 48
	readpm	x8, x5, 52
	readpm	x5, x5, 56
	sw	x8, x6, 0
	writepm	x5, x5, 36
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
	addi	x6, x0, 8
	lui	x5, 1
	writepm	x5, x6, 0
	lui	x5, 3
	readpm	x6, x5, 0
	slli	x11, x6, 4
	lui	x5, 0
	writepm	x5, x11, 0
	readpm	x7, x5, 4
	lw	x7, x7, 0
	lui	x12, 0
	lui	x5, 2
	writepm	x5, x12, 32
	readpm	x13, x5, 8
	lw	x13, x13, 0
	writepm	x5, x13, 8
	readpm	x11, x5, 12
	readpm	x8, x5, 16
	add	x8, x11, x8
	writepm	x5, x8, 12
	readpm	x6, x5, 20
	lw	x6, x6, 0
	writepm	x5, x7, 16
	writepm	x5, x6, 20
	writepm	x5, x8, 52
	readpm	x5, x5, 24
	lw	x6, x5, 0
	writepm	x5, x6, 24
	writepm	x5, x5, 32
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

HyperOp#5:
.code	;pHyperOp0
.LBB0_0:
	srli	x0, x0, 6
	fdelete 	x0, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0
.code	;pHyperOp1
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp2
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp3
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0
.data	3
i	0
i	64
i	128
