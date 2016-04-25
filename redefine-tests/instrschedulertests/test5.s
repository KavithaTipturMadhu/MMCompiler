	.text
	.align	4
	.type	test,@function
.topology	0	0               # @test
HyperOp#0:
.SMD	1	1	0	0
.distcnt	0	0	0	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x31, 4
	lui	x5, 1
	writepm	x5, x5, 0
	lui	x5, 0
	readpm	x5, x5, 0
	readpm	x6, x5, 4
	sw	x6, x5, 0
.LBB0_1:
	addi	x5, x31, 4
	lui	x5, 1
	writepm	x5, x5, 0
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
	readpm	x5, x5, 0
	blt	x0, x5, .LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	addi	x5, x31, 12
	lui	x5, 1
	writepm	x5, x5, 0
	lui	x5, 0
	readpm	x7, x5, 0
	slli	x8, x7, 2
	writepm	x5, x8, 4
	readpm	x13, x5, 4
	slli	x12, x13, 2
	writepm	x5, x12, 8
	lui	x5, 2
	writepm	x5, x8, 0
	readpm	x12, x5, 8
	readpm	x11, x5, 12
	sw	x12, x11, 0
	writepm	x5, x5, 20
	lui	x5, 3
	writepm	x5, x5, 24
	readpm	x5, x5, 16
	readpm	x5, x5, 20
	lw	x6, x5, 0
	writepm	x5, x6, 24
	writepm	x5, x5, 16
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
	lui	x5, 1
	readpm	x5, x5, 0
	lw	x5, x5, 0
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
	readpm	x5, x5, -84
	lui	x5, 4
	readpm	x5, x5, -100
	lui	x5, 0
	writepm	x5, x5, 0
	lui	x5, 2
	writepm	x5, x5, 0
	lui	x5, 3
	writepm	x5, x5, 0
	blt	x0, x5, .LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	lui	x5, 1
	readpm	x5, x5, 0
	lw	x6, x5, 0
	readpm	x8, x5, 4
	add	x6, x6, x8
	readpm	x12, x5, 8
	readpm	x11, x5, 12
	add	x12, x11, x12
	lui	x5, 0
	writepm	x5, x12, 8
	readpm	x8, x5, 16
	readpm	x5, x5, 20
	lw	x8, x8, 0
	lui	x5, 2
	writepm	x5, x8, 4
	lui	x5, 3
	writepm	x5, x6, 12
	writepm	x5, x5, 16
	writepm	x5, x5, 20
	readpm	x6, x5, 24
	addi	x6, x6, 1
	writepm	x5, x6, 12
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
.LBB0_0:
	addi	x5, x31, 8
	lui	x5, 0
	writepm	x5, x5, 0
.LBB0_1:
	lui	x5, 2
	readpm	x5, x5, 0
	blt	x0, x5, .LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	addi	x5, x31, 4
	lui	x5, 3
	writepm	x5, x5, 0
	addi	x11, x31, 0
	writepm	x5, x5, 4
	lui	x5, 1
	writepm	x5, x11, 12
	lui	x5, 2
	readpm	x8, x5, 0
	add	x8, x11, x8
	writepm	x5, x8, 16
	readpm	x8, x5, 4
	readpm	x7, x5, 8
	add	x7, x8, x7
	writepm	x5, x7, 8
	lui	x5, 0
	writepm	x5, x5, 16
	readpm	x6, x5, 12
	readpm	x5, x5, 16
	readpm	x5, x5, 20
	sw	x5, x6, 0
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
.LBB0_0:
	addi	x6, x31, 12
	lui	x5, 0
	writepm	x5, x6, 4
.LBB0_1:
	lui	x5, 3
	readpm	x5, x5, 0
	blt	x0, x5, .LBB0_3
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
	jal 	.LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	lui	x5, 3
	readpm	x5, x5, 0
	lw	x7, x5, 0
	lui	x5, 0
	writepm	x5, x7, 0
	readpm	x5, x5, 4
	lw	x13, x5, 0
	writepm	x5, x13, 4
	mul	x11, x13, x13
	writepm	x5, x11, 12
	lui	x5, 2
	writepm	x5, x7, 8
	readpm	x7, x5, 8
	readpm	x6, x5, 12
	readpm	x5, x5, 16
	readpm	x5, x5, 20
	readpm	x5, x5, 24
	sw	x6, x7, 0
	writepm	x5, x5, 20
	writepm	x5, x5, 20
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
.data	0
