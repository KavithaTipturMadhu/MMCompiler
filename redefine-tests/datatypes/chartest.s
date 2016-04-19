	.text
	.align	4
	.type	entry1,@function
.topology	0	0               # @entry1
HyperOp#0:
.SMD	1	1	0	0
.distcnt	1	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 0
	sw	x5, x1, 0
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
.launchCnt	2
.OperandValidity	000000000000000000000000000000000010
.OpWaitCnt	2
.isnextHyOpInstValid	0
.nextHyperOpInst	0
