	.text
	.align	4
	.type	test,@function
.topology	0	0               # @test
HyperOp#0:
.SMD	1	1	0	0
.distcnt	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 8
	addi	x6, x0, 2047
	addi	x6, x6, 2047
	addi	x6, x6, 2047
	addi	x6, x6, 2047
	addi	x6, x6, 2047
	addi	x6, x6, 2047
	addi	x6, x6, 2047
	addi	x6, x6, 2047
	addi	x6, x6, 2047
	addi	x6, x0, 1577
	sw	x5, x6, 0
	addi	x6, x0, 4
	addi	x5, x0, 2047
	addi	x5, x5, 2047
	addi	x5, x5, 2047
	addi	x5, x5, 2047
	addi	x5, x0, 1812
	sw	x6, x5, 0
	addi	x5, x0, 2047
	addi	x5, x5, 2047
	addi	x5, x5, 2047
	addi	x5, x5, 2047
	addi	x5, x5, 2047
	addi	x5, x5, 2047
	addi	x5, x5, 2047
	addi	x5, x5, 2047
	addi	x5, x5, 2047
	addi	x5, x0, 1578
	lw	x6, x6, 0
	blt	x6, x5, .LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_1:
	addi	x5, x0, 0
	addi	x6, x0, 4
	lw	x6, x6, 0
	sw	x5, x6, 0
.LBB0_2:
	addi	x5, x0, 0
	addi	x6, x0, 8
	lw	x6, x6, 0
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
