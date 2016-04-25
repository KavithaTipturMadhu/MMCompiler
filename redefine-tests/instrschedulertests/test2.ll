	.text
	.align	4
	.type	test,@function
.topology	0	0               # @test
HyperOp#0:
.SMD	1	1	0	0
.distcnt	1	0	3	1	

.code	;pHyperOp0
.LBB0_0:
	addi	x6, x0, 28
	lui	x5, 0
	readpm	x5, x5, 0
	readpm	x8, x5, 4
	sw	x8, x5, 0
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
	readpm	x8, x5, 24
	readpm	x11, x5, 28
	sw	x11, x8, 0
	readpm	x11, x5, 32
	readpm	x5, x5, 36
	lw	x11, x11, 0
	writepm	x5, x11, 12
	readpm	x7, x5, 40
	mul	x5, x5, x7
	writepm	x5, x5, 16
	writepm	x5, x6, 24
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp1
.LBB0_0:
	addi	x11, x0, 24
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
	readpm	x8, x5, 4
	sw	x8, x1, 0
	lui	x5, 2
	writepm	x5, x5, 8
	writepm	x5, x11, 28
	addi	x11, x0, 0
	writepm	x5, x11, 28
	writepm	x5, x11, 32
	readpm	x12, x5, 8
	readpm	x11, x5, 12
	add	x11, x11, x12
	writepm	x5, x11, 12
	writepm	x5, x7, 16
	writepm	x5, x7, 32
	readpm	x5, x5, 16
	readpm	x8, x5, 20
	add	x5, x5, x8
	writepm	x5, x5, 20
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp2
.LBB0_0:
	addi	x8, x0, 4
	lui	x5, 0
	writepm	x5, x8, 4
	lui	x5, 3
	writepm	x5, x2, 0
	writepm	x5, x3, 8
	lui	x5, 2
	readpm	x5, x5, 0
	lw	x12, x8, 0
	readpm	x8, x5, 4
	readpm	x5, x5, 8
	lw	x8, x8, 0
	writepm	x5, x8, 16
	writepm	x5, x4, 24
	lui	x5, 1
	writepm	x5, x12, 8
	readpm	x11, x5, 12
	readpm	x7, x5, 16
	sw	x7, x11, 0
	writepm	x5, x5, 36
	readpm	x5, x5, 20
	readpm	x6, x5, 24
	sw	x6, x5, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp3
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
	addi	x8, x0, 12
	lui	x5, 1
	writepm	x5, x8, 4
	lui	x5, 2
	writepm	x5, x8, 4
	readpm	x8, x5, 16
	readpm	x13, x5, 20
	add	x8, x13, x8
	writepm	x5, x8, 24
	readpm	x4, x5, 24
	readpm	x11, x5, 28
	sw	x11, x4, 0
	writepm	x5, x5, 36
	readpm	x7, x5, 32
	readpm	x5, x5, 36
	lw	x7, x7, 0
	writepm	x5, x7, 40
	writepm	x5, x8, 20
	srli	x0, x0, 6
	fdelete 	x0, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0
.isStaticHyperOp	1
.isValid	1
.isActive	1
.isIntrinsic	0
.depthHEG	0
.launchCnt	5
.OperandValidity	000000000000000000000000000000000101
.OpWaitCnt	5
.isnextHyOpInstValid	0
.nextHyperOpInst	0
.data	0
