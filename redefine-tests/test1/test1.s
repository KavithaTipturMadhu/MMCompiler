	.text
	.align	4
	.type	test,@function
.topology	0	0               # @test
HyperOp#0:
.SMD	1	1	0	0
.distcnt	5	

.code	;pHyperOp0
.LBB0_0:
	addi	x6, x0, 4
	sw	x6, x1, 0
	addi	x7, x0, 8
	sw	x7, x2, 0
	addi	x7, x0, 16
	sw	x7, x3, 0
	addi	x5, x0, 20
	lw	x6, x6, 0
	addi	x7, x0, 0
	sw	x7, x4, 0
	lw	x7, x7, 0
	add	x6, x7, x6
	sw	x5, x6, 0
	addi	x6, x0, 12
	sw	x6, x5, 0
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
.launchCnt	5
.OperandValidity	000000000000000000000000000000000101
.OpWaitCnt	5
.isnextHyOpInstValid	0
.nextHyperOpInst	0
