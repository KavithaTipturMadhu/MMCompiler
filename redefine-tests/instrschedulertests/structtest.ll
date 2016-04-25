	.text
	.align	4
	.type	main,@function
.topology	0	0               # @main
HyperOp#0:
.SMD	1	1	0	0
.distcnt	0	0	0	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 20
	lui	x5, 0
	readpm	x6, x5, 0
	readpm	x5, x5, 4
	sw	x6, x5, 4
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp1
.LBB0_0:
	addi	x6, x0, 0
	lui	x5, 3
	writepm	x5, x6, 4
	lui	x5, 0
	writepm	x5, x6, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp2
.LBB0_0:
	addi	x7, x0, 10
	lui	x5, 3
	writepm	x5, x7, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp3
.LBB0_0:
	lui	x5, 3
	readpm	x7, x5, 0
	readpm	x6, x5, 4
	sw	x6, x7, 0
	lui	x5, 0
	writepm	x5, x5, 4
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
.launchCnt	0
.OperandValidity	000000000000000000000000000000000000
.OpWaitCnt	0
.isnextHyOpInstValid	0
.nextHyperOpInst	0
.data	0
