	.text
	.align	4
	.type	sec,@function
.topology	0	0               # @sec
HyperOp#0:
.SMD	1	1	0	0
.distcnt	0	0	0	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 16
	addi	x10, x0, 8
	lui	x5, 0
	readpm	x11, x5, 0
	readpm	x5, x5, 4
	flw	f2, x11, 0
	lui	x5, 1
	fwritepm	x5, f2, 0
	lui	x5, 2
	writepm	x5, x10, 8
	readpm	x7, x5, 8
	fmv.s.x	f3, x7
	fwritepm	x5, f3, 4
	freadpm	f2, x5, 12
	freadpm	f3, x5, 16
	fdiv.s	f2, f3, f2
	fwritepm	x5, f2, 8
	fwritepm	x5, f2, 20
	freadpm	f1, x5, 20
	fsw	x5, f1, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp1
.LBB0_0:
	lui	x6, 520192
	srli	x6, x6, 1
	addi	x6, x6, 0
	lui	x5, 2
	writepm	x5, x6, 0
	addi	x11, x0, 0
	lui	x5, 3
	writepm	x5, x11, 4
	lui	x5, 0
	writepm	x5, x11, 0
	lui	x5, 1
	freadpm	f2, x5, 0
	fcos.s	f2, f2
	fwritepm	x5, f2, 4
	freadpm	f3, x5, 4
	fsin.s	f3, f3
	fwritepm	x5, f3, 12
	fwritepm	x5, f2, 12
	freadpm	f2, x5, 8
	readpm	x6, x5, 12
	fsw	x6, f2, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp2
.LBB0_0:
	lui	x5, 2
	readpm	x6, x5, 0
	fmv.s.x	f1, x6
	lui	x7, 522321
	srli	x7, x7, 1
	addi	x7, x7, 1884
	lui	x5, 3
	writepm	x5, x7, 0
	freadpm	f2, x5, 4
	readpm	x10, x5, 8
	fsw	x10, f2, 0
	lui	x5, 0
	writepm	x5, x7, 8
	freadpm	f3, x5, 12
	readpm	x10, x5, 16
	fsw	x10, f3, 0
	writepm	x5, x5, 8
	freadpm	f2, x5, 20
	fmadd.s	f1, f2, f2, f1
	fwritepm	x5, f1, 12
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp3
.LBB0_0:
	addi	x6, x0, 12
	lui	x5, 3
	readpm	x7, x5, 0
	readpm	x11, x5, 4
	sw	x11, x7, 0
	lui	x5, 0
	writepm	x5, x5, 4
	addi	x10, x0, 4
	lui	x5, 2
	writepm	x5, x10, 16
	readpm	x5, x5, 8
	flw	f3, x10, 0
	fwritepm	x5, f3, 16
	lui	x5, 1
	writepm	x5, x6, 12
	freadpm	f1, x5, 12
	fsqrt.s	f1, f1
	fwritepm	x5, f1, 20
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
