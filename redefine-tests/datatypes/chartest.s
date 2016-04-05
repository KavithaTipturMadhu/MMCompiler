	.text
	.align	4
	.type	test,@function
.topology	0	0               # @test
HyperOp#0:
.SMD	1	1	0	0
.distcnt	0	0	0	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 5
	lui	x5, 2
	writepm	x5, x5, 4
	addi	x6, x0, 4
	lui	x5, 1
	writepm	x5, x6, 4
	writepm	x5, x5, 8
	lui	x5, 3
	writepm	x5, x6, 0
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
	readpm	x5, x5, 4
	bge	x5, x6, .LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_1:
	addi	x5, x0, 0
	lui	x5, 3
	writepm	x5, x5, 4
.LBB0_2:
	addi	x5, x0, 0
	lui	x5, 3
	writepm	x5, x5, 4
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp1
.LBB0_0:
	addi	x6, x0, 97
	lui	x5, 2
	writepm	x5, x6, 0
	lui	x5, 1
	readpm	x7, x5, 0
	readpm	x6, x5, 4
	sw	x6, x7, 0
	lui	x5, 3
	writepm	x5, x5, 4
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
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	bge	x5, x6, .LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_1:
	addi	x6, x0, 4
	lui	x5, 2
	writepm	x5, x6, 0
.LBB0_2:
	addi	x6, x0, 5
	lui	x5, 2
	writepm	x5, x6, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp2
.LBB0_0:
	lui	x5, 2
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	sw	x5, x6, 0
	readpm	x5, x5, 8
	lw	x5, x5, 0
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
	readpm	x6, x5, 0
	lui	x5, 0
	writepm	x5, x5, 4
	lui	x5, 1
	writepm	x5, x5, 4
	lui	x5, 3
	writepm	x5, x5, 0
	bge	x5, x6, .LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_1:
	lui	x5, 2
	readpm	x6, x5, 0
	lw	x6, x6, 0
	lui	x5, 3
	writepm	x5, x6, 0
.LBB0_2:
	lui	x5, 2
	readpm	x6, x5, 0
	lw	x6, x6, 0
	lui	x5, 3
	writepm	x5, x6, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp3
.LBB0_0:
	addi	x7, x0, 119
	lui	x5, 1
	writepm	x5, x7, 0
	lui	x5, 3
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	lw	x6, x6, 0
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
	lui	x5, 0
	writepm	x5, x6, 0
	writepm	x5, x6, 0
	lui	x5, 2
	writepm	x5, x6, 0
	readpm	x5, x5, 0
	bge	x5, x6, .LBB0_2
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
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_1:
	lui	x5, 3
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	sw	x5, x6, 0
.LBB0_2:
	lui	x5, 3
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	sw	x5, x6, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0
.isStaticHyperOp	1
.isValid	1
.isActive	1
.isIntrinsic	1
.depthHEG	0
.launchCnt	0
.OperandValidity	000000000000000000000000000000000000
.OpWaitCnt	0
.isnextHyOpInstValid	0
.nextHyperOpInst	0

	.section	".note.GNU-stack","",@progbits
