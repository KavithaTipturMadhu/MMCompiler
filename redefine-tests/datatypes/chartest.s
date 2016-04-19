	.text
	.align	4
	.type	test,@function
.topology	0	0               # @test
HyperOp#0:
.SMD	1	1	0	0
.distcnt	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 5
	addi	x6, x0, 2
	sw	x5, x6, 0
	addi	x6, x0, 4
	addi	x7, x0, 1
	sw	x6, x7, 0
	addi	x6, x0, 0
	lw	x6, x6, 0
	lw	x6, x6, 0
	lw	x5, x5, 0
	bge	x5, x6, .LBB0_2
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
	addi	x6, x0, 5
	lw	x6, x6, 0
	sw	x5, x6, 0
	srli	x31, x31, 6
	fdelete 	x31, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0
.isStaticHyperOp	1
.isValid	1
.isActive	1
.isIntrinsic	0
.depthHEG	0
.launchCnt	1
.OperandValidity	000000000000000000000000000000000001
.OpWaitCnt	1
.isnextHyOpInstValid	0
.nextHyperOpInst	0
