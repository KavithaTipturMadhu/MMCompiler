	.text
.topology	0	0
.numHyop_in_this_CR	3
HyperOp#0:
.SMD	1	0	0	0
.distcnt	0	0	0	0	

.code	;pHyperOp0
.LBB0_1:
	addi	x5, x0, 1
	lui	x6, 2
	writepm	x6, x5, 0
	addi	x6, x0, 64
	lw	x7, x31, 0
	writecmp	x6, x7, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp1
.LBB0_1:
	addi	x6, x31, 0
	lui	x7, 2
	writepm	x7, x6, 4
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp2
.LBB0_1:
	lui	x7, 2
	dreadpm	x8, x7, 0
	dreadpm	x7, x7, 4
	sw	x7, x8, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp3
.LBB0_1:
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0
.isStaticHyperOp	1
HyOpInst#	0	
.operands
1
.isValid	1
.isActive	0
.isIntrinsic	0
.depthHEG	0
.launchCnt	1
.OperandValidity	000000000000000000000000000000000001
.OpWaitCnt	1
.isnextHyOpInstValid	0
.nextHyperOpInst	0
HyperOp#1:
.SMD	0	0	0	1
.distcnt	0	0	0	0	

.code	;pHyperOp0
.LBB1_1:
	addi	x5, x0, 200
	lui	x6, 2
	writepm	x6, x5, 0
	addi	x6, x0, 0
	lw	x7, x31, 0
	writecm	x6, x7, 0
	fdelete 	x31, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp1
.LBB1_1:
	addi	x6, x31, 0
	lui	x7, 2
	writepm	x7, x6, 4
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp2
.LBB1_1:
	lui	x7, 2
	dreadpm	x8, x7, 0
	dreadpm	x7, x7, 4
	sw	x7, x8, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp3
.LBB1_1:
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0
.isStaticHyperOp	1
HyOpInst#	64	
.operands
1
.isValid	1
.isActive	1
.isIntrinsic	0
.depthHEG	1
.launchCnt	1
.OperandValidity	000000000000000000000000000000000001
.OpWaitCnt	1
.isnextHyOpInstValid	0
.nextHyperOpInst	0
HyperOp#2:
.SMD	0	1	0	0
.distcnt	0	1	0	0	

.code	;pHyperOp0
.LBB2_0:
	add	x30, x0, x31
	lui	x5, "%hi("ga#0")"
	lui	x6, "%lo("ga#0")"
	srli	x6, x6, 12
	add	x5, x6, x5
	addi	x6, x0, 4
	mul	x6, x6, x31
	add	x31, x6, x5
	addi	x5, x31, 0
	lui	x6, 2
	writepm	x6, x5, 4
	fdelete 	x30, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp1
.LBB2_0:
	add	x30, x0, x31
	lui	x5, "%hi("ga#0")"
	lui	x6, "%lo("ga#0")"
	srli	x6, x6, 12
	add	x5, x6, x5
	addi	x6, x0, 4
	mul	x6, x6, x31
	add	x31, x6, x5
	lui	x5, 2
	writepm	x5, x1, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp2
.LBB2_0:
	add	x30, x0, x31
	lui	x5, "%hi("ga#0")"
	lui	x6, "%lo("ga#0")"
	srli	x6, x6, 12
	add	x5, x6, x5
	addi	x6, x0, 4
	mul	x6, x6, x31
	add	x31, x6, x5
	lui	x5, 2
	dreadpm	x6, x5, 0
	dreadpm	x5, x5, 4
	sw	x5, x6, 0
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

.code	;pHyperOp3
.LBB2_0:
	add	x30, x0, x31
	lui	x5, "%hi("ga#0")"
	lui	x6, "%lo("ga#0")"
	srli	x6, x6, 12
	add	x5, x6, x5
	addi	x6, x0, 4
	mul	x6, x6, x31
	add	x31, x6, x5
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0
.isStaticHyperOp	1
HyOpInst#	0	
.isValid	1
.isActive	1
.isIntrinsic	0
.depthHEG	2
.launchCnt	1
.OperandValidity	000000000000000000000000000000000001
.OpWaitCnt	1
.isnextHyOpInstValid	0
.nextHyperOpInst	0
.data	0
