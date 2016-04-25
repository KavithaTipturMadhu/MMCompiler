	.text
	.align	4
	.type	main,@function
.topology	0	0               # @main
HyperOp#0:
.SMD	1	1	0	0
.distcnt	0	0	0	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x31, 160
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
	blt	x5, x6, .LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	addi	x7, x0, 18
	addi	x5, x0, 0
	lui	x5, 2
	writepm	x5, x5, 0
	lui	x5, 0
	readpm	x5, x5, 0
	lw	x11, x5, 0
	writepm	x5, x11, 8
	readpm	x13, x5, 4
	slli	x14, x13, 3
	lui	x5, 1
	writepm	x5, x14, 4
	writepm	x5, x7, 16
	lui	x5, 3
	writepm	x5, x11, 12
	readpm	x12, x5, 8
	readpm	x11, x5, 12
	sw	x12, x11, 0
	writepm	x5, x5, 16
	readpm	x8, x5, 16
	readpm	x6, x5, 20
	add	x8, x6, x8
	writepm	x5, x8, 20
	readpm	x8, x5, 24
	readpm	x6, x5, 28
	add	x6, x6, x8
	writepm	x5, x6, 28
	readpm	x6, x5, 32
	addi	x6, x6, 1
	writepm	x5, x6, 32
	writepm	x5, x5, 44
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
.LBB0_3:
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
	addi	x6, x31, 160
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
	blt	x5, x6, .LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	lui	x5, 80
	lui	x5, 3
	writepm	x5, x5, 4
	lui	x6, 0
	lui	x5, 2
	writepm	x5, x6, 4
	lui	x5, 1
	readpm	x5, x5, 0
	lw	x13, x5, 0
	lui	x5, 0
	writepm	x5, x13, 4
	readpm	x14, x5, 4
	readpm	x8, x5, 8
	add	x8, x8, x14
	writepm	x5, x13, 12
	readpm	x7, x5, 12
	readpm	x5, x5, 16
	sw	x8, x7, 4
	readpm	x8, x5, 20
	readpm	x7, x5, 24
	sw	x8, x7, 0
	readpm	x7, x5, 28
	addi	x7, x7, 10
	writepm	x5, x7, 32
	writepm	x5, x5, 36
	readpm	x6, x5, 32
	readpm	x5, x5, 36
	readpm	x5, x5, 40
	readpm	x5, x5, 44
	readpm	x5, x5, 48
	readpm	x5, x5, 52
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
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_3:
	end 	0
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
	blt	x5, x6, .LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	addi	x6, x0, 80
	lui	x5, 3
	writepm	x5, x6, 0
	lui	x5, 2
	readpm	x5, x5, 0
	readpm	x6, x5, 4
	add	x6, x6, x5
	readpm	x11, x5, 8
	slli	x12, x11, 3
	writepm	x5, x12, 8
	readpm	x13, x5, 12
	readpm	x7, x5, 16
	mul	x7, x13, x7
	lui	x5, 1
	writepm	x5, x7, 12
	readpm	x5, x5, 20
	lw	x7, x5, 0
	writepm	x5, x7, 16
	lui	x5, 0
	writepm	x5, x6, 20
	writepm	x5, x7, 24
	readpm	x5, x5, 24
	lw	x7, x5, 0
	writepm	x5, x7, 20
	writepm	x5, x6, 28
	writepm	x5, x7, 28
	readpm	x6, x5, 28
	readpm	x7, x5, 32
	readpm	x5, x5, 36
	sw	x6, x7, 4
	writepm	x5, x5, 48
	writepm	x5, x5, 52
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
.LBB0_3:
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp3
.LBB0_1:
	lui	x5, 3
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	blt	x5, x6, .LBB0_3
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
	jal 	.LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	lui	x5, 3
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	add	x8, x5, x6
	addi	x5, x31, 160
	lui	x5, 0
	writepm	x5, x5, 0
	lui	x5, 1
	writepm	x5, x5, 0
	readpm	x12, x5, 8
	add	x12, x8, x12
	writepm	x5, x8, 8
	readpm	x11, x5, 12
	addi	x11, x11, 1
	writepm	x5, x12, 8
	writepm	x5, x11, 12
	lui	x5, 2
	writepm	x5, x5, 20
	readpm	x7, x5, 16
	slli	x8, x7, 3
	writepm	x5, x8, 16
	writepm	x5, x5, 24
	readpm	x7, x5, 20
	slli	x8, x7, 3
	writepm	x5, x8, 24
	lw	x6, x5, 0
	writepm	x5, x6, 32
	writepm	x5, x5, 36
	writepm	x5, x5, 40
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
.LBB0_3:
	end 	0
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
.data	2
i	0
i	80
