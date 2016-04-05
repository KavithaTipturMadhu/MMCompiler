	.text
	.align	4
	.type	test,@function
.topology	0	0               # @test
HyperOp#0:
.SMD	1	1	0	0
.distcnt	0	

.code	;pHyperOp0
.LBB0_0:
	addi	x5, x0, 10
	addi	x7, x0, 25
	addi	x6, x0, 0
	sw	x6, x0, 20
	addi	x10, x0, 24
	sw	x10, x0, 0
	sw	x6, x0, 16
	sw	x6, x0, 12
	sw	x6, x0, 8
	sw	x6, x0, 4
	sw	x6, x0, 0
	sw	x6, x7, 0
	sw	x6, x5, 4
.LBB0_1:
	addi	x5, x0, 24
	lw	x5, x5, 0
	blt	x0, x5, .LBB0_11
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_2
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_2:
	addi	x5, x0, 24
	lw	x5, x5, 0
	addi	x6, x0, 28
	addi	x5, x5, 1
	sw	x6, x5, 0
	addi	x5, x0, -1
	addi	x6, x0, 32
	sw	x6, x5, 0
.LBB0_3:
	addi	x5, x0, 1
	addi	x6, x0, 28
	lw	x6, x6, 0
	blt	x5, x6, .LBB0_8
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_4
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_4:
	addi	x5, x0, 32
	lw	x5, x5, 0
	addi	x6, x0, 28
	lw	x7, x6, 0
	addi	x6, x0, 0
	slli	x7, x7, 2
	add	x6, x6, x7
	lw	x6, x6, 0
	blt	x6, x5, .LBB0_6
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_5
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_5:
	addi	x5, x0, -1
	addi	x6, x0, 32
	lw	x6, x6, 0
	bne	x6, x5, .LBB0_7
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_6
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_6:
	addi	x6, x0, 32
	addi	x7, x0, 0
	addi	x5, x0, 28
	lw	x10, x5, 0
	slli	x10, x10, 2
	add	x7, x7, x10
	lw	x7, x7, 0
	sw	x6, x7, 0
	addi	x6, x0, 40
	lw	x5, x5, 0
	sw	x6, x5, 0
.LBB0_7:
	addi	x5, x0, 28
	lw	x6, x5, 0
	addi	x6, x6, 1
	sw	x5, x6, 0
	jal 	.LBB0_3
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_8:
	addi	x5, x0, 32
	lw	x5, x5, 0
	addi	x6, x0, 24
	lw	x7, x6, 0
	addi	x6, x0, 0
	slli	x7, x7, 2
	add	x6, x6, x7
	lw	x6, x6, 0
	bge	x5, x6, .LBB0_10
	addi	x0, x0, 0
	addi	x0, x0, 0
	jal 	.LBB0_9
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_9:
	addi	x5, x0, 36
	addi	x6, x0, 0
	addi	x7, x0, 24
	lw	x10, x7, 0
	slli	x10, x10, 2
	add	x10, x6, x10
	lw	x10, x10, 0
	sw	x5, x10, 0
	addi	x10, x0, 32
	lw	x10, x10, 0
	lw	x7, x7, 0
	slli	x7, x7, 2
	add	x7, x6, x7
	sw	x7, x10, 0
	addi	x7, x0, 40
	lw	x7, x7, 0
	slli	x7, x7, 2
	add	x6, x6, x7
	lw	x5, x5, 0
	sw	x6, x5, 0
.LBB0_10:
	addi	x5, x0, 24
	lw	x6, x5, 0
	addi	x6, x6, 1
	sw	x5, x6, 0
	jal 	.LBB0_1
	addi	x0, x0, 0
	addi	x0, x0, 0
.LBB0_11:
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
