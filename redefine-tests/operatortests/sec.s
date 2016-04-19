	.text
	.align	4
	.type	sec,@function
.topology	0	0               # @sec
HyperOp#0:
.SMD	1	1	0	0
.distcnt	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 16
	lui	x6, 520192
	srli	x6, x6, 1
	addi	x6, x6, 0
	fmv.s.x	f1, x6
	addi	x6, x0, 12
	addi	x7, x0, 8
	addi	x11, x0, 0
	lui	x10, 522321
	srli	x10, x10, 1
	addi	x10, x10, 1884
	sw	x11, x10, 0
	flw	f2, x11, 0
	fcos.s	f2, f2
	fsw	x7, f2, 0
	addi	x7, x0, 4
	fmv.s.x	f3, x10
	fsin.s	f3, f3
	fsw	x7, f3, 0
	flw	f3, x7, 0
	fdiv.s	f2, f3, f2
	fsw	x6, f2, 0
	fmadd.s	f1, f2, f2, f1
	fsqrt.s	f1, f1
	fsw	x5, f1, 0
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
