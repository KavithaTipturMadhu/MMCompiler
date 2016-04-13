	.text
	.align	4
	.type	main,@function
.topology	0	0               # @main
HyperOp#0:
.SMD	1	1	0	0
.distcnt	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 8
	addi	x7, x0, 0
	addi	x6, x0, 4
	flw	f0, x7, 0
	flw	f1, x6, 0
	fdiv.s	f0, f0, f1
	fsw	x5, f0, 0
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
.launchCnt	0
.OperandValidity	000000000000000000000000000000000000
.OpWaitCnt	0
.isnextHyOpInstValid	0
.nextHyperOpInst	0
