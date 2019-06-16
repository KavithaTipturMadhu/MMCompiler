;redefine_start111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#4
		.anno	S,B
		.numphy	1
		.numOperand	1
		.pHyPC	.PC_redefine_start1110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_redefine_start1110
.PC_redefine_start1110:
	JAL x0, .LBB0_00
.LBB0_00:
	add		x30, x0, x10
	movaddr	x1, "ga#44"
	movimm	x5, 192
	add	x2, x0, x5
	srli	x3, x30, 22
	srli	x4, x30, 12
	andi	x5, x4, 15
	srli	x6, x30, 6
	andi	x7, x6, 63
	addi	x8, x0, 832
	mul		x9, x3, x8
	addi	x26, x0, 52
	mul		x27, x5, x26
	add		x28, x27, x7
	add		x29, x28, x9
	mul		x31, x29, x2
	add		x30, x1, x31
	movimm 	x8, 	1
	movaddr 	x6, 	"ga#40"
	sw	x6, x8, 0
	movimm 	x6, 	34
	movaddr 	x7, 	"ga#32"
	movimm 	x11, 	1082130432
	sw	x7, x11, 0
	movaddr 	x7, 	"ga#0"
	sw	x7, x6, 0
	movimm 	x7, 	20
	movaddr 	x11, 	"ga#8"
	sw	x11, x7, 0
	movaddr 	x11, 	"ga#36"
	sw	x11, x0, 0
	movaddr 	x11, 	"ga#12"
	sw	x11, x8, 0
	movimm 	x11, 	4
	movaddr 	x12, 	"ga#4"
	sw	x12, x11, 0
	addi	x11, x30, 92
	movaddr 	x12, 	"ga#24"
	movimm 	x13, 	1056964608
	sw	x12, x13, 0
	movaddr 	x12, 	"ga#28"
	movimm 	x13, 	1047527424
	sw	x12, x13, 0
	sw	x11, x8, 0
	sw	x11, x0, 4
	movimm 	x8, 	-1
	sw	x11, x8, 8
	addi	x8, x30, 0
	sw	x8, x0, 4
	movimm 	x11, 	1732584193
	sw	x8, x11, 8
	movimm 	x11, 	-271733879
	sw	x8, x11, 12
	movimm 	x11, 	-1732584194
	sw	x8, x11, 16
	movimm 	x11, 	271733878
	sw	x8, x11, 20
	movimm 	x11, 	-1009589776
	sw	x8, x11, 24
	sw	x8, x7, 0
	movimm 	x7, 	5
	sb	x8, x0, 28
	sb	x8, x0, 29
	sb	x8, x0, 30
	sb	x8, x0, 31
	sb	x8, x0, 32
	sb	x8, x0, 33
	sb	x8, x0, 34
	sb	x8, x0, 35
	sb	x8, x0, 36
	sb	x8, x0, 37
	sb	x8, x0, 38
	sb	x8, x0, 39
	sb	x8, x0, 40
	sb	x8, x0, 41
	sb	x8, x0, 42
	sb	x8, x0, 43
	sb	x8, x0, 44
	sb	x8, x0, 45
	sb	x8, x0, 46
	sb	x8, x6, 47
.LBB0_10:
	addi	x7, x7, -1
	slli	x6, x7, 2
	addi	x8, x30, 0
	add	x6, x8, x6
	lw	x8, x6, 28
	srli	x11, x8, 24
	slli	x12, x8, 8
	or	x11, x11, x12
	movimm 	x12, 	16711935
	and	x11, x11, x12
	srli	x12, x8, 8
	slli	x8, x8, 24
	movimm 	x13, 	-16711936
	and	x12, x12, x13
	or	x8, x12, x8
	or	x8, x8, x11
	sw	x6, x8, 28
	bne	x7, x0, .LBB0_10
	jal x0,	.LBB0_20
.LBB0_20:
	addi	x6, x30, 0
	movimm 	x7, 	-2147483648
	sw	x6, x7, 48
	movimm 	x6, 	6
.LBB0_30:
	addi	x7, x6, 0
	addi	x6, x7, 1
	slli	x7, x7, 2
	addi	x8, x30, 0
	add	x7, x8, x7
	sw	x7, x0, 28
	movimm 	x7, 	14
	bne	x6, x7, .LBB0_30
	jal x0,	.LBB0_40
.LBB0_40:
	addi	x6, x30, 0
	lw	x7, x6, 4
	slli	x8, x7, 3
	lw	x7, x6, 0
	srli	x11, x7, 29
	or	x8, x11, x8
	sw	x6, x8, 84
	slli	x8, x7, 3
	sw	x6, x8, 88
	lw	x12, x6, 8
	lw	x13, x6, 12
	lw	x16, x6, 16
	lw	x15, x6, 20
	lw	x14, x6, 24
	srli	x7, x12, 27
	slli	x11, x12, 5
	or	x17, x7, x11
	and	x7, x16, x13
	xori	x11, x13, -1
	and	x11, x15, x11
	xor	x18, x11, x7
	lw	x11, x6, 28
	movimm 	x7, 	1518500249
	srli	x29, x13, 2
	slli	x13, x13, 30
	or	x13, x29, x13
	add	x14, x17, x14
	add	x14, x14, x18
	add	x14, x14, x11
	add	x17, x14, x7
	srli	x14, x17, 27
	slli	x18, x17, 5
	or	x18, x14, x18
	and	x14, x13, x12
	xori	x29, x12, -1
	and	x29, x16, x29
	xor	x29, x14, x29
	lw	x31, x6, 32
	srli	x14, x12, 2
	slli	x12, x12, 30
	or	x14, x14, x12
	add	x12, x15, x29
	add	x12, x12, x31
	add	x12, x12, x18
	add	x18, x12, x7
	srli	x12, x18, 27
	slli	x15, x18, 5
	or	x29, x12, x15
	and	x12, x17, x14
	xori	x15, x17, -1
	and	x15, x13, x15
	xor	x31, x12, x15
	lw	x12, x6, 36
	srli	x15, x17, 2
	slli	x17, x17, 30
	or	x15, x15, x17
	add	x16, x16, x12
	add	x16, x16, x31
	add	x16, x16, x29
	add	x17, x16, x7
	srli	x16, x17, 27
	slli	x29, x17, 5
	or	x29, x16, x29
	and	x16, x18, x15
	xori	x31, x18, -1
	and	x31, x14, x31
	xor	x31, x16, x31
	lw	x1, x6, 40
	srli	x16, x18, 2
	slli	x18, x18, 30
	or	x16, x16, x18
	add	x13, x13, x1
	add	x13, x13, x31
	add	x13, x13, x29
	add	x18, x13, x7
	srli	x13, x18, 27
	slli	x29, x18, 5
	or	x29, x13, x29
	and	x13, x17, x16
	xori	x31, x17, -1
	and	x31, x15, x31
	xor	x31, x13, x31
	lw	x1, x6, 44
	srli	x13, x17, 2
	slli	x17, x17, 30
	or	x13, x13, x17
	add	x14, x14, x1
	add	x14, x14, x31
	add	x14, x14, x29
	add	x17, x14, x7
	srli	x14, x17, 27
	slli	x29, x17, 5
	or	x29, x14, x29
	and	x14, x18, x13
	xori	x31, x18, -1
	and	x31, x16, x31
	xor	x31, x14, x31
	lw	x1, x6, 48
	srli	x14, x18, 2
	slli	x18, x18, 30
	or	x14, x14, x18
	add	x15, x15, x1
	add	x15, x15, x31
	add	x15, x15, x29
	add	x18, x15, x7
	srli	x15, x18, 27
	slli	x29, x18, 5
	or	x29, x15, x29
	and	x15, x17, x14
	xori	x31, x17, -1
	and	x31, x13, x31
	xor	x31, x15, x31
	lw	x1, x6, 52
	srli	x15, x17, 2
	slli	x17, x17, 30
	or	x15, x15, x17
	add	x16, x1, x16
	add	x16, x16, x31
	add	x16, x16, x29
	add	x17, x16, x7
	srli	x16, x17, 27
	slli	x29, x17, 5
	or	x29, x16, x29
	and	x16, x18, x15
	xori	x31, x18, -1
	and	x31, x14, x31
	xor	x31, x16, x31
	lw	x1, x6, 56
	srli	x16, x18, 2
	slli	x18, x18, 30
	or	x16, x16, x18
	add	x13, x1, x13
	add	x13, x13, x31
	add	x13, x13, x29
	add	x18, x13, x7
	srli	x13, x18, 27
	slli	x29, x18, 5
	or	x29, x13, x29
	and	x13, x17, x16
	xori	x31, x17, -1
	and	x31, x15, x31
	xor	x31, x13, x31
	lw	x13, x6, 60
	srli	x1, x17, 2
	slli	x17, x17, 30
	or	x17, x1, x17
	add	x14, x13, x14
	add	x14, x14, x31
	add	x14, x14, x29
	add	x29, x14, x7
	srli	x14, x29, 27
	slli	x31, x29, 5
	or	x31, x14, x31
	and	x14, x18, x17
	xori	x1, x18, -1
	and	x1, x16, x1
	xor	x1, x14, x1
	lw	x2, x6, 64
	srli	x14, x18, 2
	slli	x18, x18, 30
	or	x14, x14, x18
	add	x15, x2, x15
	add	x15, x15, x1
	add	x15, x15, x31
	add	x15, x15, x7
	srli	x18, x15, 27
	slli	x31, x15, 5
	or	x31, x18, x31
	and	x18, x29, x14
	xori	x1, x29, -1
	and	x1, x17, x1
	xor	x1, x18, x1
	lw	x2, x6, 68
	srli	x18, x29, 2
	slli	x29, x29, 30
	or	x18, x18, x29
	add	x16, x2, x16
	add	x16, x16, x1
	add	x16, x16, x31
	add	x16, x16, x7
	srli	x29, x16, 27
	slli	x31, x16, 5
	or	x31, x29, x31
	and	x29, x15, x18
	xori	x1, x15, -1
	and	x1, x14, x1
	xor	x1, x29, x1
	lw	x2, x6, 72
	srli	x29, x15, 2
	slli	x15, x15, 30
	or	x29, x29, x15
	add	x15, x2, x17
	add	x15, x15, x1
	add	x15, x15, x31
	add	x15, x15, x7
	srli	x17, x15, 27
	slli	x31, x15, 5
	or	x17, x17, x31
	and	x31, x16, x29
	xori	x1, x16, -1
	and	x1, x18, x1
	xor	x31, x31, x1
	lw	x2, x6, 76
	srli	x1, x16, 2
	slli	x16, x16, 30
	or	x1, x1, x16
	add	x14, x2, x14
	add	x14, x14, x31
	add	x14, x14, x17
	add	x14, x14, x7
	srli	x16, x14, 27
	slli	x17, x14, 5
	or	x16, x16, x17
	and	x17, x15, x1
	xori	x31, x15, -1
	and	x31, x29, x31
	xor	x17, x17, x31
	lw	x31, x6, 80
	srli	x2, x15, 2
	slli	x15, x15, 30
	or	x15, x2, x15
	add	x18, x31, x18
	add	x17, x18, x17
	add	x16, x17, x16
	add	x18, x16, x7
	srli	x16, x18, 27
	slli	x17, x18, 5
	or	x2, x16, x17
	and	x16, x14, x15
	xori	x17, x14, -1
	and	x17, x1, x17
	xor	x3, x16, x17
	lw	x17, x6, 84
	srli	x16, x14, 2
	slli	x14, x14, 30
	or	x16, x16, x14
	add	x14, x17, x29
	add	x14, x14, x3
	add	x14, x14, x2
	add	x29, x14, x7
	srli	x14, x29, 27
	slli	x2, x29, 5
	or	x2, x14, x2
	and	x14, x18, x16
	xori	x3, x18, -1
	and	x3, x15, x3
	xor	x3, x14, x3
	srli	x14, x18, 2
	slli	x18, x18, 30
	or	x14, x14, x18
	add	x8, x8, x1
	add	x8, x8, x3
	add	x8, x8, x2
	add	x18, x8, x7
	srli	x8, x18, 27
	slli	x1, x18, 5
	or	x1, x8, x1
	and	x8, x29, x14
	xori	x2, x29, -1
	and	x2, x16, x2
	xor	x2, x8, x2
	xor	x8, x12, x11
	xor	x8, x8, x13
	xor	x8, x8, x31
	slli	x11, x8, 1
	srli	x8, x8, 31
	or	x12, x11, x8
	sw	x6, x12, 28
	srli	x8, x29, 2
	slli	x11, x29, 30
	or	x8, x8, x11
	add	x11, x12, x15
	add	x11, x11, x2
	add	x11, x11, x1
	add	x13, x11, x7
	srli	x11, x13, 27
	slli	x15, x13, 5
	or	x29, x11, x15
	and	x11, x18, x8
	xori	x15, x18, -1
	and	x15, x14, x15
	xor	x31, x11, x15
	lw	x11, x6, 64
	xor	x11, x11, x17
	lw	x17, x6, 40
	xor	x11, x11, x17
	lw	x15, x6, 32
	xor	x11, x11, x15
	slli	x15, x11, 1
	srli	x11, x11, 31
	or	x15, x15, x11
	sw	x6, x15, 32
	srli	x11, x18, 2
	slli	x18, x18, 30
	or	x11, x11, x18
	add	x16, x15, x16
	add	x16, x16, x31
	add	x16, x16, x29
	add	x31, x16, x7
	srli	x16, x31, 27
	slli	x18, x31, 5
	or	x29, x16, x18
	and	x16, x13, x11
	xori	x18, x13, -1
	and	x18, x8, x18
	xor	x1, x16, x18
	lw	x16, x6, 88
	lw	x18, x6, 68
	xor	x16, x18, x16
	lw	x18, x6, 44
	xor	x16, x16, x18
	lw	x2, x6, 36
	xor	x16, x16, x2
	slli	x2, x16, 1
	srli	x16, x16, 31
	or	x16, x2, x16
	sw	x6, x16, 36
	srli	x2, x13, 2
	slli	x13, x13, 30
	or	x13, x2, x13
	add	x14, x16, x14
	add	x14, x14, x1
	add	x14, x14, x29
	add	x1, x14, x7
	srli	x14, x1, 27
	slli	x29, x1, 5
	or	x2, x14, x29
	and	x14, x31, x13
	xori	x29, x31, -1
	and	x29, x11, x29
	xor	x3, x14, x29
	lw	x14, x6, 72
	lw	x29, x6, 48
	xor	x12, x17, x12
	xor	x12, x12, x14
	xor	x12, x12, x29
	slli	x14, x12, 1
	srli	x12, x12, 31
	or	x14, x14, x12
	sw	x6, x14, 40
	srli	x12, x31, 2
	slli	x17, x31, 30
	or	x12, x12, x17
	add	x8, x14, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x31, x8, x7
	srli	x7, x31, 27
	slli	x8, x31, 5
	or	x2, x7, x8
	xor	x7, x12, x13
	xor	x3, x7, x1
	lw	x7, x6, 76
	lw	x17, x6, 52
	xor	x8, x18, x15
	xor	x7, x8, x7
	xor	x7, x7, x17
	slli	x8, x7, 1
	srli	x7, x7, 31
	or	x15, x8, x7
	sw	x6, x15, 44
	movimm 	x7, 	1859775393
	srli	x8, x1, 2
	slli	x18, x1, 30
	or	x8, x8, x18
	add	x11, x15, x11
	add	x11, x11, x3
	add	x11, x11, x2
	add	x1, x11, x7
	srli	x11, x1, 27
	slli	x18, x1, 5
	or	x2, x11, x18
	xor	x11, x8, x12
	xor	x3, x11, x31
	lw	x11, x6, 80
	lw	x18, x6, 56
	xor	x16, x16, x29
	xor	x11, x16, x11
	xor	x11, x11, x18
	slli	x16, x11, 1
	srli	x11, x11, 31
	or	x16, x16, x11
	sw	x6, x16, 48
	srli	x11, x31, 2
	slli	x29, x31, 30
	or	x11, x11, x29
	add	x13, x16, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x31, x13, x7
	srli	x13, x31, 27
	slli	x29, x31, 5
	or	x2, x13, x29
	xor	x13, x11, x8
	xor	x3, x13, x1
	lw	x13, x6, 84
	lw	x29, x6, 60
	xor	x14, x14, x17
	xor	x13, x14, x13
	xor	x13, x13, x29
	slli	x14, x13, 1
	srli	x13, x13, 31
	or	x14, x14, x13
	sw	x6, x14, 52
	srli	x13, x1, 2
	slli	x17, x1, 30
	or	x13, x13, x17
	add	x12, x14, x12
	add	x12, x12, x3
	add	x12, x12, x2
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x17, x1, 5
	or	x2, x12, x17
	xor	x12, x13, x11
	xor	x3, x12, x31
	lw	x12, x6, 88
	lw	x17, x6, 64
	xor	x15, x15, x18
	xor	x12, x15, x12
	xor	x12, x12, x17
	slli	x15, x12, 1
	srli	x12, x12, 31
	or	x15, x15, x12
	sw	x6, x15, 56
	srli	x12, x31, 2
	slli	x18, x31, 30
	or	x12, x12, x18
	add	x8, x15, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x31, x8, x7
	srli	x8, x31, 27
	slli	x18, x31, 5
	or	x2, x8, x18
	xor	x8, x12, x13
	xor	x3, x8, x1
	lw	x8, x6, 28
	lw	x18, x6, 68
	xor	x16, x16, x29
	xor	x8, x16, x8
	xor	x8, x8, x18
	slli	x16, x8, 1
	srli	x8, x8, 31
	or	x16, x16, x8
	sw	x6, x16, 60
	srli	x8, x1, 2
	slli	x29, x1, 30
	or	x8, x8, x29
	add	x11, x16, x11
	add	x11, x11, x3
	add	x11, x11, x2
	add	x1, x11, x7
	srli	x11, x1, 27
	slli	x29, x1, 5
	or	x2, x11, x29
	xor	x11, x8, x12
	xor	x3, x11, x31
	lw	x11, x6, 32
	lw	x29, x6, 72
	xor	x14, x14, x17
	xor	x11, x14, x11
	xor	x11, x11, x29
	slli	x14, x11, 1
	srli	x11, x11, 31
	or	x14, x14, x11
	sw	x6, x14, 64
	srli	x11, x31, 2
	slli	x17, x31, 30
	or	x11, x11, x17
	add	x13, x14, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x31, x13, x7
	srli	x13, x31, 27
	slli	x17, x31, 5
	or	x2, x13, x17
	xor	x13, x11, x8
	xor	x3, x13, x1
	lw	x13, x6, 36
	lw	x17, x6, 76
	xor	x15, x15, x18
	xor	x13, x15, x13
	xor	x13, x13, x17
	slli	x15, x13, 1
	srli	x13, x13, 31
	or	x15, x15, x13
	sw	x6, x15, 68
	srli	x13, x1, 2
	slli	x18, x1, 30
	or	x13, x13, x18
	add	x12, x15, x12
	add	x12, x12, x3
	add	x12, x12, x2
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x18, x1, 5
	or	x2, x12, x18
	xor	x12, x13, x11
	xor	x3, x12, x31
	lw	x12, x6, 40
	lw	x18, x6, 80
	xor	x16, x16, x29
	xor	x12, x16, x12
	xor	x12, x12, x18
	slli	x16, x12, 1
	srli	x12, x12, 31
	or	x16, x16, x12
	sw	x6, x16, 72
	srli	x12, x31, 2
	slli	x29, x31, 30
	or	x12, x12, x29
	add	x8, x16, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x31, x8, x7
	srli	x8, x31, 27
	slli	x29, x31, 5
	or	x2, x8, x29
	xor	x8, x12, x13
	xor	x3, x8, x1
	lw	x8, x6, 44
	lw	x29, x6, 84
	xor	x14, x14, x17
	xor	x8, x14, x8
	xor	x8, x8, x29
	slli	x14, x8, 1
	srli	x8, x8, 31
	or	x14, x14, x8
	sw	x6, x14, 76
	srli	x8, x1, 2
	slli	x17, x1, 30
	or	x8, x8, x17
	add	x11, x14, x11
	add	x11, x11, x3
	add	x11, x11, x2
	add	x1, x11, x7
	srli	x11, x1, 27
	slli	x17, x1, 5
	or	x2, x11, x17
	xor	x11, x8, x12
	xor	x3, x11, x31
	lw	x11, x6, 48
	lw	x17, x6, 88
	xor	x15, x15, x18
	xor	x11, x15, x11
	xor	x11, x11, x17
	slli	x15, x11, 1
	srli	x11, x11, 31
	or	x15, x15, x11
	sw	x6, x15, 80
	srli	x11, x31, 2
	slli	x18, x31, 30
	or	x11, x11, x18
	add	x13, x15, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x31, x13, x7
	srli	x13, x31, 27
	slli	x18, x31, 5
	or	x2, x13, x18
	xor	x13, x11, x8
	xor	x3, x13, x1
	lw	x13, x6, 52
	lw	x18, x6, 28
	xor	x16, x16, x29
	xor	x13, x16, x13
	xor	x13, x13, x18
	slli	x16, x13, 1
	srli	x13, x13, 31
	or	x16, x16, x13
	sw	x6, x16, 84
	srli	x13, x1, 2
	slli	x29, x1, 30
	or	x13, x13, x29
	add	x12, x16, x12
	add	x12, x12, x3
	add	x12, x12, x2
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x29, x1, 5
	or	x2, x12, x29
	xor	x12, x13, x11
	xor	x3, x12, x31
	lw	x12, x6, 56
	lw	x29, x6, 32
	xor	x14, x14, x17
	xor	x12, x14, x12
	xor	x12, x12, x29
	slli	x14, x12, 1
	srli	x12, x12, 31
	or	x14, x14, x12
	sw	x6, x14, 88
	srli	x12, x31, 2
	slli	x17, x31, 30
	or	x12, x12, x17
	add	x8, x14, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x31, x8, x7
	srli	x8, x31, 27
	slli	x17, x31, 5
	or	x2, x8, x17
	xor	x8, x12, x13
	xor	x3, x8, x1
	lw	x8, x6, 60
	lw	x17, x6, 36
	xor	x15, x15, x18
	xor	x8, x15, x8
	xor	x8, x8, x17
	slli	x15, x8, 1
	srli	x8, x8, 31
	or	x15, x15, x8
	sw	x6, x15, 28
	srli	x8, x1, 2
	slli	x18, x1, 30
	or	x8, x8, x18
	add	x11, x15, x11
	add	x11, x11, x3
	add	x11, x11, x2
	add	x1, x11, x7
	srli	x11, x1, 27
	slli	x18, x1, 5
	or	x2, x11, x18
	xor	x11, x8, x12
	xor	x3, x11, x31
	lw	x11, x6, 64
	lw	x18, x6, 40
	xor	x16, x16, x29
	xor	x11, x16, x11
	xor	x11, x11, x18
	slli	x16, x11, 1
	srli	x11, x11, 31
	or	x16, x16, x11
	sw	x6, x16, 32
	srli	x11, x31, 2
	slli	x29, x31, 30
	or	x11, x11, x29
	add	x13, x16, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x31, x13, x7
	srli	x13, x31, 27
	slli	x29, x31, 5
	or	x2, x13, x29
	xor	x13, x11, x8
	xor	x3, x13, x1
	lw	x13, x6, 68
	lw	x29, x6, 44
	xor	x14, x14, x17
	xor	x13, x14, x13
	xor	x13, x13, x29
	slli	x14, x13, 1
	srli	x13, x13, 31
	or	x17, x14, x13
	sw	x6, x17, 36
	srli	x13, x1, 2
	slli	x14, x1, 30
	or	x13, x13, x14
	add	x12, x17, x12
	add	x12, x12, x3
	add	x12, x12, x2
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x14, x1, 5
	or	x2, x12, x14
	xor	x12, x13, x11
	xor	x3, x12, x31
	lw	x14, x6, 72
	lw	x12, x6, 48
	xor	x15, x15, x18
	xor	x14, x15, x14
	xor	x14, x14, x12
	slli	x15, x14, 1
	srli	x14, x14, 31
	or	x18, x15, x14
	sw	x6, x18, 40
	srli	x14, x31, 2
	slli	x15, x31, 30
	or	x14, x14, x15
	add	x8, x18, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x2, x8, x7
	srli	x8, x2, 27
	slli	x15, x2, 5
	or	x3, x8, x15
	xor	x8, x14, x13
	xor	x4, x8, x1
	lw	x8, x6, 76
	lw	x31, x6, 52
	xor	x15, x16, x29
	xor	x8, x15, x8
	xor	x8, x8, x31
	slli	x15, x8, 1
	srli	x8, x8, 31
	or	x8, x15, x8
	sw	x6, x8, 44
	srli	x15, x1, 2
	slli	x16, x1, 30
	or	x15, x15, x16
	add	x11, x8, x11
	add	x11, x11, x4
	add	x11, x11, x3
	add	x3, x11, x7
	srli	x11, x3, 27
	slli	x16, x3, 5
	or	x16, x11, x16
	xor	x11, x15, x14
	xor	x29, x11, x2
	lw	x11, x6, 80
	lw	x1, x6, 56
	xor	x12, x17, x12
	xor	x11, x12, x11
	xor	x11, x11, x1
	slli	x12, x11, 1
	srli	x11, x11, 31
	or	x11, x12, x11
	sw	x6, x11, 48
	srli	x12, x2, 2
	slli	x17, x2, 30
	or	x12, x12, x17
	add	x13, x11, x13
	add	x13, x13, x29
	add	x13, x13, x16
	add	x4, x13, x7
	srli	x13, x4, 27
	slli	x16, x4, 5
	or	x17, x13, x16
	xor	x13, x12, x15
	xor	x2, x13, x3
	lw	x13, x6, 84
	lw	x29, x6, 60
	xor	x16, x18, x31
	xor	x13, x16, x13
	xor	x13, x13, x29
	slli	x16, x13, 1
	srli	x13, x13, 31
	or	x16, x16, x13
	sw	x6, x16, 52
	srli	x13, x3, 2
	slli	x18, x3, 30
	or	x13, x13, x18
	add	x14, x16, x14
	add	x14, x14, x2
	add	x14, x14, x17
	add	x2, x14, x7
	srli	x14, x2, 27
	slli	x17, x2, 5
	or	x18, x14, x17
	xor	x14, x13, x12
	xor	x31, x14, x4
	lw	x14, x6, 88
	lw	x17, x6, 64
	xor	x8, x8, x1
	xor	x8, x8, x14
	xor	x8, x8, x17
	slli	x14, x8, 1
	srli	x8, x8, 31
	or	x14, x14, x8
	sw	x6, x14, 56
	srli	x8, x4, 2
	slli	x1, x4, 30
	or	x8, x8, x1
	add	x15, x14, x15
	add	x15, x15, x31
	add	x15, x15, x18
	add	x31, x15, x7
	srli	x7, x31, 27
	slli	x15, x31, 5
	or	x1, x7, x15
	xor	x7, x8, x13
	and	x7, x2, x7
	and	x15, x8, x13
	xor	x3, x7, x15
	lw	x7, x6, 28
	lw	x18, x6, 68
	xor	x11, x11, x29
	xor	x7, x11, x7
	xor	x7, x7, x18
	slli	x11, x7, 1
	srli	x7, x7, 31
	or	x15, x11, x7
	sw	x6, x15, 60
	movimm 	x7, 	-1894007588
	srli	x11, x2, 2
	slli	x29, x2, 30
	or	x11, x11, x29
	add	x12, x15, x12
	add	x12, x12, x3
	add	x12, x12, x1
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x29, x1, 5
	or	x2, x12, x29
	xor	x12, x11, x8
	and	x12, x31, x12
	and	x29, x11, x8
	xor	x3, x12, x29
	lw	x12, x6, 32
	lw	x29, x6, 72
	xor	x16, x16, x17
	xor	x12, x16, x12
	xor	x12, x12, x29
	slli	x16, x12, 1
	srli	x12, x12, 31
	or	x16, x16, x12
	sw	x6, x16, 64
	srli	x12, x31, 2
	slli	x17, x31, 30
	or	x12, x12, x17
	add	x13, x16, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x31, x13, x7
	srli	x13, x31, 27
	slli	x17, x31, 5
	or	x2, x13, x17
	xor	x13, x12, x11
	and	x13, x1, x13
	and	x17, x12, x11
	xor	x3, x13, x17
	lw	x13, x6, 36
	lw	x17, x6, 76
	xor	x14, x14, x18
	xor	x13, x14, x13
	xor	x13, x13, x17
	slli	x14, x13, 1
	srli	x13, x13, 31
	or	x14, x14, x13
	sw	x6, x14, 68
	srli	x13, x1, 2
	slli	x18, x1, 30
	or	x13, x13, x18
	add	x8, x14, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x1, x8, x7
	srli	x8, x1, 27
	slli	x18, x1, 5
	or	x2, x8, x18
	xor	x8, x13, x12
	and	x8, x31, x8
	and	x18, x13, x12
	xor	x3, x8, x18
	lw	x8, x6, 40
	lw	x18, x6, 80
	xor	x15, x15, x29
	xor	x8, x15, x8
	xor	x8, x8, x18
	slli	x15, x8, 1
	srli	x8, x8, 31
	or	x15, x15, x8
	sw	x6, x15, 72
	srli	x8, x31, 2
	slli	x29, x31, 30
	or	x8, x8, x29
	add	x11, x15, x11
	add	x11, x11, x3
	add	x11, x11, x2
	add	x31, x11, x7
	srli	x11, x31, 27
	slli	x29, x31, 5
	or	x2, x11, x29
	xor	x11, x8, x13
	and	x11, x1, x11
	and	x29, x8, x13
	xor	x3, x11, x29
	lw	x11, x6, 44
	lw	x29, x6, 84
	xor	x16, x16, x17
	xor	x11, x16, x11
	xor	x11, x11, x29
	slli	x16, x11, 1
	srli	x11, x11, 31
	or	x16, x16, x11
	sw	x6, x16, 76
	srli	x11, x1, 2
	slli	x17, x1, 30
	or	x11, x11, x17
	add	x12, x16, x12
	add	x12, x12, x3
	add	x12, x12, x2
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x17, x1, 5
	or	x2, x12, x17
	xor	x12, x11, x8
	and	x12, x31, x12
	and	x17, x11, x8
	xor	x3, x12, x17
	lw	x12, x6, 48
	lw	x17, x6, 88
	xor	x14, x14, x18
	xor	x12, x14, x12
	xor	x12, x12, x17
	slli	x14, x12, 1
	srli	x12, x12, 31
	or	x14, x14, x12
	sw	x6, x14, 80
	srli	x12, x31, 2
	slli	x18, x31, 30
	or	x12, x12, x18
	add	x13, x14, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x31, x13, x7
	srli	x13, x31, 27
	slli	x18, x31, 5
	or	x2, x13, x18
	xor	x13, x12, x11
	and	x13, x1, x13
	and	x18, x12, x11
	xor	x3, x13, x18
	lw	x13, x6, 52
	lw	x18, x6, 28
	xor	x15, x15, x29
	xor	x13, x15, x13
	xor	x13, x13, x18
	slli	x15, x13, 1
	srli	x13, x13, 31
	or	x15, x15, x13
	sw	x6, x15, 84
	srli	x13, x1, 2
	slli	x29, x1, 30
	or	x13, x13, x29
	add	x8, x15, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x1, x8, x7
	srli	x8, x1, 27
	slli	x29, x1, 5
	or	x2, x8, x29
	xor	x8, x13, x12
	and	x8, x31, x8
	and	x29, x13, x12
	xor	x3, x8, x29
	lw	x8, x6, 56
	lw	x29, x6, 32
	xor	x16, x16, x17
	xor	x8, x16, x8
	xor	x8, x8, x29
	slli	x16, x8, 1
	srli	x8, x8, 31
	or	x16, x16, x8
	sw	x6, x16, 88
	srli	x8, x31, 2
	slli	x17, x31, 30
	or	x8, x8, x17
	add	x11, x16, x11
	add	x11, x11, x3
	add	x11, x11, x2
	add	x31, x11, x7
	srli	x11, x31, 27
	slli	x17, x31, 5
	or	x2, x11, x17
	xor	x11, x8, x13
	and	x11, x1, x11
	and	x17, x8, x13
	xor	x3, x11, x17
	lw	x11, x6, 60
	lw	x17, x6, 36
	xor	x14, x14, x18
	xor	x11, x14, x11
	xor	x11, x11, x17
	slli	x14, x11, 1
	srli	x11, x11, 31
	or	x14, x14, x11
	sw	x6, x14, 28
	srli	x11, x1, 2
	slli	x18, x1, 30
	or	x11, x11, x18
	add	x12, x14, x12
	add	x12, x12, x3
	add	x12, x12, x2
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x18, x1, 5
	or	x2, x12, x18
	xor	x12, x11, x8
	and	x12, x31, x12
	and	x18, x11, x8
	xor	x3, x12, x18
	lw	x12, x6, 64
	lw	x18, x6, 40
	xor	x15, x15, x29
	xor	x12, x15, x12
	xor	x12, x12, x18
	slli	x15, x12, 1
	srli	x12, x12, 31
	or	x15, x15, x12
	sw	x6, x15, 32
	srli	x12, x31, 2
	slli	x29, x31, 30
	or	x12, x12, x29
	add	x13, x15, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x31, x13, x7
	srli	x13, x31, 27
	slli	x29, x31, 5
	or	x2, x13, x29
	xor	x13, x12, x11
	and	x13, x1, x13
	and	x29, x12, x11
	xor	x3, x13, x29
	lw	x13, x6, 68
	lw	x29, x6, 44
	xor	x16, x16, x17
	xor	x13, x16, x13
	xor	x13, x13, x29
	slli	x16, x13, 1
	srli	x13, x13, 31
	or	x16, x16, x13
	sw	x6, x16, 36
	srli	x13, x1, 2
	slli	x17, x1, 30
	or	x13, x13, x17
	add	x8, x16, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x1, x8, x7
	srli	x8, x1, 27
	slli	x17, x1, 5
	or	x2, x8, x17
	xor	x8, x13, x12
	and	x8, x31, x8
	and	x17, x13, x12
	xor	x3, x8, x17
	lw	x8, x6, 72
	lw	x17, x6, 48
	xor	x14, x14, x18
	xor	x8, x14, x8
	xor	x8, x8, x17
	slli	x14, x8, 1
	srli	x8, x8, 31
	or	x14, x14, x8
	sw	x6, x14, 40
	srli	x8, x31, 2
	slli	x18, x31, 30
	or	x8, x8, x18
	add	x11, x14, x11
	add	x11, x11, x3
	add	x11, x11, x2
	add	x31, x11, x7
	srli	x11, x31, 27
	slli	x18, x31, 5
	or	x2, x11, x18
	xor	x11, x8, x13
	and	x11, x1, x11
	and	x18, x8, x13
	xor	x3, x11, x18
	lw	x11, x6, 76
	lw	x18, x6, 52
	xor	x15, x15, x29
	xor	x11, x15, x11
	xor	x11, x11, x18
	slli	x15, x11, 1
	srli	x11, x11, 31
	or	x15, x15, x11
	sw	x6, x15, 44
	srli	x11, x1, 2
	slli	x29, x1, 30
	or	x11, x11, x29
	add	x12, x15, x12
	add	x12, x12, x3
	add	x12, x12, x2
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x29, x1, 5
	or	x2, x12, x29
	xor	x12, x11, x8
	and	x12, x31, x12
	and	x29, x11, x8
	xor	x3, x12, x29
	lw	x12, x6, 80
	lw	x29, x6, 56
	xor	x16, x16, x17
	xor	x12, x16, x12
	xor	x12, x12, x29
	slli	x16, x12, 1
	srli	x12, x12, 31
	or	x16, x16, x12
	sw	x6, x16, 48
	srli	x12, x31, 2
	slli	x17, x31, 30
	or	x12, x12, x17
	add	x13, x16, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x31, x13, x7
	srli	x13, x31, 27
	slli	x17, x31, 5
	or	x2, x13, x17
	xor	x13, x12, x11
	and	x13, x1, x13
	and	x17, x12, x11
	xor	x3, x13, x17
	lw	x13, x6, 84
	lw	x17, x6, 60
	xor	x14, x14, x18
	xor	x13, x14, x13
	xor	x13, x13, x17
	slli	x14, x13, 1
	srli	x13, x13, 31
	or	x14, x14, x13
	sw	x6, x14, 52
	srli	x13, x1, 2
	slli	x18, x1, 30
	or	x13, x13, x18
	add	x8, x14, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x1, x8, x7
	srli	x8, x1, 27
	slli	x18, x1, 5
	or	x2, x8, x18
	xor	x8, x13, x12
	and	x8, x31, x8
	and	x18, x13, x12
	xor	x3, x8, x18
	lw	x8, x6, 88
	lw	x18, x6, 64
	xor	x15, x15, x29
	xor	x8, x15, x8
	xor	x8, x8, x18
	slli	x15, x8, 1
	srli	x8, x8, 31
	or	x15, x15, x8
	sw	x6, x15, 56
	srli	x8, x31, 2
	slli	x29, x31, 30
	or	x8, x8, x29
	add	x11, x15, x11
	add	x11, x11, x3
	add	x11, x11, x2
	add	x31, x11, x7
	srli	x11, x31, 27
	slli	x29, x31, 5
	or	x2, x11, x29
	xor	x11, x8, x13
	and	x11, x1, x11
	and	x29, x8, x13
	xor	x3, x11, x29
	lw	x11, x6, 28
	lw	x29, x6, 68
	xor	x16, x16, x17
	xor	x11, x16, x11
	xor	x11, x11, x29
	slli	x16, x11, 1
	srli	x11, x11, 31
	or	x16, x16, x11
	sw	x6, x16, 60
	srli	x11, x1, 2
	slli	x17, x1, 30
	or	x11, x11, x17
	add	x12, x16, x12
	add	x12, x12, x3
	add	x12, x12, x2
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x17, x1, 5
	or	x2, x12, x17
	xor	x12, x11, x8
	and	x12, x31, x12
	and	x17, x11, x8
	xor	x3, x12, x17
	lw	x12, x6, 32
	lw	x17, x6, 72
	xor	x14, x14, x18
	xor	x12, x14, x12
	xor	x12, x12, x17
	slli	x14, x12, 1
	srli	x12, x12, 31
	or	x14, x14, x12
	sw	x6, x14, 64
	srli	x12, x31, 2
	slli	x18, x31, 30
	or	x12, x12, x18
	add	x13, x14, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x31, x13, x7
	srli	x13, x31, 27
	slli	x18, x31, 5
	or	x2, x13, x18
	xor	x13, x12, x11
	and	x13, x1, x13
	and	x18, x12, x11
	xor	x3, x13, x18
	lw	x13, x6, 36
	lw	x18, x6, 76
	xor	x15, x15, x29
	xor	x13, x15, x13
	xor	x13, x13, x18
	slli	x15, x13, 1
	srli	x13, x13, 31
	or	x15, x15, x13
	sw	x6, x15, 68
	srli	x13, x1, 2
	slli	x29, x1, 30
	or	x13, x13, x29
	add	x8, x15, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x1, x8, x7
	srli	x8, x1, 27
	slli	x29, x1, 5
	or	x2, x8, x29
	xor	x8, x13, x12
	and	x8, x31, x8
	and	x29, x13, x12
	xor	x3, x8, x29
	lw	x8, x6, 40
	lw	x29, x6, 80
	xor	x16, x16, x17
	xor	x8, x16, x8
	xor	x8, x8, x29
	slli	x16, x8, 1
	srli	x8, x8, 31
	or	x16, x16, x8
	sw	x6, x16, 72
	srli	x8, x31, 2
	slli	x17, x31, 30
	or	x8, x8, x17
	add	x11, x16, x11
	add	x11, x11, x3
	add	x11, x11, x2
	add	x31, x11, x7
	srli	x7, x31, 27
	slli	x11, x31, 5
	or	x2, x7, x11
	xor	x7, x8, x13
	xor	x3, x7, x1
	lw	x7, x6, 44
	lw	x17, x6, 84
	xor	x11, x14, x18
	xor	x7, x11, x7
	xor	x7, x7, x17
	slli	x11, x7, 1
	srli	x7, x7, 31
	or	x14, x11, x7
	sw	x6, x14, 76
	movimm 	x7, 	-899497514
	srli	x11, x1, 2
	slli	x18, x1, 30
	or	x11, x11, x18
	add	x12, x14, x12
	add	x12, x12, x3
	add	x12, x12, x2
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x18, x1, 5
	or	x2, x12, x18
	xor	x12, x11, x8
	xor	x3, x12, x31
	lw	x12, x6, 48
	lw	x18, x6, 88
	xor	x15, x15, x29
	xor	x12, x15, x12
	xor	x12, x12, x18
	slli	x15, x12, 1
	srli	x12, x12, 31
	or	x15, x15, x12
	sw	x6, x15, 80
	srli	x12, x31, 2
	slli	x29, x31, 30
	or	x12, x12, x29
	add	x13, x15, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x31, x13, x7
	srli	x13, x31, 27
	slli	x29, x31, 5
	or	x2, x13, x29
	xor	x13, x12, x11
	xor	x3, x13, x1
	lw	x13, x6, 52
	lw	x29, x6, 28
	xor	x16, x16, x17
	xor	x13, x16, x13
	xor	x13, x13, x29
	slli	x16, x13, 1
	srli	x13, x13, 31
	or	x16, x16, x13
	sw	x6, x16, 84
	srli	x13, x1, 2
	slli	x17, x1, 30
	or	x13, x13, x17
	add	x8, x16, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x1, x8, x7
	srli	x8, x1, 27
	slli	x17, x1, 5
	or	x2, x8, x17
	xor	x8, x13, x12
	xor	x3, x8, x31
	lw	x8, x6, 56
	lw	x17, x6, 32
	xor	x14, x14, x18
	xor	x8, x14, x8
	xor	x8, x8, x17
	slli	x14, x8, 1
	srli	x8, x8, 31
	or	x14, x14, x8
	sw	x6, x14, 88
	srli	x8, x31, 2
	slli	x18, x31, 30
	or	x8, x8, x18
	add	x11, x14, x11
	add	x11, x11, x3
	add	x11, x11, x2
	add	x31, x11, x7
	srli	x11, x31, 27
	slli	x18, x31, 5
	or	x2, x11, x18
	xor	x11, x8, x13
	xor	x3, x11, x1
	lw	x11, x6, 60
	lw	x18, x6, 36
	xor	x15, x15, x29
	xor	x11, x15, x11
	xor	x11, x11, x18
	slli	x15, x11, 1
	srli	x11, x11, 31
	or	x15, x15, x11
	sw	x6, x15, 28
	srli	x11, x1, 2
	slli	x29, x1, 30
	or	x11, x11, x29
	add	x12, x15, x12
	add	x12, x12, x3
	add	x12, x12, x2
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x29, x1, 5
	or	x2, x12, x29
	xor	x12, x11, x8
	xor	x3, x12, x31
	lw	x12, x6, 64
	lw	x29, x6, 40
	xor	x16, x16, x17
	xor	x12, x16, x12
	xor	x12, x12, x29
	slli	x16, x12, 1
	srli	x12, x12, 31
	or	x16, x16, x12
	sw	x6, x16, 32
	srli	x12, x31, 2
	slli	x17, x31, 30
	or	x12, x12, x17
	add	x13, x16, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x31, x13, x7
	srli	x13, x31, 27
	slli	x17, x31, 5
	or	x2, x13, x17
	xor	x13, x12, x11
	xor	x3, x13, x1
	lw	x13, x6, 68
	lw	x17, x6, 44
	xor	x14, x14, x18
	xor	x13, x14, x13
	xor	x13, x13, x17
	slli	x14, x13, 1
	srli	x13, x13, 31
	or	x14, x14, x13
	sw	x6, x14, 36
	srli	x13, x1, 2
	slli	x18, x1, 30
	or	x13, x13, x18
	add	x8, x14, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x1, x8, x7
	srli	x8, x1, 27
	slli	x18, x1, 5
	or	x2, x8, x18
	xor	x8, x13, x12
	xor	x3, x8, x31
	lw	x8, x6, 72
	lw	x18, x6, 48
	xor	x15, x15, x29
	xor	x8, x15, x8
	xor	x8, x8, x18
	slli	x15, x8, 1
	srli	x8, x8, 31
	or	x15, x15, x8
	sw	x6, x15, 40
	srli	x8, x31, 2
	slli	x29, x31, 30
	or	x8, x8, x29
	add	x11, x15, x11
	add	x11, x11, x3
	add	x11, x11, x2
	add	x31, x11, x7
	srli	x11, x31, 27
	slli	x29, x31, 5
	or	x2, x11, x29
	xor	x11, x8, x13
	xor	x3, x11, x1
	lw	x11, x6, 76
	lw	x29, x6, 52
	xor	x16, x16, x17
	xor	x11, x16, x11
	xor	x11, x11, x29
	slli	x16, x11, 1
	srli	x11, x11, 31
	or	x16, x16, x11
	sw	x6, x16, 44
	srli	x11, x1, 2
	slli	x17, x1, 30
	or	x11, x11, x17
	add	x12, x16, x12
	add	x12, x12, x3
	add	x12, x12, x2
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x17, x1, 5
	or	x2, x12, x17
	xor	x12, x11, x8
	xor	x3, x12, x31
	lw	x12, x6, 80
	lw	x17, x6, 56
	xor	x14, x14, x18
	xor	x12, x14, x12
	xor	x12, x12, x17
	slli	x14, x12, 1
	srli	x12, x12, 31
	or	x14, x14, x12
	sw	x6, x14, 48
	srli	x12, x31, 2
	slli	x18, x31, 30
	or	x12, x12, x18
	add	x13, x14, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x31, x13, x7
	srli	x13, x31, 27
	slli	x18, x31, 5
	or	x2, x13, x18
	xor	x13, x12, x11
	xor	x3, x13, x1
	lw	x13, x6, 84
	lw	x18, x6, 60
	xor	x15, x15, x29
	xor	x13, x15, x13
	xor	x13, x13, x18
	slli	x15, x13, 1
	srli	x13, x13, 31
	or	x15, x15, x13
	sw	x6, x15, 52
	srli	x13, x1, 2
	slli	x29, x1, 30
	or	x13, x13, x29
	add	x8, x15, x8
	add	x8, x8, x3
	add	x8, x8, x2
	add	x1, x8, x7
	srli	x8, x1, 27
	slli	x29, x1, 5
	or	x2, x8, x29
	xor	x8, x13, x12
	xor	x3, x8, x31
	lw	x8, x6, 88
	lw	x29, x6, 64
	xor	x16, x16, x17
	xor	x8, x16, x8
	xor	x8, x8, x29
	slli	x16, x8, 1
	srli	x8, x8, 31
	or	x16, x16, x8
	sw	x6, x16, 56
	srli	x8, x31, 2
	slli	x17, x31, 30
	or	x8, x8, x17
	add	x11, x16, x11
	add	x11, x11, x3
	add	x11, x11, x2
	add	x31, x11, x7
	srli	x11, x31, 27
	slli	x17, x31, 5
	or	x2, x11, x17
	xor	x11, x8, x13
	xor	x3, x11, x1
	lw	x11, x6, 28
	lw	x17, x6, 68
	xor	x14, x14, x18
	xor	x11, x14, x11
	xor	x11, x11, x17
	slli	x14, x11, 1
	srli	x11, x11, 31
	or	x14, x14, x11
	sw	x6, x14, 60
	srli	x11, x1, 2
	slli	x18, x1, 30
	or	x11, x11, x18
	add	x12, x14, x12
	add	x12, x12, x3
	add	x12, x12, x2
	add	x1, x12, x7
	srli	x12, x1, 27
	slli	x18, x1, 5
	or	x2, x12, x18
	xor	x12, x11, x8
	xor	x3, x12, x31
	lw	x12, x6, 32
	lw	x18, x6, 72
	xor	x15, x15, x29
	xor	x12, x15, x12
	xor	x12, x12, x18
	slli	x15, x12, 1
	srli	x12, x12, 31
	or	x15, x15, x12
	sw	x6, x15, 64
	srli	x12, x31, 2
	slli	x29, x31, 30
	or	x12, x12, x29
	add	x13, x15, x13
	add	x13, x13, x3
	add	x13, x13, x2
	add	x2, x13, x7
	srli	x13, x2, 27
	slli	x29, x2, 5
	or	x31, x13, x29
	xor	x13, x12, x11
	xor	x3, x13, x1
	lw	x13, x6, 36
	lw	x29, x6, 76
	xor	x16, x16, x17
	xor	x13, x16, x13
	xor	x13, x13, x29
	slli	x16, x13, 1
	srli	x13, x13, 31
	or	x16, x16, x13
	sw	x6, x16, 68
	srli	x13, x1, 2
	slli	x17, x1, 30
	or	x13, x13, x17
	add	x8, x16, x8
	add	x8, x8, x3
	add	x8, x8, x31
	add	x8, x8, x7
	srli	x17, x8, 27
	slli	x31, x8, 5
	or	x1, x17, x31
	xor	x17, x13, x12
	xor	x3, x17, x2
	lw	x17, x6, 40
	lw	x31, x6, 80
	xor	x14, x14, x18
	xor	x14, x14, x17
	xor	x14, x14, x31
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x6, x17, 72
	srli	x14, x2, 2
	slli	x18, x2, 30
	or	x14, x14, x18
	add	x11, x17, x11
	add	x11, x11, x3
	add	x11, x11, x1
	add	x11, x11, x7
	srli	x18, x11, 27
	slli	x1, x11, 5
	or	x1, x18, x1
	xor	x18, x14, x13
	xor	x2, x18, x8
	lw	x3, x6, 44
	lw	x18, x6, 84
	xor	x15, x15, x29
	xor	x15, x15, x3
	xor	x15, x15, x18
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x15, x29, x15
	sw	x6, x15, 76
	srli	x29, x8, 2
	slli	x8, x8, 30
	or	x8, x29, x8
	add	x12, x15, x12
	add	x12, x12, x2
	add	x12, x12, x1
	add	x29, x12, x7
	srli	x12, x29, 27
	slli	x1, x29, 5
	or	x1, x12, x1
	xor	x12, x8, x14
	xor	x2, x12, x11
	lw	x3, x6, 48
	lw	x12, x6, 88
	xor	x16, x16, x31
	xor	x16, x16, x3
	xor	x16, x16, x12
	slli	x31, x16, 1
	srli	x16, x16, 31
	or	x16, x31, x16
	sw	x6, x16, 80
	srli	x31, x11, 2
	slli	x11, x11, 30
	or	x11, x31, x11
	add	x13, x16, x13
	add	x13, x13, x2
	add	x13, x13, x1
	add	x13, x13, x7
	srli	x16, x13, 27
	slli	x31, x13, 5
	or	x31, x16, x31
	xor	x16, x11, x8
	xor	x1, x16, x29
	lw	x16, x6, 52
	lw	x2, x6, 28
	xor	x17, x17, x18
	xor	x16, x17, x16
	xor	x16, x16, x2
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x6, x17, 84
	srli	x16, x29, 2
	slli	x18, x29, 30
	or	x16, x16, x18
	add	x14, x17, x14
	add	x14, x14, x1
	add	x14, x14, x31
	add	x14, x14, x7
	srli	x17, x14, 27
	slli	x18, x14, 5
	or	x17, x17, x18
	xor	x18, x16, x11
	xor	x18, x18, x13
	lw	x29, x6, 56
	lw	x31, x6, 32
	xor	x12, x15, x12
	xor	x12, x12, x29
	xor	x12, x12, x31
	slli	x15, x12, 1
	srli	x12, x12, 31
	or	x15, x15, x12
	sw	x6, x15, 88
	srli	x12, x13, 2
	slli	x13, x13, 30
	or	x12, x12, x13
	lw	x13, x6, 8
	add	x13, x13, x15
	add	x8, x13, x8
	add	x8, x8, x18
	add	x8, x8, x17
	add	x7, x8, x7
	sw	x6, x7, 8
	lw	x7, x6, 12
	add	x7, x14, x7
	sw	x6, x7, 12
	lw	x7, x6, 16
	add	x7, x12, x7
	sw	x6, x7, 16
	lw	x7, x6, 20
	add	x7, x16, x7
	sw	x6, x7, 20
	lw	x7, x6, 24
	add	x7, x11, x7
	sw	x6, x7, 24
	addi	x7, x0, 0
.LBB0_50:
	addi	x6, x7, 0
	addi	x7, x30, 0
	andi	x8, x6, -4
	add	x7, x7, x8
	lw	x7, x7, 8
	slli	x8, x6, 3
	andi	x8, x8, 24
	xori	x8, x8, 24
	srl	x8, x7, x8
	addi	x12, x30, 92
	addi	x7, x6, 1
	movimm 	x11, 	20
	add	x6, x12, x6
	sb	x6, x8, 12
	bne	x7, x11, .LBB0_50
	jal x0,	.LBB0_60
.LBB0_60:
	movimm 	x6, 	1
	movimm 	x8, 	256
	writecm	x8, x6, 60
	movaddr 	x6, 	"ga#44"
	movimm 	x7, 	192
	srli	x11, x8, 22
	srli	x12, x8, 12
	andi	x12, x12, 15
	srli	x8, x8, 6
	andi	x8, x8, 63
	addi	x13, x0, 832
	mul	x11, x11, x13
	addi	x13, x0, 52
	mul	x12, x12, x13
	add	x8, x12, x8
	add	x8, x8, x11
	mul	x7, x8, x7
	add	x6, x6, x7
	addi	x7, x0, 0
.LBB0_70:
	addi	x8, x30, 92
	add	x8, x8, x7
	lbu	x8, x8, 0
	add	x11, x6, x7
	addi	x7, x7, 1
	movimm 	x12, 	32
	sb	x11, x8, 0
	bne	x7, x12, .LBB0_70
	jal x0,	.LBB0_80
.LBB0_80:
	movimm 	x6, 	-1
	movimm 	x7, 	256
	sync	x7, x6, 60
	fdelete 	x10
	end x0
	.STATICINSTANCE_BEGIN
	.INSTID redefine_start111
	.INSTOF .HyOp#4
	.INSTADDR 0,0
	.ANNO A
	.OPERAND 15, 1
	.STATICINSTANCE_END
;uts_function2111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#5
		.anno	E
		.numphy	1
		.numOperand	5
		.pHyPC	.PC_uts_function21110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_uts_function21110
.PC_uts_function21110:
	JAL x0, .LBB1_00
.LBB1_00:
	movimm 	x6, 	128
	fdelete 	x6
	movimm 	x6, 	192
	fdelete 	x6
	slli	x6, x11, 6
	add	x6, x6, x12
	addi	x6, x6, -64
.LBB1_10:
	fdelete 	x12
	addi	x12, x12, 64
	bgeu	x6, x12, .LBB1_10
	jal x0,	.LBB1_20
.LBB1_20:
	fdelete 	x10
	end x0
	.STATICINSTANCE_BEGIN
	.INSTID uts_function2111
	.INSTOF .HyOp#5
	.INSTADDR 0,64
	.ANNO A
	.STATICINSTANCE_END
;uts_function9111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#6
		.numphy	1
		.numOperand	7
		.pHyPC	.PC_uts_function91110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_uts_function91110
.PC_uts_function91110:
	JAL x0, .LBB2_00
.LBB2_00:
	add		x30, x0, x10
	movaddr	x1, "ga#44"
	movimm	x5, 192
	add	x2, x0, x5
	srli	x3, x30, 22
	srli	x4, x30, 12
	andi	x5, x4, 15
	srli	x6, x30, 6
	andi	x7, x6, 63
	addi	x8, x0, 832
	mul		x9, x3, x8
	addi	x26, x0, 52
	mul		x27, x5, x26
	add		x28, x27, x7
	add		x29, x28, x9
	mul		x31, x29, x2
	add		x30, x1, x31
	addi	x6, x30, 0
	sw	x6, x17, 0
	movaddr 	x6, 	"ga#44"
	movimm 	x7, 	192
	srli	x8, x15, 22
	srli	x16, x15, 12
	andi	x16, x16, 15
	srli	x17, x15, 6
	andi	x17, x17, 63
	addi	x18, x0, 832
	mul	x8, x8, x18
	addi	x18, x0, 52
	mul	x16, x16, x18
	add	x16, x16, x17
	add	x8, x16, x8
	mul	x7, x8, x7
	add	x6, x6, x7
	addi	x7, x0, 0
.LBB2_10:
	addi	x8, x30, 0
	add	x8, x8, x7
	lbu	x8, x8, 0
	add	x16, x6, x7
	addi	x7, x7, 1
	movimm 	x17, 	4
	sb	x16, x8, 0
	bne	x7, x17, .LBB2_10
	jal x0,	.LBB2_20
.LBB2_20:
	movimm 	x6, 	-1
	sync	x15, x6, 60
	fdelete 	x14
	fdelete 	x13
	slli	x6, x11, 6
	add	x6, x6, x12
	addi	x6, x6, -64
.LBB2_30:
	fdelete 	x12
	addi	x12, x12, 64
	bgeu	x6, x12, .LBB2_30
	jal x0,	.LBB2_40
.LBB2_40:
	fdelete 	x10
	end x0
;uts_function41111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#7
		.anno	P
		.numphy	1
		.numOperand	1
		.pHyPC	.PC_uts_function411110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_uts_function411110
.PC_uts_function411110:
	JAL x0, .LBB3_00
.LBB3_00:
	movimm 	x6, 	1
	writecm	x11, x6, 16
	end x0
	.STATICINSTANCE_BEGIN
	.INSTID uts_function41111
	.INSTOF .HyOp#7
	.INSTADDR 0,128
	.ANNO A
	.STATICINSTANCE_END
;uts_function51111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#8
		.anno	P
		.numphy	1
		.numOperand	3
		.pHyPC	.PC_uts_function511110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_uts_function511110
.PC_uts_function511110:
	JAL x0, .LBB4_00
.LBB4_00:
	slt	x6, x0, x13
	slli	x7, x11, 6
	add	x7, x7, x12
	addi	x7, x7, -64
	addi	x8, x0, 0
.LBB4_10:
	writecm	x12, x8, 12
	writecmp	x12, x6
	addi	x12, x12, 64
	addi	x8, x8, 1
	bgeu	x7, x12, .LBB4_10
	jal x0,	.LBB4_20
.LBB4_20:
	end x0
	.STATICINSTANCE_BEGIN
	.INSTID uts_function51111
	.INSTOF .HyOp#8
	.INSTADDR 0,192
	.ANNO A
	.STATICINSTANCE_END
;uts_function6111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#9
		.anno	B
		.numphy	1
		.numOperand	1
		.pHyPC	.PC_uts_function61110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_uts_function61110
.PC_uts_function61110:
	JAL x0, .LBB5_00
.LBB5_00:
	add		x30, x0, x10
	movaddr	x1, "ga#44"
	movimm	x5, 192
	add	x2, x0, x5
	srli	x3, x30, 22
	srli	x4, x30, 12
	andi	x5, x4, 15
	srli	x6, x30, 6
	andi	x7, x6, 63
	addi	x8, x0, 832
	mul		x9, x3, x8
	addi	x26, x0, 52
	mul		x27, x5, x26
	add		x28, x27, x7
	add		x29, x28, x9
	mul		x31, x29, x2
	add		x30, x1, x31
	addi	x7, x30, 0
	lw	x6, x7, 0
	addi	x13, x30, 32
	sw	x13, x6, 0
	lw	x6, x7, 4
	sw	x13, x6, 4
	lw	x8, x7, 8
	sw	x13, x8, 8
	lbu	x8, x7, 12
	movimm 	x14, 	1
	lbu	x11, x7, 13
	lbu	x15, x7, 14
	lbu	x16, x7, 15
	addi	x12, x7, 12
	lbu	x17, x12, 4
	lbu	x18, x12, 5
	lbu	x29, x12, 6
	lbu	x31, x12, 7
	lbu	x1, x12, 8
	lbu	x2, x12, 9
	lbu	x3, x12, 10
	lbu	x4, x12, 11
	lbu	x9, x12, 12
	lbu	x19, x12, 13
	lbu	x20, x12, 14
	lbu	x21, x12, 15
	sb	x13, x8, 12
	sb	x13, x11, 13
	sb	x13, x15, 14
	sb	x13, x16, 15
	sb	x13, x17, 16
	sb	x13, x18, 17
	sb	x13, x29, 18
	sb	x13, x31, 19
	sb	x13, x1, 20
	sb	x13, x2, 21
	sb	x13, x3, 22
	sb	x13, x4, 23
	sb	x13, x9, 24
	sb	x13, x19, 25
	sb	x13, x20, 26
	sb	x13, x21, 27
	lbu	x8, x12, 16
	sb	x13, x8, 28
	lbu	x11, x12, 17
	sb	x13, x11, 29
	lbu	x12, x12, 18
	sb	x13, x12, 30
	lbu	x7, x7, 31
	sb	x13, x7, 31
	movaddr 	x13, 	"ga#32"
	flw	f1, x13, 0
	bge	x6, x14, .LBB5_10
.LBB5_850:
	jal x0,	.LBB5_40
.LBB5_10:
	fcvt.s.w	f2, x6
	movaddr 	x13, 	"ga#8"
	lw	x13, x13, 0
	fcvt.s.w	f3, x13
	fmul.s	f5, f3, f3
	fmv.x.s	x13, f5
	srli	x13, x13, 1
	movimm 	x14, 	1597463007
	sub	x13, x14, x13
	fmv.s.x	f4, x13
	movimm 	x13, 	-1090519040
	fmv.s.x	f6, x13
	fmul.s	f5, f5, f6
	fmul.s	f5, f5, f4
	fmul.s	f5, f4, f5
	movimm 	x13, 	1069547520
	fmv.s.x	f6, x13
	fadd.s	f5, f5, f6
	fmul.s	f4, f4, f5
	fcvt.s.w	f5, x0
	fle.s	x13, f5, f3
	beq	x13, x0, .LBB5_20
.LBB5_840:
	jal x0,	.LBB5_30
.LBB5_20:
	fcvt.s.w	f3, x0
	fsub.s	f4, f3, f4
.LBB5_30:
	fmul.s	f2, f2, f4
	movimm 	x13, 	1065353216
	fmv.s.x	f3, x13
	fsub.s	f2, f3, f2
	fmul.s	f1, f1, f2
.LBB5_40:
	movimm 	x13, 	1065353216
	fmv.s.x	f2, x13
	fadd.s	f1, f1, f2
	fmul.s	f3, f1, f1
	fmv.x.s	x13, f3
	srli	x13, x13, 1
	movimm 	x14, 	1597463007
	sub	x13, x14, x13
	fmv.s.x	f2, x13
	movimm 	x13, 	-1090519040
	fmv.s.x	f4, x13
	fmul.s	f3, f3, f4
	fmul.s	f3, f3, f2
	fmul.s	f3, f2, f3
	movimm 	x13, 	1069547520
	fmv.s.x	f4, x13
	fadd.s	f3, f3, f4
	fmul.s	f2, f2, f3
	fcvt.s.w	f3, x0
	fle.s	x13, f3, f1
	beq	x13, x0, .LBB5_50
.LBB5_860:
	jal x0,	.LBB5_60
.LBB5_50:
	fcvt.s.w	f1, x0
	fsub.s	f2, f1, f2
.LBB5_60:
	slli	x8, x8, 24
	slli	x11, x11, 16
	slli	x12, x12, 8
	movimm 	x13, 	2130706432
	and	x8, x8, x13
	or	x8, x8, x11
	or	x8, x8, x12
	or	x7, x8, x7
	fcvt.s.w	f1, x7
	movimm 	x7, 	-1342205681
	fmv.s.x	f3, x7
	fmul.s	f1, f1, f3
	movimm 	x7, 	1065353216
	fmv.s.x	f3, x7
	fadd.s	f3, f1, f3
	fmv.x.s	x7, f3
	movimm 	x8, 	8388607
	bge	x8, x7, .LBB5_70
.LBB5_870:
	addi	x11, x0, 0
	jal x0,	.LBB5_110
.LBB5_70:
	movimm 	x8, 	2147483647
	and	x8, x7, x8
	movimm 	x11, 	-334920857
	fmv.s.x	f1, x11
	beq	x8, x0, .LBB5_280
	jal x0,	.LBB5_80
.LBB5_80:
	movimm 	x8, 	-1
	blt	x8, x7, .LBB5_100
	jal x0,	.LBB5_90
.LBB5_90:
	fsub.s	f1, f3, f3
	movimm 	x7, 	1602847591
	fmv.s.x	f3, x7
	fmul.s	f1, f1, f3
	fmv.s.x	f3, x7
	fmul.s	f1, f1, f3
	jal x0,	.LBB5_280
.LBB5_100:
	movimm 	x7, 	1275068416
	fmv.s.x	f1, x7
	fmul.s	f3, f3, f1
	fmv.x.s	x7, f3
	movimm 	x11, 	-25
.LBB5_110:
	movimm 	x8, 	2139095040
	blt	x7, x8, .LBB5_130
	jal x0,	.LBB5_120
.LBB5_120:
	fadd.s	f1, f3, f3
	jal x0,	.LBB5_280
.LBB5_130:
	srli	x12, x7, 23
	movimm 	x8, 	8388607
	and	x8, x7, x8
	movimm 	x13, 	4913952
	add	x13, x8, x13
	movimm 	x14, 	8388608
	and	x13, x13, x14
	or	x14, x13, x8
	movimm 	x15, 	1065353216
	xor	x14, x14, x15
	fmv.s.x	f1, x14
	srli	x13, x13, 23
	movimm 	x14, 	-1082130432
	fmv.s.x	f3, x14
	fadd.s	f1, f1, f3
	movimm 	x14, 	32768
	add	x7, x7, x14
	movimm 	x14, 	8372224
	and	x14, x7, x14
	add	x7, x12, x11
	add	x7, x7, x13
	addi	x7, x7, -127
	movimm 	x11, 	49151
	bltu	x11, x14, .LBB5_210
	jal x0,	.LBB5_140
.LBB5_140:
	fcvt.s.w	f3, x0
	feq.s	x8, f3, f1
	sltiu	x8, x8, 1
	bne	x8, x0, .LBB5_180
	jal x0,	.LBB5_150
.LBB5_150:
	bne	x7, x0, .LBB5_170
.LBB5_160:
	fcvt.s.w	f1, x0
	jal x0,	.LBB5_280
.LBB5_170:
	fcvt.s.w	f1, x7
	movimm 	x7, 	1060204928
	fmv.s.x	f3, x7
	fmul.s	f3, f1, f3
	movimm 	x7, 	924317649
	fmv.s.x	f4, x7
	fmul.s	f1, f1, f4
	fadd.s	f1, f3, f1
	jal x0,	.LBB5_280
.LBB5_180:
	fmul.s	f3, f1, f1
	movimm 	x8, 	-1096111445
	fmv.s.x	f4, x8
	fmul.s	f4, f1, f4
	movimm 	x8, 	1056964608
	fmv.s.x	f5, x8
	fadd.s	f4, f4, f5
	fmul.s	f3, f3, f4
	bne	x7, x0, .LBB5_200
	jal x0,	.LBB5_190
.LBB5_190:
	fsub.s	f1, f1, f3
	jal x0,	.LBB5_280
.LBB5_200:
	fcvt.s.w	f4, x7
	movimm 	x7, 	1060204928
	fmv.s.x	f5, x7
	fmul.s	f5, f4, f5
	movimm 	x7, 	-1223165999
	fmv.s.x	f6, x7
	fmul.s	f4, f4, f6
	fadd.s	f3, f3, f4
	fsub.s	f1, f3, f1
	fsub.s	f1, f5, f1
	jal x0,	.LBB5_280
.LBB5_210:
	movimm 	x11, 	1073741824
	fmv.s.x	f3, x11
	fadd.s	f3, f1, f3
	fmul.s	f3, f3, f3
	fmv.x.s	x11, f3
	srli	x12, x11, 1
	movimm 	x11, 	1
	movimm 	x13, 	1597463007
	sub	x12, x13, x12
	fmv.s.x	f4, x12
	movimm 	x12, 	-1090519040
	fmv.s.x	f5, x12
	fmul.s	f3, f3, f5
	fmul.s	f3, f3, f4
	fmul.s	f3, f4, f3
	movimm 	x12, 	1069547520
	fmv.s.x	f5, x12
	fadd.s	f3, f3, f5
	fmul.s	f3, f4, f3
	fmul.s	f4, f1, f3
	fcvt.s.w	f3, x7
	fmul.s	f5, f4, f4
	movimm 	x12, 	-3187664
	add	x12, x8, x12
	fmul.s	f6, f5, f5
	movimm 	x13, 	3523208
	sub	x8, x13, x8
	movimm 	x13, 	1048092198
	fmv.s.x	f7, x13
	fmul.s	f7, f6, f7
	movimm 	x13, 	1053609491
	fmv.s.x	f8, x13
	fadd.s	f7, f7, f8
	fmul.s	f7, f6, f7
	movimm 	x13, 	1049749998
	fmv.s.x	f8, x13
	fmul.s	f6, f6, f8
	movimm 	x13, 	1059760810
	fmv.s.x	f8, x13
	fadd.s	f6, f6, f8
	fmul.s	f5, f5, f6
	or	x8, x12, x8
	fadd.s	f5, f5, f7
	blt	x8, x11, .LBB5_250
	jal x0,	.LBB5_220
.LBB5_220:
	movimm 	x8, 	1056964608
	fmv.s.x	f6, x8
	fmul.s	f6, f1, f6
	fmul.s	f6, f1, f6
	bne	x7, x0, .LBB5_240
	jal x0,	.LBB5_230
.LBB5_230:
	fadd.s	f3, f6, f5
	fmul.s	f3, f4, f3
	fsub.s	f3, f6, f3
	fsub.s	f1, f1, f3
	jal x0,	.LBB5_280
.LBB5_240:
	movimm 	x7, 	1060204928
	fmv.s.x	f7, x7
	fmul.s	f7, f3, f7
	fadd.s	f5, f6, f5
	fmul.s	f4, f4, f5
	movimm 	x7, 	924317649
	fmv.s.x	f5, x7
	fmul.s	f3, f3, f5
	fadd.s	f3, f3, f4
	fsub.s	f3, f6, f3
	fsub.s	f1, f3, f1
	fsub.s	f1, f7, f1
	jal x0,	.LBB5_280
.LBB5_250:
	bne	x7, x0, .LBB5_270
	jal x0,	.LBB5_260
.LBB5_260:
	fsub.s	f3, f1, f5
	fmul.s	f3, f4, f3
	fsub.s	f1, f1, f3
	jal x0,	.LBB5_280
.LBB5_270:
	movimm 	x7, 	1060204928
	fmv.s.x	f6, x7
	fmul.s	f6, f3, f6
	fsub.s	f5, f1, f5
	fmul.s	f4, f4, f5
	movimm 	x7, 	-1223165999
	fmv.s.x	f5, x7
	fmul.s	f3, f3, f5
	fadd.s	f3, f4, f3
	fsub.s	f1, f3, f1
	fsub.s	f1, f6, f1
.LBB5_280:
	movimm 	x7, 	1065353216
	fmv.s.x	f3, x7
	fsub.s	f3, f3, f2
	fmv.x.s	x7, f3
	movimm 	x8, 	8388607
	bge	x8, x7, .LBB5_290
.LBB5_880:
	addi	x11, x0, 0
	jal x0,	.LBB5_330
.LBB5_290:
	movimm 	x8, 	2147483647
	and	x8, x7, x8
	movimm 	x11, 	-334920857
	fmv.s.x	f2, x11
	beq	x8, x0, .LBB5_500
	jal x0,	.LBB5_300
.LBB5_300:
	movimm 	x8, 	-1
	blt	x8, x7, .LBB5_320
	jal x0,	.LBB5_310
.LBB5_310:
	fsub.s	f2, f3, f3
	movimm 	x7, 	1602847591
	fmv.s.x	f3, x7
	fmul.s	f2, f2, f3
	fmv.s.x	f3, x7
	fmul.s	f2, f2, f3
	jal x0,	.LBB5_500
.LBB5_320:
	movimm 	x7, 	1275068416
	fmv.s.x	f2, x7
	fmul.s	f3, f3, f2
	fmv.x.s	x7, f3
	movimm 	x11, 	-25
.LBB5_330:
	movimm 	x8, 	2139095040
	blt	x7, x8, .LBB5_350
	jal x0,	.LBB5_340
.LBB5_340:
	fadd.s	f2, f3, f3
	jal x0,	.LBB5_500
.LBB5_350:
	srli	x12, x7, 23
	movimm 	x8, 	8388607
	and	x8, x7, x8
	movimm 	x13, 	4913952
	add	x13, x8, x13
	movimm 	x14, 	8388608
	and	x13, x13, x14
	or	x14, x13, x8
	movimm 	x15, 	1065353216
	xor	x14, x14, x15
	fmv.s.x	f2, x14
	srli	x13, x13, 23
	movimm 	x14, 	-1082130432
	fmv.s.x	f3, x14
	fadd.s	f2, f2, f3
	movimm 	x14, 	32768
	add	x7, x7, x14
	movimm 	x14, 	8372224
	and	x14, x7, x14
	add	x7, x12, x11
	add	x7, x7, x13
	addi	x7, x7, -127
	movimm 	x11, 	49151
	bltu	x11, x14, .LBB5_430
	jal x0,	.LBB5_360
.LBB5_360:
	fcvt.s.w	f3, x0
	feq.s	x8, f3, f2
	sltiu	x8, x8, 1
	bne	x8, x0, .LBB5_400
	jal x0,	.LBB5_370
.LBB5_370:
	bne	x7, x0, .LBB5_390
.LBB5_380:
	fcvt.s.w	f2, x0
	jal x0,	.LBB5_500
.LBB5_390:
	fcvt.s.w	f2, x7
	movimm 	x7, 	1060204928
	fmv.s.x	f3, x7
	fmul.s	f3, f2, f3
	movimm 	x7, 	924317649
	fmv.s.x	f4, x7
	fmul.s	f2, f2, f4
	fadd.s	f2, f3, f2
	jal x0,	.LBB5_500
.LBB5_400:
	fmul.s	f3, f2, f2
	movimm 	x8, 	-1096111445
	fmv.s.x	f4, x8
	fmul.s	f4, f2, f4
	movimm 	x8, 	1056964608
	fmv.s.x	f5, x8
	fadd.s	f4, f4, f5
	fmul.s	f3, f3, f4
	bne	x7, x0, .LBB5_420
	jal x0,	.LBB5_410
.LBB5_410:
	fsub.s	f2, f2, f3
	jal x0,	.LBB5_500
.LBB5_420:
	fcvt.s.w	f4, x7
	movimm 	x7, 	1060204928
	fmv.s.x	f5, x7
	fmul.s	f5, f4, f5
	movimm 	x7, 	-1223165999
	fmv.s.x	f6, x7
	fmul.s	f4, f4, f6
	fadd.s	f3, f3, f4
	fsub.s	f2, f3, f2
	fsub.s	f2, f5, f2
	jal x0,	.LBB5_500
.LBB5_430:
	movimm 	x11, 	1073741824
	fmv.s.x	f3, x11
	fadd.s	f3, f2, f3
	fmul.s	f3, f3, f3
	fmv.x.s	x11, f3
	srli	x12, x11, 1
	movimm 	x11, 	1
	movimm 	x13, 	1597463007
	sub	x12, x13, x12
	fmv.s.x	f4, x12
	movimm 	x12, 	-1090519040
	fmv.s.x	f5, x12
	fmul.s	f3, f3, f5
	fmul.s	f3, f3, f4
	fmul.s	f3, f4, f3
	movimm 	x12, 	1069547520
	fmv.s.x	f5, x12
	fadd.s	f3, f3, f5
	fmul.s	f3, f4, f3
	fmul.s	f4, f2, f3
	fcvt.s.w	f3, x7
	fmul.s	f5, f4, f4
	movimm 	x12, 	-3187664
	add	x12, x8, x12
	fmul.s	f6, f5, f5
	movimm 	x13, 	3523208
	sub	x8, x13, x8
	movimm 	x13, 	1048092198
	fmv.s.x	f7, x13
	fmul.s	f7, f6, f7
	movimm 	x13, 	1053609491
	fmv.s.x	f8, x13
	fadd.s	f7, f7, f8
	fmul.s	f7, f6, f7
	movimm 	x13, 	1049749998
	fmv.s.x	f8, x13
	fmul.s	f6, f6, f8
	movimm 	x13, 	1059760810
	fmv.s.x	f8, x13
	fadd.s	f6, f6, f8
	fmul.s	f5, f5, f6
	or	x8, x12, x8
	fadd.s	f5, f5, f7
	blt	x8, x11, .LBB5_470
	jal x0,	.LBB5_440
.LBB5_440:
	movimm 	x8, 	1056964608
	fmv.s.x	f6, x8
	fmul.s	f6, f2, f6
	fmul.s	f6, f2, f6
	bne	x7, x0, .LBB5_460
	jal x0,	.LBB5_450
.LBB5_450:
	fadd.s	f3, f6, f5
	fmul.s	f3, f4, f3
	fsub.s	f3, f6, f3
	fsub.s	f2, f2, f3
	jal x0,	.LBB5_500
.LBB5_460:
	movimm 	x7, 	1060204928
	fmv.s.x	f7, x7
	fmul.s	f7, f3, f7
	fadd.s	f5, f6, f5
	fmul.s	f4, f4, f5
	movimm 	x7, 	924317649
	fmv.s.x	f5, x7
	fmul.s	f3, f3, f5
	fadd.s	f3, f3, f4
	fsub.s	f3, f6, f3
	fsub.s	f2, f3, f2
	fsub.s	f2, f7, f2
	jal x0,	.LBB5_500
.LBB5_470:
	bne	x7, x0, .LBB5_490
	jal x0,	.LBB5_480
.LBB5_480:
	fsub.s	f3, f2, f5
	fmul.s	f3, f4, f3
	fsub.s	f2, f2, f3
	jal x0,	.LBB5_500
.LBB5_490:
	movimm 	x7, 	1060204928
	fmv.s.x	f6, x7
	fmul.s	f6, f3, f6
	fsub.s	f5, f2, f5
	fmul.s	f4, f4, f5
	movimm 	x7, 	-1223165999
	fmv.s.x	f5, x7
	fmul.s	f3, f3, f5
	fadd.s	f3, f4, f3
	fsub.s	f2, f3, f2
	fsub.s	f2, f6, f2
.LBB5_500:
	fmul.s	f4, f2, f2
	fmv.x.s	x7, f4
	srli	x7, x7, 1
	movimm 	x8, 	1597463007
	sub	x7, x8, x7
	fmv.s.x	f3, x7
	movimm 	x7, 	-1090519040
	fmv.s.x	f5, x7
	fmul.s	f4, f4, f5
	fmul.s	f4, f4, f3
	fmul.s	f4, f3, f4
	movimm 	x7, 	1069547520
	fmv.s.x	f5, x7
	fadd.s	f4, f4, f5
	fmul.s	f3, f3, f4
	fcvt.s.w	f4, x0
	fle.s	x7, f4, f2
	beq	x7, x0, .LBB5_510
.LBB5_890:
	jal x0,	.LBB5_520
.LBB5_510:
	fcvt.s.w	f2, x0
	fsub.s	f3, f2, f3
.LBB5_520:
	fmul.s	f1, f1, f3
	fmv.x.s	x7, f1
	srli	x8, x7, 23
	andi	x8, x8, 255
	addi	x8, x8, -127
	movimm 	x11, 	22
	blt	x11, x8, .LBB5_660
	jal x0,	.LBB5_530
.LBB5_530:
	movimm 	x11, 	-1
	blt	x11, x8, .LBB5_600
	jal x0,	.LBB5_540
.LBB5_540:
	movimm 	x8, 	1900671690
	fmv.s.x	f2, x8
	fadd.s	f1, f1, f2
	fcvt.s.w	f2, x0
	fle.s	x8, f1, f2
	beq	x8, x0, .LBB5_550
.LBB5_920:
	addi	x8, x7, 0
	jal x0,	.LBB5_650
.LBB5_550:
	movimm 	x11, 	-1
	addi	x8, x0, 0
	blt	x11, x7, .LBB5_650
	jal x0,	.LBB5_560
.LBB5_560:
	movimm 	x8, 	2147483647
	and	x8, x7, x8
	beq	x0, x8, .LBB5_590
.LBB5_570:
	movimm 	x7, 	-1082130432
	jal x0,	.LBB5_590
.LBB5_590:
	addi	x8, x7, 0
	jal x0,	.LBB5_650
.LBB5_600:
	movimm 	x11, 	8388607
	srl	x11, x11, x8
	and	x12, x11, x7
	bne	x12, x0, .LBB5_610
.LBB5_940:
	jal x0,	.LBB5_680
.LBB5_610:
	movimm 	x12, 	1900671690
	fmv.s.x	f2, x12
	fadd.s	f1, f1, f2
	fcvt.s.w	f2, x0
	fle.s	x12, f1, f2
	beq	x12, x0, .LBB5_620
.LBB5_910:
	addi	x8, x7, 0
	jal x0,	.LBB5_650
.LBB5_620:
	movimm 	x12, 	-1
	bge	x12, x7, .LBB5_630
.LBB5_900:
	jal x0,	.LBB5_640
.LBB5_630:
	movimm 	x12, 	8388608
	srl	x8, x12, x8
	add	x7, x8, x7
.LBB5_640:
	xori	x8, x11, -1
	and	x8, x7, x8
.LBB5_650:
	fmv.s.x	f1, x8
	jal x0,	.LBB5_680
.LBB5_660:
	movimm 	x7, 	128
	beq	x8, x7, .LBB5_670
.LBB5_930:
	jal x0,	.LBB5_680
.LBB5_670:
	fadd.s	f1, f1, f1
.LBB5_680:
	movimm 	x7, 	5
	addi	x11, x0, 0
	blt	x7, x6, .LBB5_700
	jal x0,	.LBB5_690
.LBB5_690:
	fcvt.w.s	x11, f1
.LBB5_700:
	slt	x7, x0, x11
	addi	x6, x11, 0
	bne	x0, x7, .LBB5_720
.LBB5_710:
	addi	x6, x0, 0
.LBB5_720:
	movimm 	x8, 	1
	addi	x12, x11, 0
	bne	x0, x7, .LBB5_740
.LBB5_730:
	addi	x12, x8, 0
.LBB5_740:
	movimm 	x13, 	320
	writecm	x13, x12, 60
	addi	x12, x6, -1
	addi	x13, x12, 0
	bne	x0, x7, .LBB5_760
.LBB5_750:
	addi	x13, x8, 0
.LBB5_760:
	falloc	x7, x13
	slli	x8, x12, 6
	add	x8, x8, x7
	addi	x12, x7, 0
.LBB5_770:
	movaddr 	x13, 	.HyOp#11
	fbind 	x13, 	x12
	addi	x12, x12, 64
	bgeu	x8, x12, .LBB5_770
	jal x0,	.LBB5_780
.LBB5_780:
	slt	x12, x0, x11
	movimm 	x13, 	192
	writecm	x13, x11, 8
	writecm	x13, x7, 4
	writecm	x13, x6, 0
	writecmp	x13, x12
	movimm 	x13, 	64
	movimm 	x12, 	128
	writecm	x12, x13, 0
	slti	x11, x11, 1
	writecmp	x12, x11
	addi	x11, x7, 0
.LBB5_790:
	movaddr 	x12, 	"ga#44"
	movimm 	x13, 	192
	srli	x14, x11, 22
	srli	x15, x11, 12
	andi	x15, x15, 15
	srli	x16, x11, 6
	andi	x16, x16, 63
	addi	x17, x0, 832
	mul	x14, x14, x17
	addi	x17, x0, 52
	mul	x15, x15, x17
	add	x15, x15, x16
	add	x14, x15, x14
	mul	x13, x14, x13
	add	x12, x12, x13
	addi	x14, x12, 32
	addi	x15, x0, 0
.LBB5_800:
	addi	x13, x30, 32
	add	x13, x13, x15
	lbu	x13, x13, 0
	add	x17, x14, x15
	addi	x15, x15, 1
	movimm 	x16, 	32
	sb	x17, x13, 0
	addi	x13, x0, 0
	bne	x15, x16, .LBB5_800
	jal x0,	.LBB5_810
.LBB5_810:
	addi	x14, x30, 64
	add	x14, x14, x13
	lbu	x14, x14, 0
	add	x15, x12, x13
	addi	x13, x13, 1
	movimm 	x16, 	32
	sb	x15, x14, 0
	bne	x13, x16, .LBB5_810
	jal x0,	.LBB5_820
.LBB5_820:
	movimm 	x12, 	1
	writecm	x11, x12, 16
	writecm	x11, x6, 8
	writecm	x11, x7, 0
	movimm 	x12, 	320
	writecm	x11, x12, 4
	addi	x11, x11, 64
	bgeu	x8, x11, .LBB5_790
	jal x0,	.LBB5_830
.LBB5_830:
	movimm 	x11, 	320
	writecm	x11, x6, 4
	movimm 	x8, 	64
	writecm	x11, x8, 0
	movimm 	x11, 	128
	writecm	x8, x11, 12
	movimm 	x11, 	192
	writecm	x8, x11, 8
	writecm	x8, x6, 0
	writecm	x8, x7, 4
	fdelete 	x10
	end x0
	.STATICINSTANCE_BEGIN
	.INSTID uts_function6111
	.INSTOF .HyOp#9
	.INSTADDR 0,256
	.ANNO A
	.STATICINSTANCE_END
;uts_function_for.body71111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#10
		.anno	B
		.numphy	1
		.numOperand	3
		.pHyPC	.PC_uts_function_for_body711110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_uts_function_for_body711110
.PC_uts_function_for_body711110:
	JAL x0, .LBB6_00
.LBB6_00:
	add		x30, x0, x10
	movaddr	x1, "ga#44"
	movimm	x5, 192
	add	x2, x0, x5
	srli	x3, x30, 22
	srli	x4, x30, 12
	andi	x5, x4, 15
	srli	x6, x30, 6
	andi	x7, x6, 63
	addi	x8, x0, 832
	mul		x9, x3, x8
	addi	x26, x0, 52
	mul		x27, x5, x26
	add		x28, x27, x7
	add		x29, x28, x9
	mul		x31, x29, x2
	add		x30, x1, x31
	movimm 	x6, 	1
	bge	x12, x6, .LBB6_10
.LBB6_40:
	addi	x6, x0, 0
	jal x0,	.LBB6_30
.LBB6_10:
	addi	x7, x30, 0
	addi	x6, x0, 0
	addi	x8, x0, 0
.LBB6_20:
	slli	x13, x8, 2
	add	x13, x7, x13
	lw	x13, x13, 0
	add	x6, x13, x6
	addi	x8, x8, 1
	bne	x8, x12, .LBB6_20
	jal x0,	.LBB6_30
.LBB6_30:
	writecm	x11, x6, 16
	fdelete 	x10
	end x0
	.STATICINSTANCE_BEGIN
	.INSTID uts_function_for.body71111
	.INSTOF .HyOp#10
	.INSTADDR 0,320
	.ANNO A
	.STATICINSTANCE_END
;uts_function_for.body811111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#11
		.anno	P
		.numphy	1
		.numOperand	5
		.pHyPC	.PC_uts_function_for_body8111110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_uts_function_for_body8111110
.PC_uts_function_for_body8111110:
	JAL x0, .LBB7_00
.LBB7_00:
	add		x30, x0, x10
	movaddr	x1, "ga#44"
	movimm	x5, 192
	add	x2, x0, x5
	srli	x3, x30, 22
	srli	x4, x30, 12
	andi	x5, x4, 15
	srli	x6, x30, 6
	andi	x7, x6, 63
	addi	x8, x0, 832
	mul		x9, x3, x8
	addi	x26, x0, 52
	mul		x27, x5, x26
	add		x28, x27, x7
	add		x29, x28, x9
	mul		x31, x29, x2
	add		x30, x1, x31
	addi	x6, x30, 0
	sw	x6, x15, 0
	addi	x15, x30, 32
	lw	x7, x15, 4
	addi	x7, x7, 1
	sw	x6, x7, 4
	movimm 	x7, 	-1
	sw	x6, x7, 8
	addi	x13, x30, 156
	srli	x16, x14, 24
	movimm 	x7, 	24
	srli	x17, x14, 16
	srli	x18, x14, 8
	addi	x8, x30, 64
	sb	x13, x16, 0
	sb	x13, x17, 1
	sb	x13, x18, 2
	sb	x13, x14, 3
	sw	x8, x0, 4
	movimm 	x13, 	1732584193
	sw	x8, x13, 8
	movimm 	x13, 	-271733879
	sw	x8, x13, 12
	movimm 	x13, 	-1732584194
	sw	x8, x13, 16
	movimm 	x13, 	271733878
	sw	x8, x13, 20
	movimm 	x13, 	-1009589776
	sw	x8, x13, 24
	addi	x13, x8, 28
	lbu	x14, x15, 12
	lbu	x16, x15, 13
	lbu	x17, x15, 14
	lbu	x18, x15, 15
	lbu	x29, x15, 16
	lbu	x31, x15, 17
	lbu	x1, x15, 18
	lbu	x2, x15, 19
	lbu	x3, x15, 20
	lbu	x4, x15, 21
	lbu	x9, x15, 22
	lbu	x19, x15, 23
	lbu	x20, x15, 24
	lbu	x21, x15, 25
	lbu	x22, x15, 26
	lbu	x23, x15, 27
	lbu	x24, x15, 28
	lbu	x25, x15, 29
	lbu	x26, x15, 30
	lbu	x15, x15, 31
	sb	x8, x14, 28
	sb	x8, x16, 29
	sb	x8, x17, 30
	sb	x8, x18, 31
	sb	x8, x29, 32
	sb	x8, x31, 33
	sb	x8, x1, 34
	sb	x8, x2, 35
	sb	x8, x3, 36
	sb	x8, x4, 37
	sb	x8, x9, 38
	sb	x8, x19, 39
	sb	x8, x20, 40
	sb	x8, x21, 41
	sb	x8, x22, 42
	sb	x8, x23, 43
	sb	x8, x24, 44
	sb	x8, x25, 45
	sb	x8, x26, 46
	sb	x8, x15, 47
	sw	x8, x7, 0
	addi	x7, x0, 0
.LBB7_10:
	addi	x8, x7, 0
	addi	x7, x30, 156
	add	x7, x7, x8
	lbu	x14, x7, 0
	addi	x7, x8, 1
	movimm 	x15, 	4
	add	x8, x8, x13
	sb	x8, x14, 20
	bltu	x7, x15, .LBB7_10
	jal x0,	.LBB7_20
.LBB7_20:
	addi	x7, x30, 64
	lw	x8, x7, 0
	andi	x7, x8, 63
	addi	x13, x7, 3
	srli	x13, x13, 2
	beq	x13, x0, .LBB7_50
.LBB7_40:
	addi	x13, x13, -1
	slli	x14, x13, 2
	addi	x15, x30, 64
	add	x14, x15, x14
	lw	x15, x14, 28
	srli	x16, x15, 24
	slli	x17, x15, 8
	or	x16, x16, x17
	movimm 	x17, 	16711935
	and	x16, x16, x17
	srli	x17, x15, 8
	slli	x15, x15, 24
	movimm 	x18, 	-16711936
	and	x17, x17, x18
	or	x15, x17, x15
	or	x15, x15, x16
	sw	x14, x15, 28
	bne	x13, x0, .LBB7_40
	jal x0,	.LBB7_50
.LBB7_50:
	slli	x8, x8, 3
	andi	x8, x8, 24
	xori	x13, x8, 24
	movimm 	x8, 	-128
	sll	x14, x8, x13
	srli	x8, x7, 2
	slli	x15, x8, 2
	addi	x16, x30, 64
	add	x15, x16, x15
	lw	x16, x15, 28
	and	x14, x16, x14
	movimm 	x16, 	128
	sll	x13, x16, x13
	or	x13, x14, x13
	sw	x15, x13, 28
	movimm 	x13, 	56
	bgeu	x7, x13, .LBB7_60
	jal x0,	.LBB7_90
.LBB7_60:
	movimm 	x8, 	59
	bltu	x8, x7, .LBB7_80
	jal x0,	.LBB7_70
.LBB7_70:
	addi	x7, x30, 64
	sw	x7, x0, 88
.LBB7_80:
	addi	x7, x30, 64
	lw	x14, x7, 8
	lw	x15, x7, 12
	lw	x18, x7, 16
	lw	x17, x7, 20
	lw	x16, x7, 24
	srli	x8, x14, 27
	slli	x13, x14, 5
	or	x29, x8, x13
	and	x8, x18, x15
	xori	x13, x15, -1
	and	x13, x17, x13
	xor	x31, x13, x8
	lw	x13, x7, 28
	movimm 	x8, 	1518500249
	srli	x1, x15, 2
	slli	x15, x15, 30
	or	x15, x1, x15
	add	x16, x29, x16
	add	x16, x16, x31
	add	x16, x16, x13
	add	x29, x16, x8
	srli	x16, x29, 27
	slli	x31, x29, 5
	or	x31, x16, x31
	and	x16, x15, x14
	xori	x1, x14, -1
	and	x1, x18, x1
	xor	x1, x16, x1
	lw	x2, x7, 32
	srli	x16, x14, 2
	slli	x14, x14, 30
	or	x16, x16, x14
	add	x14, x17, x1
	add	x14, x14, x2
	add	x14, x14, x31
	add	x31, x14, x8
	srli	x14, x31, 27
	slli	x17, x31, 5
	or	x1, x14, x17
	and	x14, x29, x16
	xori	x17, x29, -1
	and	x17, x15, x17
	xor	x2, x14, x17
	lw	x14, x7, 36
	srli	x17, x29, 2
	slli	x29, x29, 30
	or	x17, x17, x29
	add	x18, x18, x14
	add	x18, x18, x2
	add	x18, x18, x1
	add	x29, x18, x8
	srli	x18, x29, 27
	slli	x1, x29, 5
	or	x1, x18, x1
	and	x18, x31, x17
	xori	x2, x31, -1
	and	x2, x16, x2
	xor	x2, x18, x2
	lw	x3, x7, 40
	srli	x18, x31, 2
	slli	x31, x31, 30
	or	x18, x18, x31
	add	x15, x15, x3
	add	x15, x15, x2
	add	x15, x15, x1
	add	x31, x15, x8
	srli	x15, x31, 27
	slli	x1, x31, 5
	or	x1, x15, x1
	and	x15, x29, x18
	xori	x2, x29, -1
	and	x2, x17, x2
	xor	x2, x15, x2
	lw	x3, x7, 44
	srli	x15, x29, 2
	slli	x29, x29, 30
	or	x15, x15, x29
	add	x16, x16, x3
	add	x16, x16, x2
	add	x16, x16, x1
	add	x29, x16, x8
	srli	x16, x29, 27
	slli	x1, x29, 5
	or	x1, x16, x1
	and	x16, x31, x15
	xori	x2, x31, -1
	and	x2, x18, x2
	xor	x2, x16, x2
	lw	x3, x7, 48
	srli	x16, x31, 2
	slli	x31, x31, 30
	or	x16, x16, x31
	add	x17, x17, x3
	add	x17, x17, x2
	add	x17, x17, x1
	add	x31, x17, x8
	srli	x17, x31, 27
	slli	x1, x31, 5
	or	x1, x17, x1
	and	x17, x29, x16
	xori	x2, x29, -1
	and	x2, x15, x2
	xor	x2, x17, x2
	lw	x3, x7, 52
	srli	x17, x29, 2
	slli	x29, x29, 30
	or	x17, x17, x29
	add	x18, x3, x18
	add	x18, x18, x2
	add	x18, x18, x1
	add	x29, x18, x8
	srli	x18, x29, 27
	slli	x1, x29, 5
	or	x1, x18, x1
	and	x18, x31, x17
	xori	x2, x31, -1
	and	x2, x16, x2
	xor	x2, x18, x2
	lw	x3, x7, 56
	srli	x18, x31, 2
	slli	x31, x31, 30
	or	x18, x18, x31
	add	x15, x3, x15
	add	x15, x15, x2
	add	x15, x15, x1
	add	x31, x15, x8
	srli	x15, x31, 27
	slli	x1, x31, 5
	or	x1, x15, x1
	and	x15, x29, x18
	xori	x2, x29, -1
	and	x2, x17, x2
	xor	x2, x15, x2
	lw	x15, x7, 60
	srli	x3, x29, 2
	slli	x29, x29, 30
	or	x29, x3, x29
	add	x16, x15, x16
	add	x16, x16, x2
	add	x16, x16, x1
	add	x1, x16, x8
	srli	x16, x1, 27
	slli	x2, x1, 5
	or	x2, x16, x2
	and	x16, x31, x29
	xori	x3, x31, -1
	and	x3, x18, x3
	xor	x3, x16, x3
	lw	x4, x7, 64
	srli	x16, x31, 2
	slli	x31, x31, 30
	or	x16, x16, x31
	add	x17, x4, x17
	add	x17, x17, x3
	add	x17, x17, x2
	add	x17, x17, x8
	srli	x31, x17, 27
	slli	x2, x17, 5
	or	x2, x31, x2
	and	x31, x1, x16
	xori	x3, x1, -1
	and	x3, x29, x3
	xor	x3, x31, x3
	lw	x4, x7, 68
	srli	x31, x1, 2
	slli	x1, x1, 30
	or	x31, x31, x1
	add	x18, x4, x18
	add	x18, x18, x3
	add	x18, x18, x2
	add	x18, x18, x8
	srli	x1, x18, 27
	slli	x2, x18, 5
	or	x1, x1, x2
	and	x2, x17, x31
	xori	x3, x17, -1
	and	x3, x16, x3
	xor	x3, x2, x3
	lw	x4, x7, 72
	srli	x2, x17, 2
	slli	x17, x17, 30
	or	x2, x2, x17
	add	x17, x4, x29
	add	x17, x17, x3
	add	x17, x17, x1
	add	x29, x17, x8
	srli	x17, x29, 27
	slli	x1, x29, 5
	or	x17, x17, x1
	and	x1, x18, x2
	xori	x3, x18, -1
	and	x3, x31, x3
	xor	x1, x1, x3
	lw	x4, x7, 76
	srli	x3, x18, 2
	slli	x18, x18, 30
	or	x3, x3, x18
	add	x16, x4, x16
	add	x16, x16, x1
	add	x16, x16, x17
	add	x16, x16, x8
	srli	x17, x16, 27
	slli	x18, x16, 5
	or	x1, x17, x18
	and	x17, x29, x3
	xori	x18, x29, -1
	and	x18, x2, x18
	xor	x4, x17, x18
	lw	x17, x7, 80
	srli	x18, x29, 2
	slli	x29, x29, 30
	or	x18, x18, x29
	add	x29, x17, x31
	add	x29, x29, x4
	add	x29, x29, x1
	add	x9, x29, x8
	srli	x29, x9, 27
	slli	x31, x9, 5
	or	x31, x29, x31
	and	x29, x16, x18
	xori	x1, x16, -1
	and	x1, x3, x1
	xor	x4, x29, x1
	lw	x1, x7, 84
	srli	x29, x16, 2
	slli	x16, x16, 30
	or	x29, x29, x16
	add	x16, x1, x2
	add	x16, x16, x4
	add	x16, x16, x31
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x31, x4, 5
	or	x2, x16, x31
	and	x16, x9, x29
	xori	x31, x9, -1
	and	x31, x18, x31
	xor	x19, x16, x31
	lw	x31, x7, 88
	srli	x16, x9, 2
	slli	x9, x9, 30
	or	x16, x16, x9
	add	x3, x31, x3
	add	x3, x3, x19
	add	x2, x3, x2
	add	x2, x2, x8
	srli	x3, x2, 27
	slli	x9, x2, 5
	or	x3, x3, x9
	and	x9, x4, x16
	xori	x19, x4, -1
	and	x19, x29, x19
	xor	x9, x9, x19
	xor	x13, x14, x13
	xor	x13, x13, x15
	xor	x13, x13, x17
	slli	x14, x13, 1
	srli	x13, x13, 31
	or	x17, x14, x13
	sw	x7, x17, 28
	srli	x13, x4, 2
	slli	x14, x4, 30
	or	x13, x13, x14
	add	x14, x17, x18
	add	x14, x14, x9
	add	x14, x14, x3
	add	x15, x14, x8
	srli	x14, x15, 27
	slli	x18, x15, 5
	or	x3, x14, x18
	and	x14, x2, x13
	xori	x18, x2, -1
	and	x18, x16, x18
	xor	x4, x14, x18
	lw	x14, x7, 64
	xor	x14, x14, x1
	lw	x1, x7, 40
	xor	x14, x14, x1
	lw	x18, x7, 32
	xor	x14, x14, x18
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 32
	srli	x14, x2, 2
	slli	x2, x2, 30
	or	x14, x14, x2
	add	x29, x18, x29
	add	x29, x29, x4
	add	x29, x29, x3
	add	x3, x29, x8
	srli	x29, x3, 27
	slli	x2, x3, 5
	or	x2, x29, x2
	and	x29, x15, x14
	xori	x4, x15, -1
	and	x4, x13, x4
	xor	x4, x29, x4
	lw	x29, x7, 68
	xor	x29, x29, x31
	lw	x31, x7, 44
	xor	x29, x29, x31
	lw	x9, x7, 36
	xor	x29, x29, x9
	slli	x9, x29, 1
	srli	x29, x29, 31
	or	x29, x9, x29
	sw	x7, x29, 36
	srli	x9, x15, 2
	slli	x15, x15, 30
	or	x15, x9, x15
	add	x16, x29, x16
	add	x16, x16, x4
	add	x16, x16, x2
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x2, x4, 5
	or	x9, x16, x2
	and	x16, x3, x15
	xori	x2, x3, -1
	and	x2, x14, x2
	xor	x19, x16, x2
	lw	x16, x7, 72
	lw	x2, x7, 48
	xor	x17, x1, x17
	xor	x16, x17, x16
	xor	x16, x16, x2
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 40
	srli	x16, x3, 2
	slli	x1, x3, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x8, x3, 27
	slli	x13, x3, 5
	or	x9, x8, x13
	xor	x8, x16, x15
	xor	x19, x8, x4
	lw	x8, x7, 76
	lw	x1, x7, 52
	xor	x13, x18, x31
	xor	x8, x13, x8
	xor	x8, x8, x1
	slli	x13, x8, 1
	srli	x8, x8, 31
	or	x18, x13, x8
	sw	x7, x18, 44
	movimm 	x8, 	1859775393
	srli	x13, x4, 2
	slli	x31, x4, 30
	or	x13, x13, x31
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x31, x4, 5
	or	x9, x14, x31
	xor	x14, x13, x16
	xor	x19, x14, x3
	lw	x14, x7, 80
	lw	x31, x7, 56
	xor	x29, x29, x2
	xor	x14, x29, x14
	xor	x14, x14, x31
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 48
	srli	x14, x3, 2
	slli	x2, x3, 30
	or	x14, x14, x2
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x3, x15, x8
	srli	x15, x3, 27
	slli	x2, x3, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	xor	x19, x15, x4
	lw	x15, x7, 84
	lw	x2, x7, 60
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x2
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 52
	srli	x15, x4, 2
	slli	x1, x4, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x1, x4, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	xor	x19, x16, x3
	lw	x16, x7, 88
	lw	x1, x7, 64
	xor	x18, x18, x31
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 56
	srli	x16, x3, 2
	slli	x31, x3, 30
	or	x16, x16, x31
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x31, x3, 5
	or	x9, x13, x31
	xor	x13, x16, x15
	xor	x19, x13, x4
	lw	x13, x7, 28
	lw	x31, x7, 68
	xor	x29, x29, x2
	xor	x13, x29, x13
	xor	x13, x13, x31
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 60
	srli	x13, x4, 2
	slli	x2, x4, 30
	or	x13, x13, x2
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x2, x4, 5
	or	x9, x14, x2
	xor	x14, x13, x16
	xor	x19, x14, x3
	lw	x14, x7, 32
	lw	x2, x7, 72
	xor	x17, x17, x1
	xor	x14, x17, x14
	xor	x14, x14, x2
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x7, x17, 64
	srli	x14, x3, 2
	slli	x1, x3, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x3, x15, x8
	srli	x15, x3, 27
	slli	x1, x3, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	xor	x19, x15, x4
	lw	x15, x7, 36
	lw	x1, x7, 76
	xor	x18, x18, x31
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 68
	srli	x15, x4, 2
	slli	x31, x4, 30
	or	x15, x15, x31
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x31, x4, 5
	or	x9, x16, x31
	xor	x16, x15, x14
	xor	x19, x16, x3
	lw	x16, x7, 40
	lw	x31, x7, 80
	xor	x29, x29, x2
	xor	x16, x29, x16
	xor	x16, x16, x31
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 72
	srli	x16, x3, 2
	slli	x2, x3, 30
	or	x16, x16, x2
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x2, x3, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	xor	x19, x13, x4
	lw	x13, x7, 44
	lw	x2, x7, 84
	xor	x17, x17, x1
	xor	x13, x17, x13
	xor	x13, x13, x2
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 76
	srli	x13, x4, 2
	slli	x1, x4, 30
	or	x13, x13, x1
	add	x14, x17, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x1, x4, 5
	or	x9, x14, x1
	xor	x14, x13, x16
	xor	x19, x14, x3
	lw	x14, x7, 48
	lw	x1, x7, 88
	xor	x18, x18, x31
	xor	x14, x18, x14
	xor	x14, x14, x1
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 80
	srli	x14, x3, 2
	slli	x31, x3, 30
	or	x14, x14, x31
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x3, x15, x8
	srli	x15, x3, 27
	slli	x31, x3, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	xor	x19, x15, x4
	lw	x15, x7, 52
	lw	x31, x7, 28
	xor	x29, x29, x2
	xor	x15, x29, x15
	xor	x15, x15, x31
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 84
	srli	x15, x4, 2
	slli	x2, x4, 30
	or	x15, x15, x2
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x2, x4, 5
	or	x9, x16, x2
	xor	x16, x15, x14
	xor	x19, x16, x3
	lw	x16, x7, 56
	lw	x2, x7, 32
	xor	x17, x17, x1
	xor	x16, x17, x16
	xor	x16, x16, x2
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 88
	srli	x16, x3, 2
	slli	x1, x3, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x1, x3, 5
	or	x9, x13, x1
	xor	x13, x16, x15
	xor	x19, x13, x4
	lw	x13, x7, 60
	lw	x1, x7, 36
	xor	x18, x18, x31
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 28
	srli	x13, x4, 2
	slli	x31, x4, 30
	or	x13, x13, x31
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x31, x4, 5
	or	x9, x14, x31
	xor	x14, x13, x16
	xor	x19, x14, x3
	lw	x14, x7, 64
	lw	x31, x7, 40
	xor	x29, x29, x2
	xor	x14, x29, x14
	xor	x14, x14, x31
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 32
	srli	x14, x3, 2
	slli	x2, x3, 30
	or	x14, x14, x2
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x3, x15, x8
	srli	x15, x3, 27
	slli	x2, x3, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	xor	x19, x15, x4
	lw	x15, x7, 68
	lw	x2, x7, 44
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x2
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x15, x17, x15
	sw	x7, x15, 36
	srli	x17, x4, 2
	slli	x1, x4, 30
	or	x17, x17, x1
	add	x16, x15, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x1, x4, 5
	or	x9, x16, x1
	xor	x16, x17, x14
	xor	x19, x16, x3
	lw	x16, x7, 72
	lw	x1, x7, 48
	xor	x18, x18, x31
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x16, x18, x16
	sw	x7, x16, 40
	srli	x18, x3, 2
	slli	x31, x3, 30
	or	x18, x18, x31
	add	x13, x16, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x31, x3, 5
	or	x9, x13, x31
	xor	x13, x18, x17
	xor	x19, x13, x4
	lw	x13, x7, 76
	lw	x31, x7, 52
	xor	x29, x29, x2
	xor	x13, x29, x13
	xor	x13, x13, x31
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x13, x29, x13
	sw	x7, x13, 44
	srli	x29, x4, 2
	slli	x2, x4, 30
	or	x29, x29, x2
	add	x14, x13, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x2, x4, 5
	or	x9, x14, x2
	xor	x14, x29, x18
	xor	x19, x14, x3
	lw	x14, x7, 80
	lw	x2, x7, 56
	xor	x15, x15, x1
	xor	x14, x15, x14
	xor	x14, x14, x2
	slli	x15, x14, 1
	srli	x14, x14, 31
	or	x14, x15, x14
	sw	x7, x14, 48
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x17, x14, x17
	add	x17, x17, x19
	add	x17, x17, x9
	add	x19, x17, x8
	srli	x17, x19, 27
	slli	x1, x19, 5
	or	x17, x17, x1
	xor	x1, x15, x29
	xor	x1, x1, x4
	lw	x9, x7, 84
	lw	x3, x7, 60
	xor	x16, x16, x31
	xor	x16, x16, x9
	xor	x16, x16, x3
	slli	x31, x16, 1
	srli	x16, x16, 31
	or	x31, x31, x16
	sw	x7, x31, 52
	srli	x16, x4, 2
	slli	x4, x4, 30
	or	x16, x16, x4
	add	x18, x31, x18
	add	x18, x18, x1
	add	x17, x18, x17
	add	x9, x17, x8
	srli	x17, x9, 27
	slli	x18, x9, 5
	or	x18, x17, x18
	xor	x17, x16, x15
	xor	x4, x17, x19
	lw	x17, x7, 88
	lw	x1, x7, 64
	xor	x13, x13, x2
	xor	x13, x13, x17
	xor	x13, x13, x1
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 56
	srli	x13, x19, 2
	slli	x2, x19, 30
	or	x13, x13, x2
	add	x29, x17, x29
	add	x29, x29, x4
	add	x18, x29, x18
	add	x4, x18, x8
	srli	x8, x4, 27
	slli	x18, x4, 5
	or	x29, x8, x18
	xor	x8, x13, x16
	and	x8, x9, x8
	and	x18, x13, x16
	xor	x19, x8, x18
	lw	x8, x7, 28
	lw	x2, x7, 68
	xor	x14, x14, x3
	xor	x8, x14, x8
	xor	x8, x8, x2
	slli	x14, x8, 1
	srli	x8, x8, 31
	or	x18, x14, x8
	sw	x7, x18, 60
	movimm 	x8, 	-1894007588
	srli	x14, x9, 2
	slli	x3, x9, 30
	or	x14, x14, x3
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x29
	add	x9, x15, x8
	srli	x15, x9, 27
	slli	x29, x9, 5
	or	x19, x15, x29
	xor	x15, x14, x13
	and	x15, x4, x15
	and	x29, x14, x13
	xor	x20, x15, x29
	lw	x15, x7, 32
	lw	x3, x7, 72
	xor	x29, x31, x1
	xor	x15, x29, x15
	xor	x15, x15, x3
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 64
	srli	x15, x4, 2
	slli	x31, x4, 30
	or	x15, x15, x31
	add	x16, x29, x16
	add	x16, x16, x20
	add	x16, x16, x19
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x31, x4, 5
	or	x1, x16, x31
	xor	x16, x15, x14
	and	x16, x9, x16
	and	x31, x15, x14
	xor	x19, x16, x31
	lw	x16, x7, 36
	lw	x31, x7, 76
	xor	x17, x17, x2
	xor	x16, x17, x16
	xor	x16, x16, x31
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 68
	srli	x16, x9, 2
	slli	x2, x9, 30
	or	x16, x16, x2
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x1
	add	x9, x13, x8
	srli	x13, x9, 27
	slli	x1, x9, 5
	or	x2, x13, x1
	xor	x13, x16, x15
	and	x13, x4, x13
	and	x1, x16, x15
	xor	x19, x13, x1
	lw	x13, x7, 40
	lw	x1, x7, 80
	xor	x18, x18, x3
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 72
	srli	x13, x4, 2
	slli	x3, x4, 30
	or	x13, x13, x3
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x2
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x2, x3, 5
	or	x4, x14, x2
	xor	x14, x13, x16
	and	x14, x9, x14
	and	x2, x13, x16
	xor	x19, x14, x2
	lw	x14, x7, 44
	lw	x2, x7, 84
	xor	x29, x29, x31
	xor	x14, x29, x14
	xor	x14, x14, x2
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 76
	srli	x14, x9, 2
	slli	x31, x9, 30
	or	x14, x14, x31
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x4
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x31, x14, x13
	xor	x19, x15, x31
	lw	x15, x7, 48
	lw	x31, x7, 88
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x31
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 80
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x1, x15, x14
	xor	x19, x16, x1
	lw	x16, x7, 52
	lw	x1, x7, 28
	xor	x18, x18, x2
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 84
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x2, x4, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x2, x16, x15
	xor	x19, x13, x2
	lw	x13, x7, 56
	lw	x2, x7, 32
	xor	x29, x29, x31
	xor	x13, x29, x13
	xor	x13, x13, x2
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 88
	srli	x13, x3, 2
	slli	x31, x3, 30
	or	x13, x13, x31
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x31, x3, 5
	or	x9, x14, x31
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x31, x13, x16
	xor	x19, x14, x31
	lw	x14, x7, 60
	lw	x31, x7, 36
	xor	x17, x17, x1
	xor	x14, x17, x14
	xor	x14, x14, x31
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x7, x17, 28
	srli	x14, x4, 2
	slli	x1, x4, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x1, x14, x13
	xor	x19, x15, x1
	lw	x15, x7, 64
	lw	x1, x7, 40
	xor	x18, x18, x2
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 32
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x2, x3, 5
	or	x9, x16, x2
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x2, x15, x14
	xor	x19, x16, x2
	lw	x16, x7, 68
	lw	x2, x7, 44
	xor	x29, x29, x31
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 36
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x31, x4, 5
	or	x9, x13, x31
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x31, x16, x15
	xor	x19, x13, x31
	lw	x13, x7, 72
	lw	x31, x7, 48
	xor	x17, x17, x1
	xor	x13, x17, x13
	xor	x13, x13, x31
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 40
	srli	x13, x3, 2
	slli	x1, x3, 30
	or	x13, x13, x1
	add	x14, x17, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x1, x3, 5
	or	x9, x14, x1
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x1, x13, x16
	xor	x19, x14, x1
	lw	x14, x7, 76
	lw	x1, x7, 52
	xor	x18, x18, x2
	xor	x14, x18, x14
	xor	x14, x14, x1
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 44
	srli	x14, x4, 2
	slli	x2, x4, 30
	or	x14, x14, x2
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x2, x4, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x2, x14, x13
	xor	x19, x15, x2
	lw	x15, x7, 80
	lw	x2, x7, 56
	xor	x29, x29, x31
	xor	x15, x29, x15
	xor	x15, x15, x2
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 48
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x31, x3, 5
	or	x9, x16, x31
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x31, x15, x14
	xor	x19, x16, x31
	lw	x16, x7, 84
	lw	x31, x7, 60
	xor	x17, x17, x1
	xor	x16, x17, x16
	xor	x16, x16, x31
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 52
	srli	x16, x4, 2
	slli	x1, x4, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x1, x4, 5
	or	x9, x13, x1
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x1, x16, x15
	xor	x19, x13, x1
	lw	x13, x7, 88
	lw	x1, x7, 64
	xor	x18, x18, x2
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 56
	srli	x13, x3, 2
	slli	x2, x3, 30
	or	x13, x13, x2
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x2, x3, 5
	or	x9, x14, x2
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x2, x13, x16
	xor	x19, x14, x2
	lw	x14, x7, 28
	lw	x2, x7, 68
	xor	x29, x29, x31
	xor	x14, x29, x14
	xor	x14, x14, x2
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 60
	srli	x14, x4, 2
	slli	x31, x4, 30
	or	x14, x14, x31
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x31, x14, x13
	xor	x19, x15, x31
	lw	x15, x7, 32
	lw	x31, x7, 72
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x31
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 64
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x1, x15, x14
	xor	x19, x16, x1
	lw	x16, x7, 36
	lw	x1, x7, 76
	xor	x18, x18, x2
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 68
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x2, x4, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x2, x16, x15
	xor	x19, x13, x2
	lw	x13, x7, 40
	lw	x2, x7, 80
	xor	x29, x29, x31
	xor	x13, x29, x13
	xor	x13, x13, x2
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 72
	srli	x13, x3, 2
	slli	x31, x3, 30
	or	x13, x13, x31
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x8, x3, 27
	slli	x14, x3, 5
	or	x9, x8, x14
	xor	x8, x13, x16
	xor	x19, x8, x4
	lw	x8, x7, 44
	lw	x31, x7, 84
	xor	x14, x17, x1
	xor	x8, x14, x8
	xor	x8, x8, x31
	slli	x14, x8, 1
	srli	x8, x8, 31
	or	x17, x14, x8
	sw	x7, x17, 76
	movimm 	x8, 	-899497514
	srli	x14, x4, 2
	slli	x1, x4, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 48
	lw	x1, x7, 88
	xor	x18, x18, x2
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 80
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x2, x3, 5
	or	x9, x16, x2
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 52
	lw	x2, x7, 28
	xor	x29, x29, x31
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 84
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x31, x4, 5
	or	x9, x13, x31
	xor	x13, x16, x15
	xor	x19, x13, x3
	lw	x13, x7, 56
	lw	x31, x7, 32
	xor	x17, x17, x1
	xor	x13, x17, x13
	xor	x13, x13, x31
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 88
	srli	x13, x3, 2
	slli	x1, x3, 30
	or	x13, x13, x1
	add	x14, x17, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x1, x3, 5
	or	x9, x14, x1
	xor	x14, x13, x16
	xor	x19, x14, x4
	lw	x14, x7, 60
	lw	x1, x7, 36
	xor	x18, x18, x2
	xor	x14, x18, x14
	xor	x14, x14, x1
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 28
	srli	x14, x4, 2
	slli	x2, x4, 30
	or	x14, x14, x2
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x2, x4, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 64
	lw	x2, x7, 40
	xor	x29, x29, x31
	xor	x15, x29, x15
	xor	x15, x15, x2
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 32
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x31, x3, 5
	or	x9, x16, x31
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 68
	lw	x31, x7, 44
	xor	x17, x17, x1
	xor	x16, x17, x16
	xor	x16, x16, x31
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 36
	srli	x16, x4, 2
	slli	x1, x4, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x1, x4, 5
	or	x9, x13, x1
	xor	x13, x16, x15
	xor	x19, x13, x3
	lw	x13, x7, 72
	lw	x1, x7, 48
	xor	x18, x18, x2
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 40
	srli	x13, x3, 2
	slli	x2, x3, 30
	or	x13, x13, x2
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x2, x3, 5
	or	x9, x14, x2
	xor	x14, x13, x16
	xor	x19, x14, x4
	lw	x14, x7, 76
	lw	x2, x7, 52
	xor	x29, x29, x31
	xor	x14, x29, x14
	xor	x14, x14, x2
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 44
	srli	x14, x4, 2
	slli	x31, x4, 30
	or	x14, x14, x31
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 80
	lw	x31, x7, 56
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x31
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 48
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 84
	lw	x1, x7, 60
	xor	x18, x18, x2
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 52
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x2, x4, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	xor	x19, x13, x3
	lw	x13, x7, 88
	lw	x2, x7, 64
	xor	x29, x29, x31
	xor	x13, x29, x13
	xor	x13, x13, x2
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 56
	srli	x13, x3, 2
	slli	x31, x3, 30
	or	x13, x13, x31
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x31, x3, 5
	or	x9, x14, x31
	xor	x14, x13, x16
	xor	x19, x14, x4
	lw	x14, x7, 28
	lw	x31, x7, 68
	xor	x17, x17, x1
	xor	x14, x17, x14
	xor	x14, x14, x31
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x7, x17, 60
	srli	x14, x4, 2
	slli	x1, x4, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 32
	lw	x1, x7, 72
	xor	x18, x18, x2
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 64
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x9, x16, x8
	srli	x16, x9, 27
	slli	x2, x9, 5
	or	x3, x16, x2
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 36
	lw	x2, x7, 76
	xor	x29, x29, x31
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 68
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x3
	add	x13, x13, x8
	srli	x31, x13, 27
	slli	x3, x13, 5
	or	x4, x31, x3
	xor	x31, x16, x15
	xor	x19, x31, x9
	lw	x31, x7, 40
	lw	x3, x7, 80
	xor	x17, x17, x1
	xor	x17, x17, x31
	xor	x17, x17, x3
	slli	x31, x17, 1
	srli	x17, x17, 31
	or	x31, x31, x17
	sw	x7, x31, 72
	srli	x17, x9, 2
	slli	x1, x9, 30
	or	x17, x17, x1
	add	x14, x31, x14
	add	x14, x14, x19
	add	x14, x14, x4
	add	x14, x14, x8
	srli	x1, x14, 27
	slli	x4, x14, 5
	or	x4, x1, x4
	xor	x1, x17, x16
	xor	x9, x1, x13
	lw	x19, x7, 44
	lw	x1, x7, 84
	xor	x18, x18, x2
	xor	x18, x18, x19
	xor	x18, x18, x1
	slli	x2, x18, 1
	srli	x18, x18, 31
	or	x18, x2, x18
	sw	x7, x18, 76
	srli	x2, x13, 2
	slli	x13, x13, 30
	or	x13, x2, x13
	add	x15, x18, x15
	add	x15, x15, x9
	add	x15, x15, x4
	add	x2, x15, x8
	srli	x15, x2, 27
	slli	x4, x2, 5
	or	x4, x15, x4
	xor	x15, x13, x17
	xor	x9, x15, x14
	lw	x19, x7, 48
	lw	x15, x7, 88
	xor	x29, x29, x3
	xor	x29, x29, x19
	xor	x29, x29, x15
	slli	x3, x29, 1
	srli	x29, x29, 31
	or	x29, x3, x29
	sw	x7, x29, 80
	srli	x3, x14, 2
	slli	x14, x14, 30
	or	x14, x3, x14
	add	x16, x29, x16
	add	x16, x16, x9
	add	x16, x16, x4
	add	x16, x16, x8
	srli	x29, x16, 27
	slli	x3, x16, 5
	or	x3, x29, x3
	xor	x29, x14, x13
	xor	x4, x29, x2
	lw	x29, x7, 52
	lw	x9, x7, 28
	xor	x31, x31, x1
	xor	x29, x31, x29
	xor	x29, x29, x9
	slli	x31, x29, 1
	srli	x29, x29, 31
	or	x31, x31, x29
	sw	x7, x31, 84
	srli	x29, x2, 2
	slli	x1, x2, 30
	or	x29, x29, x1
	add	x17, x31, x17
	add	x17, x17, x4
	add	x17, x17, x3
	add	x17, x17, x8
	srli	x31, x17, 27
	slli	x1, x17, 5
	or	x31, x31, x1
	xor	x1, x29, x14
	xor	x1, x1, x16
	lw	x2, x7, 56
	lw	x3, x7, 32
	xor	x15, x18, x15
	xor	x15, x15, x2
	xor	x15, x15, x3
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 88
	srli	x15, x16, 2
	slli	x16, x16, 30
	or	x15, x15, x16
	lw	x16, x7, 8
	add	x16, x16, x18
	add	x13, x16, x13
	add	x13, x13, x1
	add	x13, x13, x31
	add	x8, x13, x8
	sw	x7, x8, 8
	lw	x8, x7, 12
	add	x8, x17, x8
	sw	x7, x8, 12
	lw	x8, x7, 16
	add	x8, x15, x8
	sw	x7, x8, 16
	lw	x8, x7, 20
	add	x8, x29, x8
	sw	x7, x8, 20
	lw	x8, x7, 24
	add	x8, x14, x8
	sw	x7, x8, 24
	addi	x7, x0, 0
	jal x0,	.LBB7_100
.LBB7_90:
	addi	x7, x8, 1
	movimm 	x8, 	13
	bltu	x8, x7, .LBB7_110
	jal x0,	.LBB7_100
.LBB7_100:
	addi	x8, x7, 0
	addi	x7, x8, 1
	slli	x8, x8, 2
	addi	x13, x30, 64
	add	x8, x13, x8
	sw	x8, x0, 28
	movimm 	x8, 	14
	bne	x7, x8, .LBB7_100
	jal x0,	.LBB7_110
.LBB7_110:
	addi	x7, x30, 64
	lw	x8, x7, 4
	slli	x13, x8, 3
	lw	x8, x7, 0
	srli	x14, x8, 29
	or	x13, x14, x13
	sw	x7, x13, 84
	slli	x13, x8, 3
	sw	x7, x13, 88
	lw	x15, x7, 8
	lw	x16, x7, 12
	lw	x29, x7, 16
	lw	x18, x7, 20
	lw	x17, x7, 24
	srli	x8, x15, 27
	slli	x14, x15, 5
	or	x31, x8, x14
	and	x8, x29, x16
	xori	x14, x16, -1
	and	x14, x18, x14
	xor	x1, x14, x8
	lw	x14, x7, 28
	movimm 	x8, 	1518500249
	srli	x2, x16, 2
	slli	x16, x16, 30
	or	x16, x2, x16
	add	x17, x31, x17
	add	x17, x17, x1
	add	x17, x17, x14
	add	x31, x17, x8
	srli	x17, x31, 27
	slli	x1, x31, 5
	or	x1, x17, x1
	and	x17, x16, x15
	xori	x2, x15, -1
	and	x2, x29, x2
	xor	x2, x17, x2
	lw	x3, x7, 32
	srli	x17, x15, 2
	slli	x15, x15, 30
	or	x17, x17, x15
	add	x15, x18, x2
	add	x15, x15, x3
	add	x15, x15, x1
	add	x1, x15, x8
	srli	x15, x1, 27
	slli	x18, x1, 5
	or	x2, x15, x18
	and	x15, x31, x17
	xori	x18, x31, -1
	and	x18, x16, x18
	xor	x3, x15, x18
	lw	x15, x7, 36
	srli	x18, x31, 2
	slli	x31, x31, 30
	or	x18, x18, x31
	add	x29, x29, x15
	add	x29, x29, x3
	add	x29, x29, x2
	add	x31, x29, x8
	srli	x29, x31, 27
	slli	x2, x31, 5
	or	x2, x29, x2
	and	x29, x1, x18
	xori	x3, x1, -1
	and	x3, x17, x3
	xor	x3, x29, x3
	lw	x4, x7, 40
	srli	x29, x1, 2
	slli	x1, x1, 30
	or	x29, x29, x1
	add	x16, x16, x4
	add	x16, x16, x3
	add	x16, x16, x2
	add	x1, x16, x8
	srli	x16, x1, 27
	slli	x2, x1, 5
	or	x2, x16, x2
	and	x16, x31, x29
	xori	x3, x31, -1
	and	x3, x18, x3
	xor	x3, x16, x3
	lw	x4, x7, 44
	srli	x16, x31, 2
	slli	x31, x31, 30
	or	x16, x16, x31
	add	x17, x17, x4
	add	x17, x17, x3
	add	x17, x17, x2
	add	x31, x17, x8
	srli	x17, x31, 27
	slli	x2, x31, 5
	or	x2, x17, x2
	and	x17, x1, x16
	xori	x3, x1, -1
	and	x3, x29, x3
	xor	x3, x17, x3
	lw	x4, x7, 48
	srli	x17, x1, 2
	slli	x1, x1, 30
	or	x17, x17, x1
	add	x18, x18, x4
	add	x18, x18, x3
	add	x18, x18, x2
	add	x1, x18, x8
	srli	x18, x1, 27
	slli	x2, x1, 5
	or	x2, x18, x2
	and	x18, x31, x17
	xori	x3, x31, -1
	and	x3, x16, x3
	xor	x3, x18, x3
	lw	x4, x7, 52
	srli	x18, x31, 2
	slli	x31, x31, 30
	or	x18, x18, x31
	add	x29, x4, x29
	add	x29, x29, x3
	add	x29, x29, x2
	add	x31, x29, x8
	srli	x29, x31, 27
	slli	x2, x31, 5
	or	x2, x29, x2
	and	x29, x1, x18
	xori	x3, x1, -1
	and	x3, x17, x3
	xor	x3, x29, x3
	lw	x4, x7, 56
	srli	x29, x1, 2
	slli	x1, x1, 30
	or	x29, x29, x1
	add	x16, x4, x16
	add	x16, x16, x3
	add	x16, x16, x2
	add	x1, x16, x8
	srli	x16, x1, 27
	slli	x2, x1, 5
	or	x2, x16, x2
	and	x16, x31, x29
	xori	x3, x31, -1
	and	x3, x18, x3
	xor	x3, x16, x3
	lw	x16, x7, 60
	srli	x4, x31, 2
	slli	x31, x31, 30
	or	x31, x4, x31
	add	x17, x16, x17
	add	x17, x17, x3
	add	x17, x17, x2
	add	x2, x17, x8
	srli	x17, x2, 27
	slli	x3, x2, 5
	or	x3, x17, x3
	and	x17, x1, x31
	xori	x4, x1, -1
	and	x4, x29, x4
	xor	x4, x17, x4
	lw	x9, x7, 64
	srli	x17, x1, 2
	slli	x1, x1, 30
	or	x17, x17, x1
	add	x18, x9, x18
	add	x18, x18, x4
	add	x18, x18, x3
	add	x18, x18, x8
	srli	x1, x18, 27
	slli	x3, x18, 5
	or	x3, x1, x3
	and	x1, x2, x17
	xori	x4, x2, -1
	and	x4, x31, x4
	xor	x4, x1, x4
	lw	x9, x7, 68
	srli	x1, x2, 2
	slli	x2, x2, 30
	or	x1, x1, x2
	add	x29, x9, x29
	add	x29, x29, x4
	add	x29, x29, x3
	add	x29, x29, x8
	srli	x2, x29, 27
	slli	x3, x29, 5
	or	x3, x2, x3
	and	x2, x18, x1
	xori	x4, x18, -1
	and	x4, x17, x4
	xor	x4, x2, x4
	lw	x9, x7, 72
	srli	x2, x18, 2
	slli	x18, x18, 30
	or	x2, x2, x18
	add	x18, x9, x31
	add	x18, x18, x4
	add	x18, x18, x3
	add	x18, x18, x8
	srli	x31, x18, 27
	slli	x3, x18, 5
	or	x31, x31, x3
	and	x3, x29, x2
	xori	x4, x29, -1
	and	x4, x1, x4
	xor	x3, x3, x4
	lw	x9, x7, 76
	srli	x4, x29, 2
	slli	x29, x29, 30
	or	x4, x4, x29
	add	x17, x9, x17
	add	x17, x17, x3
	add	x17, x17, x31
	add	x17, x17, x8
	srli	x29, x17, 27
	slli	x31, x17, 5
	or	x29, x29, x31
	and	x31, x18, x4
	xori	x3, x18, -1
	and	x3, x2, x3
	xor	x31, x31, x3
	lw	x3, x7, 80
	srli	x9, x18, 2
	slli	x18, x18, 30
	or	x18, x9, x18
	add	x1, x3, x1
	add	x31, x1, x31
	add	x29, x31, x29
	add	x1, x29, x8
	srli	x29, x1, 27
	slli	x31, x1, 5
	or	x9, x29, x31
	and	x29, x17, x18
	xori	x31, x17, -1
	and	x31, x4, x31
	xor	x19, x29, x31
	lw	x31, x7, 84
	srli	x29, x17, 2
	slli	x17, x17, 30
	or	x29, x29, x17
	add	x17, x31, x2
	add	x17, x17, x19
	add	x17, x17, x9
	add	x2, x17, x8
	srli	x17, x2, 27
	slli	x9, x2, 5
	or	x9, x17, x9
	and	x17, x1, x29
	xori	x19, x1, -1
	and	x19, x18, x19
	xor	x19, x17, x19
	srli	x17, x1, 2
	slli	x1, x1, 30
	or	x17, x17, x1
	add	x13, x13, x4
	add	x13, x13, x19
	add	x13, x13, x9
	add	x1, x13, x8
	srli	x13, x1, 27
	slli	x4, x1, 5
	or	x4, x13, x4
	and	x13, x2, x17
	xori	x9, x2, -1
	and	x9, x29, x9
	xor	x9, x13, x9
	xor	x13, x15, x14
	xor	x13, x13, x16
	xor	x13, x13, x3
	slli	x14, x13, 1
	srli	x13, x13, 31
	or	x15, x14, x13
	sw	x7, x15, 28
	srli	x13, x2, 2
	slli	x14, x2, 30
	or	x13, x13, x14
	add	x14, x15, x18
	add	x14, x14, x9
	add	x14, x14, x4
	add	x16, x14, x8
	srli	x14, x16, 27
	slli	x18, x16, 5
	or	x2, x14, x18
	and	x14, x1, x13
	xori	x18, x1, -1
	and	x18, x17, x18
	xor	x3, x14, x18
	lw	x14, x7, 64
	xor	x14, x14, x31
	lw	x31, x7, 40
	xor	x14, x14, x31
	lw	x18, x7, 32
	xor	x14, x14, x18
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 32
	srli	x14, x1, 2
	slli	x1, x1, 30
	or	x14, x14, x1
	add	x29, x18, x29
	add	x29, x29, x3
	add	x29, x29, x2
	add	x3, x29, x8
	srli	x29, x3, 27
	slli	x1, x3, 5
	or	x2, x29, x1
	and	x29, x16, x14
	xori	x1, x16, -1
	and	x1, x13, x1
	xor	x4, x29, x1
	lw	x29, x7, 88
	lw	x1, x7, 68
	xor	x29, x1, x29
	lw	x1, x7, 44
	xor	x29, x29, x1
	lw	x9, x7, 36
	xor	x29, x29, x9
	slli	x9, x29, 1
	srli	x29, x29, 31
	or	x29, x9, x29
	sw	x7, x29, 36
	srli	x9, x16, 2
	slli	x16, x16, 30
	or	x16, x9, x16
	add	x17, x29, x17
	add	x17, x17, x4
	add	x17, x17, x2
	add	x4, x17, x8
	srli	x17, x4, 27
	slli	x2, x4, 5
	or	x9, x17, x2
	and	x17, x3, x16
	xori	x2, x3, -1
	and	x2, x14, x2
	xor	x19, x17, x2
	lw	x17, x7, 72
	lw	x2, x7, 48
	xor	x15, x31, x15
	xor	x15, x15, x17
	xor	x15, x15, x2
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 40
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x8, x3, 27
	slli	x13, x3, 5
	or	x9, x8, x13
	xor	x8, x15, x16
	xor	x19, x8, x4
	lw	x8, x7, 76
	lw	x31, x7, 52
	xor	x13, x1, x18
	xor	x8, x13, x8
	xor	x8, x8, x31
	slli	x13, x8, 1
	srli	x8, x8, 31
	or	x18, x13, x8
	sw	x7, x18, 44
	movimm 	x8, 	1859775393
	srli	x13, x4, 2
	slli	x1, x4, 30
	or	x13, x13, x1
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x1, x4, 5
	or	x9, x14, x1
	xor	x14, x13, x15
	xor	x19, x14, x3
	lw	x14, x7, 80
	lw	x1, x7, 56
	xor	x29, x29, x2
	xor	x14, x29, x14
	xor	x14, x14, x1
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 48
	srli	x14, x3, 2
	slli	x2, x3, 30
	or	x14, x14, x2
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x2, x3, 5
	or	x9, x16, x2
	xor	x16, x14, x13
	xor	x19, x16, x4
	lw	x16, x7, 84
	lw	x2, x7, 60
	xor	x17, x17, x31
	xor	x16, x17, x16
	xor	x16, x16, x2
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 52
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x16, x14
	xor	x19, x15, x3
	lw	x15, x7, 88
	lw	x31, x7, 64
	xor	x18, x18, x1
	xor	x15, x18, x15
	xor	x15, x15, x31
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 56
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x1, x3, 5
	or	x9, x13, x1
	xor	x13, x15, x16
	xor	x19, x13, x4
	lw	x13, x7, 28
	lw	x1, x7, 68
	xor	x29, x29, x2
	xor	x13, x29, x13
	xor	x13, x13, x1
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 60
	srli	x13, x4, 2
	slli	x2, x4, 30
	or	x13, x13, x2
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x2, x4, 5
	or	x9, x14, x2
	xor	x14, x13, x15
	xor	x19, x14, x3
	lw	x14, x7, 32
	lw	x2, x7, 72
	xor	x17, x17, x31
	xor	x14, x17, x14
	xor	x14, x14, x2
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x7, x17, 64
	srli	x14, x3, 2
	slli	x31, x3, 30
	or	x14, x14, x31
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x31, x3, 5
	or	x9, x16, x31
	xor	x16, x14, x13
	xor	x19, x16, x4
	lw	x16, x7, 36
	lw	x31, x7, 76
	xor	x18, x18, x1
	xor	x16, x18, x16
	xor	x16, x16, x31
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 68
	srli	x16, x4, 2
	slli	x1, x4, 30
	or	x16, x16, x1
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x16, x14
	xor	x19, x15, x3
	lw	x15, x7, 40
	lw	x1, x7, 80
	xor	x29, x29, x2
	xor	x15, x29, x15
	xor	x15, x15, x1
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 72
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x2, x3, 5
	or	x9, x13, x2
	xor	x13, x15, x16
	xor	x19, x13, x4
	lw	x13, x7, 44
	lw	x2, x7, 84
	xor	x17, x17, x31
	xor	x13, x17, x13
	xor	x13, x13, x2
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 76
	srli	x13, x4, 2
	slli	x31, x4, 30
	or	x13, x13, x31
	add	x14, x17, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x31, x4, 5
	or	x9, x14, x31
	xor	x14, x13, x15
	xor	x19, x14, x3
	lw	x14, x7, 48
	lw	x31, x7, 88
	xor	x18, x18, x1
	xor	x14, x18, x14
	xor	x14, x14, x31
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 80
	srli	x14, x3, 2
	slli	x1, x3, 30
	or	x14, x14, x1
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x14, x13
	xor	x19, x16, x4
	lw	x16, x7, 52
	lw	x1, x7, 28
	xor	x29, x29, x2
	xor	x16, x29, x16
	xor	x16, x16, x1
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 84
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x2, x4, 5
	or	x9, x15, x2
	xor	x15, x16, x14
	xor	x19, x15, x3
	lw	x15, x7, 56
	lw	x2, x7, 32
	xor	x17, x17, x31
	xor	x15, x17, x15
	xor	x15, x15, x2
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 88
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x31, x3, 5
	or	x9, x13, x31
	xor	x13, x15, x16
	xor	x19, x13, x4
	lw	x13, x7, 60
	lw	x31, x7, 36
	xor	x18, x18, x1
	xor	x13, x18, x13
	xor	x13, x13, x31
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 28
	srli	x13, x4, 2
	slli	x1, x4, 30
	or	x13, x13, x1
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x1, x4, 5
	or	x9, x14, x1
	xor	x14, x13, x15
	xor	x19, x14, x3
	lw	x14, x7, 64
	lw	x1, x7, 40
	xor	x29, x29, x2
	xor	x14, x29, x14
	xor	x14, x14, x1
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 32
	srli	x14, x3, 2
	slli	x2, x3, 30
	or	x14, x14, x2
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x2, x3, 5
	or	x9, x16, x2
	xor	x16, x14, x13
	xor	x19, x16, x4
	lw	x16, x7, 68
	lw	x2, x7, 44
	xor	x17, x17, x31
	xor	x16, x17, x16
	xor	x16, x16, x2
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x31, x17, x16
	sw	x7, x31, 36
	srli	x16, x4, 2
	slli	x17, x4, 30
	or	x16, x16, x17
	add	x15, x31, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x17, x4, 5
	or	x9, x15, x17
	xor	x15, x16, x14
	xor	x19, x15, x3
	lw	x17, x7, 72
	lw	x15, x7, 48
	xor	x18, x18, x1
	xor	x17, x18, x17
	xor	x17, x17, x15
	slli	x18, x17, 1
	srli	x17, x17, 31
	or	x1, x18, x17
	sw	x7, x1, 40
	srli	x17, x3, 2
	slli	x18, x3, 30
	or	x17, x17, x18
	add	x13, x1, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x9, x13, x8
	srli	x13, x9, 27
	slli	x18, x9, 5
	or	x19, x13, x18
	xor	x13, x17, x16
	xor	x20, x13, x4
	lw	x13, x7, 76
	lw	x3, x7, 52
	xor	x18, x29, x2
	xor	x13, x18, x13
	xor	x13, x13, x3
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x13, x18, x13
	sw	x7, x13, 44
	srli	x18, x4, 2
	slli	x29, x4, 30
	or	x18, x18, x29
	add	x14, x13, x14
	add	x14, x14, x20
	add	x14, x14, x19
	add	x19, x14, x8
	srli	x14, x19, 27
	slli	x29, x19, 5
	or	x29, x14, x29
	xor	x14, x18, x17
	xor	x2, x14, x9
	lw	x14, x7, 80
	lw	x4, x7, 56
	xor	x15, x31, x15
	xor	x14, x15, x14
	xor	x14, x14, x4
	slli	x15, x14, 1
	srli	x14, x14, 31
	or	x14, x15, x14
	sw	x7, x14, 48
	srli	x15, x9, 2
	slli	x31, x9, 30
	or	x15, x15, x31
	add	x16, x14, x16
	add	x16, x16, x2
	add	x16, x16, x29
	add	x20, x16, x8
	srli	x16, x20, 27
	slli	x29, x20, 5
	or	x31, x16, x29
	xor	x16, x15, x18
	xor	x9, x16, x19
	lw	x16, x7, 84
	lw	x2, x7, 60
	xor	x29, x1, x3
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 52
	srli	x16, x19, 2
	slli	x1, x19, 30
	or	x16, x16, x1
	add	x17, x29, x17
	add	x17, x17, x9
	add	x17, x17, x31
	add	x9, x17, x8
	srli	x17, x9, 27
	slli	x31, x9, 5
	or	x1, x17, x31
	xor	x17, x16, x15
	xor	x3, x17, x20
	lw	x17, x7, 88
	lw	x31, x7, 64
	xor	x13, x13, x4
	xor	x13, x13, x17
	xor	x13, x13, x31
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 56
	srli	x13, x20, 2
	slli	x4, x20, 30
	or	x13, x13, x4
	add	x18, x17, x18
	add	x18, x18, x3
	add	x18, x18, x1
	add	x3, x18, x8
	srli	x8, x3, 27
	slli	x18, x3, 5
	or	x4, x8, x18
	xor	x8, x13, x16
	and	x8, x9, x8
	and	x18, x13, x16
	xor	x19, x8, x18
	lw	x8, x7, 28
	lw	x1, x7, 68
	xor	x14, x14, x2
	xor	x8, x14, x8
	xor	x8, x8, x1
	slli	x14, x8, 1
	srli	x8, x8, 31
	or	x18, x14, x8
	sw	x7, x18, 60
	movimm 	x8, 	-1894007588
	srli	x14, x9, 2
	slli	x2, x9, 30
	or	x14, x14, x2
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x4
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x2, x4, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x2, x14, x13
	xor	x19, x15, x2
	lw	x15, x7, 32
	lw	x2, x7, 72
	xor	x29, x29, x31
	xor	x15, x29, x15
	xor	x15, x15, x2
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 64
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x31, x3, 5
	or	x9, x16, x31
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x31, x15, x14
	xor	x19, x16, x31
	lw	x16, x7, 36
	lw	x31, x7, 76
	xor	x17, x17, x1
	xor	x16, x17, x16
	xor	x16, x16, x31
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 68
	srli	x16, x4, 2
	slli	x1, x4, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x1, x4, 5
	or	x9, x13, x1
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x1, x16, x15
	xor	x19, x13, x1
	lw	x13, x7, 40
	lw	x1, x7, 80
	xor	x18, x18, x2
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 72
	srli	x13, x3, 2
	slli	x2, x3, 30
	or	x13, x13, x2
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x2, x3, 5
	or	x9, x14, x2
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x2, x13, x16
	xor	x19, x14, x2
	lw	x14, x7, 44
	lw	x2, x7, 84
	xor	x29, x29, x31
	xor	x14, x29, x14
	xor	x14, x14, x2
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 76
	srli	x14, x4, 2
	slli	x31, x4, 30
	or	x14, x14, x31
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x31, x14, x13
	xor	x19, x15, x31
	lw	x15, x7, 48
	lw	x31, x7, 88
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x31
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 80
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x1, x15, x14
	xor	x19, x16, x1
	lw	x16, x7, 52
	lw	x1, x7, 28
	xor	x18, x18, x2
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 84
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x2, x4, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x2, x16, x15
	xor	x19, x13, x2
	lw	x13, x7, 56
	lw	x2, x7, 32
	xor	x29, x29, x31
	xor	x13, x29, x13
	xor	x13, x13, x2
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 88
	srli	x13, x3, 2
	slli	x31, x3, 30
	or	x13, x13, x31
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x31, x3, 5
	or	x9, x14, x31
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x31, x13, x16
	xor	x19, x14, x31
	lw	x14, x7, 60
	lw	x31, x7, 36
	xor	x17, x17, x1
	xor	x14, x17, x14
	xor	x14, x14, x31
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x7, x17, 28
	srli	x14, x4, 2
	slli	x1, x4, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x1, x14, x13
	xor	x19, x15, x1
	lw	x15, x7, 64
	lw	x1, x7, 40
	xor	x18, x18, x2
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 32
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x2, x3, 5
	or	x9, x16, x2
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x2, x15, x14
	xor	x19, x16, x2
	lw	x16, x7, 68
	lw	x2, x7, 44
	xor	x29, x29, x31
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 36
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x31, x4, 5
	or	x9, x13, x31
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x31, x16, x15
	xor	x19, x13, x31
	lw	x13, x7, 72
	lw	x31, x7, 48
	xor	x17, x17, x1
	xor	x13, x17, x13
	xor	x13, x13, x31
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 40
	srli	x13, x3, 2
	slli	x1, x3, 30
	or	x13, x13, x1
	add	x14, x17, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x1, x3, 5
	or	x9, x14, x1
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x1, x13, x16
	xor	x19, x14, x1
	lw	x14, x7, 76
	lw	x1, x7, 52
	xor	x18, x18, x2
	xor	x14, x18, x14
	xor	x14, x14, x1
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 44
	srli	x14, x4, 2
	slli	x2, x4, 30
	or	x14, x14, x2
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x2, x4, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x2, x14, x13
	xor	x19, x15, x2
	lw	x15, x7, 80
	lw	x2, x7, 56
	xor	x29, x29, x31
	xor	x15, x29, x15
	xor	x15, x15, x2
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 48
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x31, x3, 5
	or	x9, x16, x31
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x31, x15, x14
	xor	x19, x16, x31
	lw	x16, x7, 84
	lw	x31, x7, 60
	xor	x17, x17, x1
	xor	x16, x17, x16
	xor	x16, x16, x31
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 52
	srli	x16, x4, 2
	slli	x1, x4, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x1, x4, 5
	or	x9, x13, x1
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x1, x16, x15
	xor	x19, x13, x1
	lw	x13, x7, 88
	lw	x1, x7, 64
	xor	x18, x18, x2
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 56
	srli	x13, x3, 2
	slli	x2, x3, 30
	or	x13, x13, x2
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x2, x3, 5
	or	x9, x14, x2
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x2, x13, x16
	xor	x19, x14, x2
	lw	x14, x7, 28
	lw	x2, x7, 68
	xor	x29, x29, x31
	xor	x14, x29, x14
	xor	x14, x14, x2
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 60
	srli	x14, x4, 2
	slli	x31, x4, 30
	or	x14, x14, x31
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x31, x14, x13
	xor	x19, x15, x31
	lw	x15, x7, 32
	lw	x31, x7, 72
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x31
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 64
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x1, x15, x14
	xor	x19, x16, x1
	lw	x16, x7, 36
	lw	x1, x7, 76
	xor	x18, x18, x2
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 68
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x2, x4, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x2, x16, x15
	xor	x19, x13, x2
	lw	x13, x7, 40
	lw	x2, x7, 80
	xor	x29, x29, x31
	xor	x13, x29, x13
	xor	x13, x13, x2
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 72
	srli	x13, x3, 2
	slli	x31, x3, 30
	or	x13, x13, x31
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x8, x3, 27
	slli	x14, x3, 5
	or	x9, x8, x14
	xor	x8, x13, x16
	xor	x19, x8, x4
	lw	x8, x7, 44
	lw	x31, x7, 84
	xor	x14, x17, x1
	xor	x8, x14, x8
	xor	x8, x8, x31
	slli	x14, x8, 1
	srli	x8, x8, 31
	or	x17, x14, x8
	sw	x7, x17, 76
	movimm 	x8, 	-899497514
	srli	x14, x4, 2
	slli	x1, x4, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 48
	lw	x1, x7, 88
	xor	x18, x18, x2
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 80
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x2, x3, 5
	or	x9, x16, x2
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 52
	lw	x2, x7, 28
	xor	x29, x29, x31
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 84
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x31, x4, 5
	or	x9, x13, x31
	xor	x13, x16, x15
	xor	x19, x13, x3
	lw	x13, x7, 56
	lw	x31, x7, 32
	xor	x17, x17, x1
	xor	x13, x17, x13
	xor	x13, x13, x31
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 88
	srli	x13, x3, 2
	slli	x1, x3, 30
	or	x13, x13, x1
	add	x14, x17, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x1, x3, 5
	or	x9, x14, x1
	xor	x14, x13, x16
	xor	x19, x14, x4
	lw	x14, x7, 60
	lw	x1, x7, 36
	xor	x18, x18, x2
	xor	x14, x18, x14
	xor	x14, x14, x1
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 28
	srli	x14, x4, 2
	slli	x2, x4, 30
	or	x14, x14, x2
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x2, x4, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 64
	lw	x2, x7, 40
	xor	x29, x29, x31
	xor	x15, x29, x15
	xor	x15, x15, x2
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 32
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x31, x3, 5
	or	x9, x16, x31
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 68
	lw	x31, x7, 44
	xor	x17, x17, x1
	xor	x16, x17, x16
	xor	x16, x16, x31
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 36
	srli	x16, x4, 2
	slli	x1, x4, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x1, x4, 5
	or	x9, x13, x1
	xor	x13, x16, x15
	xor	x19, x13, x3
	lw	x13, x7, 72
	lw	x1, x7, 48
	xor	x18, x18, x2
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 40
	srli	x13, x3, 2
	slli	x2, x3, 30
	or	x13, x13, x2
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x2, x3, 5
	or	x9, x14, x2
	xor	x14, x13, x16
	xor	x19, x14, x4
	lw	x14, x7, 76
	lw	x2, x7, 52
	xor	x29, x29, x31
	xor	x14, x29, x14
	xor	x14, x14, x2
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 44
	srli	x14, x4, 2
	slli	x31, x4, 30
	or	x14, x14, x31
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 80
	lw	x31, x7, 56
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x31
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 48
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 84
	lw	x1, x7, 60
	xor	x18, x18, x2
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 52
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x2, x4, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	xor	x19, x13, x3
	lw	x13, x7, 88
	lw	x2, x7, 64
	xor	x29, x29, x31
	xor	x13, x29, x13
	xor	x13, x13, x2
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 56
	srli	x13, x3, 2
	slli	x31, x3, 30
	or	x13, x13, x31
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x31, x3, 5
	or	x9, x14, x31
	xor	x14, x13, x16
	xor	x19, x14, x4
	lw	x14, x7, 28
	lw	x31, x7, 68
	xor	x17, x17, x1
	xor	x14, x17, x14
	xor	x14, x14, x31
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x7, x17, 60
	srli	x14, x4, 2
	slli	x1, x4, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 32
	lw	x1, x7, 72
	xor	x18, x18, x2
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 64
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x9, x16, x8
	srli	x16, x9, 27
	slli	x2, x9, 5
	or	x3, x16, x2
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 36
	lw	x2, x7, 76
	xor	x29, x29, x31
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 68
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x3
	add	x13, x13, x8
	srli	x31, x13, 27
	slli	x3, x13, 5
	or	x4, x31, x3
	xor	x31, x16, x15
	xor	x19, x31, x9
	lw	x31, x7, 40
	lw	x3, x7, 80
	xor	x17, x17, x1
	xor	x17, x17, x31
	xor	x17, x17, x3
	slli	x31, x17, 1
	srli	x17, x17, 31
	or	x31, x31, x17
	sw	x7, x31, 72
	srli	x17, x9, 2
	slli	x1, x9, 30
	or	x17, x17, x1
	add	x14, x31, x14
	add	x14, x14, x19
	add	x14, x14, x4
	add	x14, x14, x8
	srli	x1, x14, 27
	slli	x4, x14, 5
	or	x4, x1, x4
	xor	x1, x17, x16
	xor	x9, x1, x13
	lw	x19, x7, 44
	lw	x1, x7, 84
	xor	x18, x18, x2
	xor	x18, x18, x19
	xor	x18, x18, x1
	slli	x2, x18, 1
	srli	x18, x18, 31
	or	x18, x2, x18
	sw	x7, x18, 76
	srli	x2, x13, 2
	slli	x13, x13, 30
	or	x13, x2, x13
	add	x15, x18, x15
	add	x15, x15, x9
	add	x15, x15, x4
	add	x2, x15, x8
	srli	x15, x2, 27
	slli	x4, x2, 5
	or	x4, x15, x4
	xor	x15, x13, x17
	xor	x9, x15, x14
	lw	x19, x7, 48
	lw	x15, x7, 88
	xor	x29, x29, x3
	xor	x29, x29, x19
	xor	x29, x29, x15
	slli	x3, x29, 1
	srli	x29, x29, 31
	or	x29, x3, x29
	sw	x7, x29, 80
	srli	x3, x14, 2
	slli	x14, x14, 30
	or	x14, x3, x14
	add	x16, x29, x16
	add	x16, x16, x9
	add	x16, x16, x4
	add	x16, x16, x8
	srli	x29, x16, 27
	slli	x3, x16, 5
	or	x3, x29, x3
	xor	x29, x14, x13
	xor	x4, x29, x2
	lw	x29, x7, 52
	lw	x9, x7, 28
	xor	x31, x31, x1
	xor	x29, x31, x29
	xor	x29, x29, x9
	slli	x31, x29, 1
	srli	x29, x29, 31
	or	x31, x31, x29
	sw	x7, x31, 84
	srli	x29, x2, 2
	slli	x1, x2, 30
	or	x29, x29, x1
	add	x17, x31, x17
	add	x17, x17, x4
	add	x17, x17, x3
	add	x17, x17, x8
	srli	x31, x17, 27
	slli	x1, x17, 5
	or	x31, x31, x1
	xor	x1, x29, x14
	xor	x1, x1, x16
	lw	x2, x7, 56
	lw	x3, x7, 32
	xor	x15, x18, x15
	xor	x15, x15, x2
	xor	x15, x15, x3
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 88
	srli	x15, x16, 2
	slli	x16, x16, 30
	or	x15, x15, x16
	lw	x16, x7, 8
	add	x16, x16, x18
	add	x13, x16, x13
	add	x13, x13, x1
	add	x13, x13, x31
	add	x8, x13, x8
	sw	x7, x8, 8
	lw	x8, x7, 12
	add	x8, x17, x8
	sw	x7, x8, 12
	lw	x8, x7, 16
	add	x8, x15, x8
	sw	x7, x8, 16
	lw	x8, x7, 20
	add	x8, x29, x8
	sw	x7, x8, 20
	lw	x8, x7, 24
	add	x8, x14, x8
	sw	x7, x8, 24
	addi	x8, x0, 0
.LBB7_120:
	addi	x7, x8, 0
	addi	x8, x30, 64
	andi	x13, x7, -4
	add	x8, x8, x13
	lw	x8, x8, 8
	slli	x13, x7, 3
	andi	x13, x13, 24
	xori	x13, x13, 24
	srl	x13, x8, x13
	addi	x8, x7, 1
	movimm 	x14, 	20
	add	x7, x6, x7
	sb	x7, x13, 12
	bne	x8, x14, .LBB7_120
	jal x0,	.LBB7_130
.LBB7_130:
	sub	x7, x10, x11
	lw	x11, x6, 0
	addi	x8, x30, 160
	sw	x8, x11, 0
	lw	x11, x6, 4
	sw	x8, x11, 4
	lw	x11, x6, 8
	sw	x8, x11, 8
	lbu	x11, x6, 12
	lbu	x13, x6, 13
	lbu	x14, x6, 14
	lbu	x15, x6, 15
	lbu	x16, x6, 16
	lbu	x17, x6, 17
	lbu	x18, x6, 18
	lbu	x29, x6, 19
	lbu	x31, x6, 20
	lbu	x1, x6, 21
	lbu	x2, x6, 22
	lbu	x3, x6, 23
	lbu	x4, x6, 24
	lbu	x9, x6, 25
	lbu	x10, x6, 26
	lbu	x19, x6, 27
	lbu	x20, x6, 28
	lbu	x21, x6, 29
	lbu	x22, x6, 30
	lbu	x6, x6, 31
	sb	x8, x11, 12
	sb	x8, x13, 13
	sb	x8, x14, 14
	sb	x8, x15, 15
	sb	x8, x16, 16
	sb	x8, x17, 17
	sb	x8, x18, 18
	sb	x8, x29, 19
	sb	x8, x31, 20
	sb	x8, x1, 21
	sb	x8, x2, 22
	sb	x8, x3, 23
	sb	x8, x4, 24
	sb	x8, x9, 25
	sb	x8, x10, 26
	sb	x8, x19, 27
	sb	x8, x20, 28
	sb	x8, x21, 29
	sb	x8, x22, 30
	sb	x8, x6, 31
	falloc	x6, x0
	movaddr 	x8, 	.HyOp#6
	fbind 	x8, 	x6
	falloc	x8, x0
	movaddr 	x11, 	.HyOp#14
	fbind 	x11, 	x8
	movaddr 	x11, 	"ga#44"
	movimm 	x13, 	192
	srli	x14, x8, 22
	srli	x15, x8, 12
	andi	x15, x15, 15
	srli	x16, x8, 6
	andi	x16, x16, 63
	addi	x17, x0, 832
	mul	x14, x14, x17
	addi	x17, x0, 52
	mul	x15, x15, x17
	add	x15, x15, x16
	add	x14, x15, x14
	mul	x13, x14, x13
	add	x11, x11, x13
	addi	x13, x0, 0
.LBB7_140:
	addi	x14, x30, 160
	add	x14, x14, x13
	lbu	x14, x14, 0
	add	x15, x11, x13
	addi	x13, x13, 1
	movimm 	x16, 	32
	sb	x15, x14, 0
	bne	x13, x16, .LBB7_140
	jal x0,	.LBB7_150
.LBB7_150:
	writecm	x8, x6, 0
	writecm	x6, x7, 20
	writecm	x6, x12, 16
	end x0
;uts_function111111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#12
		.anno	P
		.numphy	1
		.numOperand	1
		.pHyPC	.PC_uts_function1111110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_uts_function1111110
.PC_uts_function1111110:
	JAL x0, .LBB8_00
.LBB8_00:
	movimm 	x6, 	1
	writecm	x11, x6, 24
	end x0
;uts_function121111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#13
		.anno	P
		.numphy	1
		.numOperand	3
		.pHyPC	.PC_uts_function1211110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_uts_function1211110
.PC_uts_function1211110:
	JAL x0, .LBB9_00
.LBB9_00:
	slt	x6, x0, x13
	slli	x7, x11, 6
	add	x7, x7, x12
	addi	x7, x7, -64
	addi	x8, x0, 0
.LBB9_10:
	writecm	x12, x8, 12
	writecmp	x12, x6
	addi	x12, x12, 64
	addi	x8, x8, 1
	bgeu	x7, x12, .LBB9_10
	jal x0,	.LBB9_20
.LBB9_20:
	end x0
;uts_function131111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#14
		.numphy	1
		.numOperand	1
		.pHyPC	.PC_uts_function1311110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_uts_function1311110
.PC_uts_function1311110:
	JAL x0, .LBB10_00
.LBB10_00:
	add		x30, x0, x10
	movaddr	x1, "ga#44"
	movimm	x5, 192
	add	x2, x0, x5
	srli	x3, x30, 22
	srli	x4, x30, 12
	andi	x5, x4, 15
	srli	x6, x30, 6
	andi	x7, x6, 63
	addi	x8, x0, 832
	mul		x9, x3, x8
	addi	x26, x0, 52
	mul		x27, x5, x26
	add		x28, x27, x7
	add		x29, x28, x9
	mul		x31, x29, x2
	add		x30, x1, x31
	addi	x7, x30, 0
	lw	x6, x7, 0
	addi	x14, x30, 32
	sw	x14, x6, 0
	lw	x6, x7, 4
	sw	x14, x6, 4
	lw	x8, x7, 8
	sw	x14, x8, 8
	lbu	x8, x7, 12
	movimm 	x15, 	1
	lbu	x12, x7, 13
	lbu	x16, x7, 14
	lbu	x17, x7, 15
	addi	x13, x7, 12
	lbu	x18, x13, 4
	lbu	x29, x13, 5
	lbu	x31, x13, 6
	lbu	x1, x13, 7
	lbu	x2, x13, 8
	lbu	x3, x13, 9
	lbu	x4, x13, 10
	lbu	x9, x13, 11
	lbu	x19, x13, 12
	lbu	x20, x13, 13
	lbu	x21, x13, 14
	lbu	x22, x13, 15
	sb	x14, x8, 12
	sb	x14, x12, 13
	sb	x14, x16, 14
	sb	x14, x17, 15
	sb	x14, x18, 16
	sb	x14, x29, 17
	sb	x14, x31, 18
	sb	x14, x1, 19
	sb	x14, x2, 20
	sb	x14, x3, 21
	sb	x14, x4, 22
	sb	x14, x9, 23
	sb	x14, x19, 24
	sb	x14, x20, 25
	sb	x14, x21, 26
	sb	x14, x22, 27
	lbu	x8, x13, 16
	sb	x14, x8, 28
	lbu	x12, x13, 17
	sb	x14, x12, 29
	lbu	x13, x13, 18
	sb	x14, x13, 30
	lbu	x7, x7, 31
	sb	x14, x7, 31
	movaddr 	x14, 	"ga#32"
	flw	f1, x14, 0
	bge	x6, x15, .LBB10_10
.LBB10_850:
	jal x0,	.LBB10_40
.LBB10_10:
	fcvt.s.w	f2, x6
	movaddr 	x14, 	"ga#8"
	lw	x14, x14, 0
	fcvt.s.w	f3, x14
	fmul.s	f5, f3, f3
	fmv.x.s	x14, f5
	srli	x14, x14, 1
	movimm 	x15, 	1597463007
	sub	x14, x15, x14
	fmv.s.x	f4, x14
	movimm 	x14, 	-1090519040
	fmv.s.x	f6, x14
	fmul.s	f5, f5, f6
	fmul.s	f5, f5, f4
	fmul.s	f5, f4, f5
	movimm 	x14, 	1069547520
	fmv.s.x	f6, x14
	fadd.s	f5, f5, f6
	fmul.s	f4, f4, f5
	fcvt.s.w	f5, x0
	fle.s	x14, f5, f3
	beq	x14, x0, .LBB10_20
.LBB10_840:
	jal x0,	.LBB10_30
.LBB10_20:
	fcvt.s.w	f3, x0
	fsub.s	f4, f3, f4
.LBB10_30:
	fmul.s	f2, f2, f4
	movimm 	x14, 	1065353216
	fmv.s.x	f3, x14
	fsub.s	f2, f3, f2
	fmul.s	f1, f1, f2
.LBB10_40:
	movimm 	x14, 	1065353216
	fmv.s.x	f2, x14
	fadd.s	f1, f1, f2
	fmul.s	f3, f1, f1
	fmv.x.s	x14, f3
	srli	x14, x14, 1
	movimm 	x15, 	1597463007
	sub	x14, x15, x14
	fmv.s.x	f2, x14
	movimm 	x14, 	-1090519040
	fmv.s.x	f4, x14
	fmul.s	f3, f3, f4
	fmul.s	f3, f3, f2
	fmul.s	f3, f2, f3
	movimm 	x14, 	1069547520
	fmv.s.x	f4, x14
	fadd.s	f3, f3, f4
	fmul.s	f2, f2, f3
	fcvt.s.w	f3, x0
	fle.s	x14, f3, f1
	beq	x14, x0, .LBB10_50
.LBB10_860:
	jal x0,	.LBB10_60
.LBB10_50:
	fcvt.s.w	f1, x0
	fsub.s	f2, f1, f2
.LBB10_60:
	slli	x8, x8, 24
	slli	x12, x12, 16
	slli	x13, x13, 8
	movimm 	x14, 	2130706432
	and	x8, x8, x14
	or	x8, x8, x12
	or	x8, x8, x13
	or	x7, x8, x7
	fcvt.s.w	f1, x7
	movimm 	x7, 	-1342205681
	fmv.s.x	f3, x7
	fmul.s	f1, f1, f3
	movimm 	x7, 	1065353216
	fmv.s.x	f3, x7
	fadd.s	f3, f1, f3
	fmv.x.s	x7, f3
	movimm 	x8, 	8388607
	bge	x8, x7, .LBB10_70
.LBB10_870:
	addi	x12, x0, 0
	jal x0,	.LBB10_110
.LBB10_70:
	movimm 	x8, 	2147483647
	and	x8, x7, x8
	movimm 	x12, 	-334920857
	fmv.s.x	f1, x12
	beq	x8, x0, .LBB10_280
	jal x0,	.LBB10_80
.LBB10_80:
	movimm 	x8, 	-1
	blt	x8, x7, .LBB10_100
	jal x0,	.LBB10_90
.LBB10_90:
	fsub.s	f1, f3, f3
	movimm 	x7, 	1602847591
	fmv.s.x	f3, x7
	fmul.s	f1, f1, f3
	fmv.s.x	f3, x7
	fmul.s	f1, f1, f3
	jal x0,	.LBB10_280
.LBB10_100:
	movimm 	x7, 	1275068416
	fmv.s.x	f1, x7
	fmul.s	f3, f3, f1
	fmv.x.s	x7, f3
	movimm 	x12, 	-25
.LBB10_110:
	movimm 	x8, 	2139095040
	blt	x7, x8, .LBB10_130
	jal x0,	.LBB10_120
.LBB10_120:
	fadd.s	f1, f3, f3
	jal x0,	.LBB10_280
.LBB10_130:
	srli	x13, x7, 23
	movimm 	x8, 	8388607
	and	x8, x7, x8
	movimm 	x14, 	4913952
	add	x14, x8, x14
	movimm 	x15, 	8388608
	and	x14, x14, x15
	or	x15, x14, x8
	movimm 	x16, 	1065353216
	xor	x15, x15, x16
	fmv.s.x	f1, x15
	srli	x14, x14, 23
	movimm 	x15, 	-1082130432
	fmv.s.x	f3, x15
	fadd.s	f1, f1, f3
	movimm 	x15, 	32768
	add	x7, x7, x15
	movimm 	x15, 	8372224
	and	x15, x7, x15
	add	x7, x13, x12
	add	x7, x7, x14
	addi	x7, x7, -127
	movimm 	x12, 	49151
	bltu	x12, x15, .LBB10_210
	jal x0,	.LBB10_140
.LBB10_140:
	fcvt.s.w	f3, x0
	feq.s	x8, f3, f1
	sltiu	x8, x8, 1
	bne	x8, x0, .LBB10_180
	jal x0,	.LBB10_150
.LBB10_150:
	bne	x7, x0, .LBB10_170
.LBB10_160:
	fcvt.s.w	f1, x0
	jal x0,	.LBB10_280
.LBB10_170:
	fcvt.s.w	f1, x7
	movimm 	x7, 	1060204928
	fmv.s.x	f3, x7
	fmul.s	f3, f1, f3
	movimm 	x7, 	924317649
	fmv.s.x	f4, x7
	fmul.s	f1, f1, f4
	fadd.s	f1, f3, f1
	jal x0,	.LBB10_280
.LBB10_180:
	fmul.s	f3, f1, f1
	movimm 	x8, 	-1096111445
	fmv.s.x	f4, x8
	fmul.s	f4, f1, f4
	movimm 	x8, 	1056964608
	fmv.s.x	f5, x8
	fadd.s	f4, f4, f5
	fmul.s	f3, f3, f4
	bne	x7, x0, .LBB10_200
	jal x0,	.LBB10_190
.LBB10_190:
	fsub.s	f1, f1, f3
	jal x0,	.LBB10_280
.LBB10_200:
	fcvt.s.w	f4, x7
	movimm 	x7, 	1060204928
	fmv.s.x	f5, x7
	fmul.s	f5, f4, f5
	movimm 	x7, 	-1223165999
	fmv.s.x	f6, x7
	fmul.s	f4, f4, f6
	fadd.s	f3, f3, f4
	fsub.s	f1, f3, f1
	fsub.s	f1, f5, f1
	jal x0,	.LBB10_280
.LBB10_210:
	movimm 	x12, 	1073741824
	fmv.s.x	f3, x12
	fadd.s	f3, f1, f3
	fmul.s	f3, f3, f3
	fmv.x.s	x12, f3
	srli	x13, x12, 1
	movimm 	x12, 	1
	movimm 	x14, 	1597463007
	sub	x13, x14, x13
	fmv.s.x	f4, x13
	movimm 	x13, 	-1090519040
	fmv.s.x	f5, x13
	fmul.s	f3, f3, f5
	fmul.s	f3, f3, f4
	fmul.s	f3, f4, f3
	movimm 	x13, 	1069547520
	fmv.s.x	f5, x13
	fadd.s	f3, f3, f5
	fmul.s	f3, f4, f3
	fmul.s	f4, f1, f3
	fcvt.s.w	f3, x7
	fmul.s	f5, f4, f4
	movimm 	x13, 	-3187664
	add	x13, x8, x13
	fmul.s	f6, f5, f5
	movimm 	x14, 	3523208
	sub	x8, x14, x8
	movimm 	x14, 	1048092198
	fmv.s.x	f7, x14
	fmul.s	f7, f6, f7
	movimm 	x14, 	1053609491
	fmv.s.x	f8, x14
	fadd.s	f7, f7, f8
	fmul.s	f7, f6, f7
	movimm 	x14, 	1049749998
	fmv.s.x	f8, x14
	fmul.s	f6, f6, f8
	movimm 	x14, 	1059760810
	fmv.s.x	f8, x14
	fadd.s	f6, f6, f8
	fmul.s	f5, f5, f6
	or	x8, x13, x8
	fadd.s	f5, f5, f7
	blt	x8, x12, .LBB10_250
	jal x0,	.LBB10_220
.LBB10_220:
	movimm 	x8, 	1056964608
	fmv.s.x	f6, x8
	fmul.s	f6, f1, f6
	fmul.s	f6, f1, f6
	bne	x7, x0, .LBB10_240
	jal x0,	.LBB10_230
.LBB10_230:
	fadd.s	f3, f6, f5
	fmul.s	f3, f4, f3
	fsub.s	f3, f6, f3
	fsub.s	f1, f1, f3
	jal x0,	.LBB10_280
.LBB10_240:
	movimm 	x7, 	1060204928
	fmv.s.x	f7, x7
	fmul.s	f7, f3, f7
	fadd.s	f5, f6, f5
	fmul.s	f4, f4, f5
	movimm 	x7, 	924317649
	fmv.s.x	f5, x7
	fmul.s	f3, f3, f5
	fadd.s	f3, f3, f4
	fsub.s	f3, f6, f3
	fsub.s	f1, f3, f1
	fsub.s	f1, f7, f1
	jal x0,	.LBB10_280
.LBB10_250:
	bne	x7, x0, .LBB10_270
	jal x0,	.LBB10_260
.LBB10_260:
	fsub.s	f3, f1, f5
	fmul.s	f3, f4, f3
	fsub.s	f1, f1, f3
	jal x0,	.LBB10_280
.LBB10_270:
	movimm 	x7, 	1060204928
	fmv.s.x	f6, x7
	fmul.s	f6, f3, f6
	fsub.s	f5, f1, f5
	fmul.s	f4, f4, f5
	movimm 	x7, 	-1223165999
	fmv.s.x	f5, x7
	fmul.s	f3, f3, f5
	fadd.s	f3, f4, f3
	fsub.s	f1, f3, f1
	fsub.s	f1, f6, f1
.LBB10_280:
	movimm 	x7, 	1065353216
	fmv.s.x	f3, x7
	fsub.s	f3, f3, f2
	fmv.x.s	x7, f3
	movimm 	x8, 	8388607
	bge	x8, x7, .LBB10_290
.LBB10_880:
	addi	x12, x0, 0
	jal x0,	.LBB10_330
.LBB10_290:
	movimm 	x8, 	2147483647
	and	x8, x7, x8
	movimm 	x12, 	-334920857
	fmv.s.x	f2, x12
	beq	x8, x0, .LBB10_500
	jal x0,	.LBB10_300
.LBB10_300:
	movimm 	x8, 	-1
	blt	x8, x7, .LBB10_320
	jal x0,	.LBB10_310
.LBB10_310:
	fsub.s	f2, f3, f3
	movimm 	x7, 	1602847591
	fmv.s.x	f3, x7
	fmul.s	f2, f2, f3
	fmv.s.x	f3, x7
	fmul.s	f2, f2, f3
	jal x0,	.LBB10_500
.LBB10_320:
	movimm 	x7, 	1275068416
	fmv.s.x	f2, x7
	fmul.s	f3, f3, f2
	fmv.x.s	x7, f3
	movimm 	x12, 	-25
.LBB10_330:
	movimm 	x8, 	2139095040
	blt	x7, x8, .LBB10_350
	jal x0,	.LBB10_340
.LBB10_340:
	fadd.s	f2, f3, f3
	jal x0,	.LBB10_500
.LBB10_350:
	srli	x13, x7, 23
	movimm 	x8, 	8388607
	and	x8, x7, x8
	movimm 	x14, 	4913952
	add	x14, x8, x14
	movimm 	x15, 	8388608
	and	x14, x14, x15
	or	x15, x14, x8
	movimm 	x16, 	1065353216
	xor	x15, x15, x16
	fmv.s.x	f2, x15
	srli	x14, x14, 23
	movimm 	x15, 	-1082130432
	fmv.s.x	f3, x15
	fadd.s	f2, f2, f3
	movimm 	x15, 	32768
	add	x7, x7, x15
	movimm 	x15, 	8372224
	and	x15, x7, x15
	add	x7, x13, x12
	add	x7, x7, x14
	addi	x7, x7, -127
	movimm 	x12, 	49151
	bltu	x12, x15, .LBB10_430
	jal x0,	.LBB10_360
.LBB10_360:
	fcvt.s.w	f3, x0
	feq.s	x8, f3, f2
	sltiu	x8, x8, 1
	bne	x8, x0, .LBB10_400
	jal x0,	.LBB10_370
.LBB10_370:
	bne	x7, x0, .LBB10_390
.LBB10_380:
	fcvt.s.w	f2, x0
	jal x0,	.LBB10_500
.LBB10_390:
	fcvt.s.w	f2, x7
	movimm 	x7, 	1060204928
	fmv.s.x	f3, x7
	fmul.s	f3, f2, f3
	movimm 	x7, 	924317649
	fmv.s.x	f4, x7
	fmul.s	f2, f2, f4
	fadd.s	f2, f3, f2
	jal x0,	.LBB10_500
.LBB10_400:
	fmul.s	f3, f2, f2
	movimm 	x8, 	-1096111445
	fmv.s.x	f4, x8
	fmul.s	f4, f2, f4
	movimm 	x8, 	1056964608
	fmv.s.x	f5, x8
	fadd.s	f4, f4, f5
	fmul.s	f3, f3, f4
	bne	x7, x0, .LBB10_420
	jal x0,	.LBB10_410
.LBB10_410:
	fsub.s	f2, f2, f3
	jal x0,	.LBB10_500
.LBB10_420:
	fcvt.s.w	f4, x7
	movimm 	x7, 	1060204928
	fmv.s.x	f5, x7
	fmul.s	f5, f4, f5
	movimm 	x7, 	-1223165999
	fmv.s.x	f6, x7
	fmul.s	f4, f4, f6
	fadd.s	f3, f3, f4
	fsub.s	f2, f3, f2
	fsub.s	f2, f5, f2
	jal x0,	.LBB10_500
.LBB10_430:
	movimm 	x12, 	1073741824
	fmv.s.x	f3, x12
	fadd.s	f3, f2, f3
	fmul.s	f3, f3, f3
	fmv.x.s	x12, f3
	srli	x13, x12, 1
	movimm 	x12, 	1
	movimm 	x14, 	1597463007
	sub	x13, x14, x13
	fmv.s.x	f4, x13
	movimm 	x13, 	-1090519040
	fmv.s.x	f5, x13
	fmul.s	f3, f3, f5
	fmul.s	f3, f3, f4
	fmul.s	f3, f4, f3
	movimm 	x13, 	1069547520
	fmv.s.x	f5, x13
	fadd.s	f3, f3, f5
	fmul.s	f3, f4, f3
	fmul.s	f4, f2, f3
	fcvt.s.w	f3, x7
	fmul.s	f5, f4, f4
	movimm 	x13, 	-3187664
	add	x13, x8, x13
	fmul.s	f6, f5, f5
	movimm 	x14, 	3523208
	sub	x8, x14, x8
	movimm 	x14, 	1048092198
	fmv.s.x	f7, x14
	fmul.s	f7, f6, f7
	movimm 	x14, 	1053609491
	fmv.s.x	f8, x14
	fadd.s	f7, f7, f8
	fmul.s	f7, f6, f7
	movimm 	x14, 	1049749998
	fmv.s.x	f8, x14
	fmul.s	f6, f6, f8
	movimm 	x14, 	1059760810
	fmv.s.x	f8, x14
	fadd.s	f6, f6, f8
	fmul.s	f5, f5, f6
	or	x8, x13, x8
	fadd.s	f5, f5, f7
	blt	x8, x12, .LBB10_470
	jal x0,	.LBB10_440
.LBB10_440:
	movimm 	x8, 	1056964608
	fmv.s.x	f6, x8
	fmul.s	f6, f2, f6
	fmul.s	f6, f2, f6
	bne	x7, x0, .LBB10_460
	jal x0,	.LBB10_450
.LBB10_450:
	fadd.s	f3, f6, f5
	fmul.s	f3, f4, f3
	fsub.s	f3, f6, f3
	fsub.s	f2, f2, f3
	jal x0,	.LBB10_500
.LBB10_460:
	movimm 	x7, 	1060204928
	fmv.s.x	f7, x7
	fmul.s	f7, f3, f7
	fadd.s	f5, f6, f5
	fmul.s	f4, f4, f5
	movimm 	x7, 	924317649
	fmv.s.x	f5, x7
	fmul.s	f3, f3, f5
	fadd.s	f3, f3, f4
	fsub.s	f3, f6, f3
	fsub.s	f2, f3, f2
	fsub.s	f2, f7, f2
	jal x0,	.LBB10_500
.LBB10_470:
	bne	x7, x0, .LBB10_490
	jal x0,	.LBB10_480
.LBB10_480:
	fsub.s	f3, f2, f5
	fmul.s	f3, f4, f3
	fsub.s	f2, f2, f3
	jal x0,	.LBB10_500
.LBB10_490:
	movimm 	x7, 	1060204928
	fmv.s.x	f6, x7
	fmul.s	f6, f3, f6
	fsub.s	f5, f2, f5
	fmul.s	f4, f4, f5
	movimm 	x7, 	-1223165999
	fmv.s.x	f5, x7
	fmul.s	f3, f3, f5
	fadd.s	f3, f4, f3
	fsub.s	f2, f3, f2
	fsub.s	f2, f6, f2
.LBB10_500:
	fmul.s	f4, f2, f2
	fmv.x.s	x7, f4
	srli	x7, x7, 1
	movimm 	x8, 	1597463007
	sub	x7, x8, x7
	fmv.s.x	f3, x7
	movimm 	x7, 	-1090519040
	fmv.s.x	f5, x7
	fmul.s	f4, f4, f5
	fmul.s	f4, f4, f3
	fmul.s	f4, f3, f4
	movimm 	x7, 	1069547520
	fmv.s.x	f5, x7
	fadd.s	f4, f4, f5
	fmul.s	f3, f3, f4
	fcvt.s.w	f4, x0
	fle.s	x7, f4, f2
	beq	x7, x0, .LBB10_510
.LBB10_890:
	jal x0,	.LBB10_520
.LBB10_510:
	fcvt.s.w	f2, x0
	fsub.s	f3, f2, f3
.LBB10_520:
	fmul.s	f1, f1, f3
	fmv.x.s	x7, f1
	srli	x8, x7, 23
	andi	x8, x8, 255
	addi	x8, x8, -127
	movimm 	x12, 	22
	blt	x12, x8, .LBB10_660
	jal x0,	.LBB10_530
.LBB10_530:
	movimm 	x12, 	-1
	blt	x12, x8, .LBB10_600
	jal x0,	.LBB10_540
.LBB10_540:
	movimm 	x8, 	1900671690
	fmv.s.x	f2, x8
	fadd.s	f1, f1, f2
	fcvt.s.w	f2, x0
	fle.s	x8, f1, f2
	beq	x8, x0, .LBB10_550
.LBB10_920:
	addi	x8, x7, 0
	jal x0,	.LBB10_650
.LBB10_550:
	movimm 	x12, 	-1
	addi	x8, x0, 0
	blt	x12, x7, .LBB10_650
	jal x0,	.LBB10_560
.LBB10_560:
	movimm 	x8, 	2147483647
	and	x8, x7, x8
	beq	x0, x8, .LBB10_590
.LBB10_570:
	movimm 	x7, 	-1082130432
	jal x0,	.LBB10_590
.LBB10_590:
	addi	x8, x7, 0
	jal x0,	.LBB10_650
.LBB10_600:
	movimm 	x12, 	8388607
	srl	x12, x12, x8
	and	x13, x12, x7
	bne	x13, x0, .LBB10_610
.LBB10_940:
	jal x0,	.LBB10_680
.LBB10_610:
	movimm 	x13, 	1900671690
	fmv.s.x	f2, x13
	fadd.s	f1, f1, f2
	fcvt.s.w	f2, x0
	fle.s	x13, f1, f2
	beq	x13, x0, .LBB10_620
.LBB10_910:
	addi	x8, x7, 0
	jal x0,	.LBB10_650
.LBB10_620:
	movimm 	x13, 	-1
	bge	x13, x7, .LBB10_630
.LBB10_900:
	jal x0,	.LBB10_640
.LBB10_630:
	movimm 	x13, 	8388608
	srl	x8, x13, x8
	add	x7, x8, x7
.LBB10_640:
	xori	x8, x12, -1
	and	x8, x7, x8
.LBB10_650:
	fmv.s.x	f1, x8
	jal x0,	.LBB10_680
.LBB10_660:
	movimm 	x7, 	128
	beq	x8, x7, .LBB10_670
.LBB10_930:
	jal x0,	.LBB10_680
.LBB10_670:
	fadd.s	f1, f1, f1
.LBB10_680:
	movimm 	x7, 	5
	addi	x15, x0, 0
	blt	x7, x6, .LBB10_700
	jal x0,	.LBB10_690
.LBB10_690:
	fcvt.w.s	x15, f1
.LBB10_700:
	slt	x13, x0, x15
	addi	x6, x15, 0
	bne	x0, x13, .LBB10_720
.LBB10_710:
	addi	x6, x0, 0
.LBB10_720:
	falloc	x7, x0
	movaddr 	x8, 	.HyOp#12
	fbind 	x8, 	x7
	falloc	x8, x0
	movaddr 	x12, 	.HyOp#13
	fbind 	x12, 	x8
	falloc	x12, x0
	movaddr 	x14, 	.HyOp#15
	fbind 	x14, 	x12
	movimm 	x14, 	1
	addi	x16, x15, 0
	bne	x0, x13, .LBB10_740
.LBB10_730:
	addi	x16, x14, 0
.LBB10_740:
	writecm	x12, x16, 60
	addi	x16, x6, -1
	addi	x17, x16, 0
	bne	x0, x13, .LBB10_760
.LBB10_750:
	addi	x17, x14, 0
.LBB10_760:
	falloc	x13, x17
	slli	x14, x16, 6
	add	x14, x13, x14
	addi	x16, x13, 0
.LBB10_770:
	movaddr 	x17, 	.HyOp#16
	fbind 	x17, 	x16
	addi	x16, x16, 64
	bgeu	x14, x16, .LBB10_770
	jal x0,	.LBB10_780
.LBB10_780:
	slt	x16, x0, x15
	writecm	x7, x11, 0
	slti	x17, x15, 1
	writecmp	x7, x17
	writecm	x8, x15, 8
	writecm	x8, x13, 4
	writecm	x8, x6, 0
	writecmp	x8, x16
	addi	x15, x13, 0
.LBB10_790:
	movaddr 	x16, 	"ga#44"
	movimm 	x17, 	192
	srli	x18, x15, 22
	srli	x29, x15, 12
	andi	x29, x29, 15
	srli	x31, x15, 6
	andi	x31, x31, 63
	addi	x1, x0, 832
	mul	x18, x18, x1
	addi	x1, x0, 52
	mul	x29, x29, x1
	add	x29, x29, x31
	add	x18, x29, x18
	mul	x17, x18, x17
	add	x16, x16, x17
	addi	x18, x16, 32
	addi	x29, x0, 0
.LBB10_800:
	addi	x17, x30, 32
	add	x17, x17, x29
	lbu	x17, x17, 0
	add	x1, x18, x29
	addi	x29, x29, 1
	movimm 	x31, 	32
	sb	x1, x17, 0
	addi	x17, x0, 0
	bne	x29, x31, .LBB10_800
	jal x0,	.LBB10_810
.LBB10_810:
	addi	x18, x30, 64
	add	x18, x18, x17
	lbu	x18, x18, 0
	add	x29, x16, x17
	addi	x17, x17, 1
	movimm 	x31, 	32
	sb	x29, x18, 0
	bne	x17, x31, .LBB10_810
	jal x0,	.LBB10_820
.LBB10_820:
	movimm 	x16, 	1
	writecm	x15, x16, 16
	writecm	x15, x6, 8
	writecm	x15, x12, 4
	writecm	x15, x13, 0
	addi	x15, x15, 64
	bgeu	x14, x15, .LBB10_790
	jal x0,	.LBB10_830
.LBB10_830:
	writecm	x12, x6, 4
	writecm	x12, x11, 0
	writecm	x11, x7, 12
	writecm	x11, x8, 8
	writecm	x11, x13, 4
	writecm	x11, x6, 0
	fdelete 	x10
	end x0
;uts_function_for.body141111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#15
		.anno	B
		.numphy	1
		.numOperand	3
		.pHyPC	.PC_uts_function_for_body1411110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_uts_function_for_body1411110
.PC_uts_function_for_body1411110:
	JAL x0, .LBB11_00
.LBB11_00:
	add		x30, x0, x10
	movaddr	x1, "ga#44"
	movimm	x5, 192
	add	x2, x0, x5
	srli	x3, x30, 22
	srli	x4, x30, 12
	andi	x5, x4, 15
	srli	x6, x30, 6
	andi	x7, x6, 63
	addi	x8, x0, 832
	mul		x9, x3, x8
	addi	x26, x0, 52
	mul		x27, x5, x26
	add		x28, x27, x7
	add		x29, x28, x9
	mul		x31, x29, x2
	add		x30, x1, x31
	movimm 	x6, 	1
	bge	x12, x6, .LBB11_10
.LBB11_40:
	addi	x6, x0, 0
	jal x0,	.LBB11_30
.LBB11_10:
	addi	x7, x30, 0
	addi	x6, x0, 0
	addi	x8, x0, 0
.LBB11_20:
	slli	x13, x8, 2
	add	x13, x7, x13
	lw	x13, x13, 0
	add	x6, x13, x6
	addi	x8, x8, 1
	bne	x8, x12, .LBB11_20
	jal x0,	.LBB11_30
.LBB11_30:
	writecm	x11, x6, 24
	fdelete 	x10
	end x0
;uts_function_for.body1511111

	.align 16
	.SMD_BEGIN	
		.HYPEROPID	.HyOp#16
		.anno	P
		.numphy	1
		.numOperand	5
		.pHyPC	.PC_uts_function_for_body15111110
	.SMD_END
	.align	16
	.PHYPEROP	.PC_uts_function_for_body15111110
.PC_uts_function_for_body15111110:
	JAL x0, .LBB12_00
.LBB12_00:
	add		x30, x0, x10
	movaddr	x1, "ga#44"
	movimm	x5, 192
	add	x2, x0, x5
	srli	x3, x30, 22
	srli	x4, x30, 12
	andi	x5, x4, 15
	srli	x6, x30, 6
	andi	x7, x6, 63
	addi	x8, x0, 832
	mul		x9, x3, x8
	addi	x26, x0, 52
	mul		x27, x5, x26
	add		x28, x27, x7
	add		x29, x28, x9
	mul		x31, x29, x2
	add		x30, x1, x31
	addi	x6, x30, 0
	sw	x6, x15, 0
	addi	x15, x30, 32
	lw	x7, x15, 4
	addi	x7, x7, 1
	sw	x6, x7, 4
	movimm 	x7, 	-1
	sw	x6, x7, 8
	addi	x13, x30, 156
	srli	x16, x14, 24
	movimm 	x7, 	24
	srli	x17, x14, 16
	srli	x18, x14, 8
	addi	x8, x30, 64
	sb	x13, x16, 0
	sb	x13, x17, 1
	sb	x13, x18, 2
	sb	x13, x14, 3
	sw	x8, x0, 4
	movimm 	x13, 	1732584193
	sw	x8, x13, 8
	movimm 	x13, 	-271733879
	sw	x8, x13, 12
	movimm 	x13, 	-1732584194
	sw	x8, x13, 16
	movimm 	x13, 	271733878
	sw	x8, x13, 20
	movimm 	x13, 	-1009589776
	sw	x8, x13, 24
	addi	x13, x8, 28
	lbu	x14, x15, 12
	lbu	x16, x15, 13
	lbu	x17, x15, 14
	lbu	x18, x15, 15
	lbu	x29, x15, 16
	lbu	x31, x15, 17
	lbu	x1, x15, 18
	lbu	x2, x15, 19
	lbu	x3, x15, 20
	lbu	x4, x15, 21
	lbu	x9, x15, 22
	lbu	x19, x15, 23
	lbu	x20, x15, 24
	lbu	x21, x15, 25
	lbu	x22, x15, 26
	lbu	x23, x15, 27
	lbu	x24, x15, 28
	lbu	x25, x15, 29
	lbu	x26, x15, 30
	lbu	x15, x15, 31
	sb	x8, x14, 28
	sb	x8, x16, 29
	sb	x8, x17, 30
	sb	x8, x18, 31
	sb	x8, x29, 32
	sb	x8, x31, 33
	sb	x8, x1, 34
	sb	x8, x2, 35
	sb	x8, x3, 36
	sb	x8, x4, 37
	sb	x8, x9, 38
	sb	x8, x19, 39
	sb	x8, x20, 40
	sb	x8, x21, 41
	sb	x8, x22, 42
	sb	x8, x23, 43
	sb	x8, x24, 44
	sb	x8, x25, 45
	sb	x8, x26, 46
	sb	x8, x15, 47
	sw	x8, x7, 0
	addi	x7, x0, 0
.LBB12_10:
	addi	x8, x7, 0
	addi	x7, x30, 156
	add	x7, x7, x8
	lbu	x14, x7, 0
	addi	x7, x8, 1
	movimm 	x15, 	4
	add	x8, x8, x13
	sb	x8, x14, 20
	bltu	x7, x15, .LBB12_10
	jal x0,	.LBB12_20
.LBB12_20:
	addi	x7, x30, 64
	lw	x8, x7, 0
	andi	x7, x8, 63
	addi	x13, x7, 3
	srli	x13, x13, 2
	beq	x13, x0, .LBB12_50
.LBB12_40:
	addi	x13, x13, -1
	slli	x14, x13, 2
	addi	x15, x30, 64
	add	x14, x15, x14
	lw	x15, x14, 28
	srli	x16, x15, 24
	slli	x17, x15, 8
	or	x16, x16, x17
	movimm 	x17, 	16711935
	and	x16, x16, x17
	srli	x17, x15, 8
	slli	x15, x15, 24
	movimm 	x18, 	-16711936
	and	x17, x17, x18
	or	x15, x17, x15
	or	x15, x15, x16
	sw	x14, x15, 28
	bne	x13, x0, .LBB12_40
	jal x0,	.LBB12_50
.LBB12_50:
	slli	x8, x8, 3
	andi	x8, x8, 24
	xori	x13, x8, 24
	movimm 	x8, 	-128
	sll	x14, x8, x13
	srli	x8, x7, 2
	slli	x15, x8, 2
	addi	x16, x30, 64
	add	x15, x16, x15
	lw	x16, x15, 28
	and	x14, x16, x14
	movimm 	x16, 	128
	sll	x13, x16, x13
	or	x13, x14, x13
	sw	x15, x13, 28
	movimm 	x13, 	56
	bgeu	x7, x13, .LBB12_60
	jal x0,	.LBB12_90
.LBB12_60:
	movimm 	x8, 	59
	bltu	x8, x7, .LBB12_80
	jal x0,	.LBB12_70
.LBB12_70:
	addi	x7, x30, 64
	sw	x7, x0, 88
.LBB12_80:
	addi	x7, x30, 64
	lw	x14, x7, 8
	lw	x15, x7, 12
	lw	x18, x7, 16
	lw	x17, x7, 20
	lw	x16, x7, 24
	srli	x8, x14, 27
	slli	x13, x14, 5
	or	x29, x8, x13
	and	x8, x18, x15
	xori	x13, x15, -1
	and	x13, x17, x13
	xor	x31, x13, x8
	lw	x13, x7, 28
	movimm 	x8, 	1518500249
	srli	x1, x15, 2
	slli	x15, x15, 30
	or	x15, x1, x15
	add	x16, x29, x16
	add	x16, x16, x31
	add	x16, x16, x13
	add	x29, x16, x8
	srli	x16, x29, 27
	slli	x31, x29, 5
	or	x31, x16, x31
	and	x16, x15, x14
	xori	x1, x14, -1
	and	x1, x18, x1
	xor	x1, x16, x1
	lw	x2, x7, 32
	srli	x16, x14, 2
	slli	x14, x14, 30
	or	x16, x16, x14
	add	x14, x17, x1
	add	x14, x14, x2
	add	x14, x14, x31
	add	x31, x14, x8
	srli	x14, x31, 27
	slli	x17, x31, 5
	or	x1, x14, x17
	and	x14, x29, x16
	xori	x17, x29, -1
	and	x17, x15, x17
	xor	x2, x14, x17
	lw	x14, x7, 36
	srli	x17, x29, 2
	slli	x29, x29, 30
	or	x17, x17, x29
	add	x18, x18, x14
	add	x18, x18, x2
	add	x18, x18, x1
	add	x29, x18, x8
	srli	x18, x29, 27
	slli	x1, x29, 5
	or	x1, x18, x1
	and	x18, x31, x17
	xori	x2, x31, -1
	and	x2, x16, x2
	xor	x2, x18, x2
	lw	x3, x7, 40
	srli	x18, x31, 2
	slli	x31, x31, 30
	or	x18, x18, x31
	add	x15, x15, x3
	add	x15, x15, x2
	add	x15, x15, x1
	add	x31, x15, x8
	srli	x15, x31, 27
	slli	x1, x31, 5
	or	x1, x15, x1
	and	x15, x29, x18
	xori	x2, x29, -1
	and	x2, x17, x2
	xor	x2, x15, x2
	lw	x3, x7, 44
	srli	x15, x29, 2
	slli	x29, x29, 30
	or	x15, x15, x29
	add	x16, x16, x3
	add	x16, x16, x2
	add	x16, x16, x1
	add	x29, x16, x8
	srli	x16, x29, 27
	slli	x1, x29, 5
	or	x1, x16, x1
	and	x16, x31, x15
	xori	x2, x31, -1
	and	x2, x18, x2
	xor	x2, x16, x2
	lw	x3, x7, 48
	srli	x16, x31, 2
	slli	x31, x31, 30
	or	x16, x16, x31
	add	x17, x17, x3
	add	x17, x17, x2
	add	x17, x17, x1
	add	x31, x17, x8
	srli	x17, x31, 27
	slli	x1, x31, 5
	or	x1, x17, x1
	and	x17, x29, x16
	xori	x2, x29, -1
	and	x2, x15, x2
	xor	x2, x17, x2
	lw	x3, x7, 52
	srli	x17, x29, 2
	slli	x29, x29, 30
	or	x17, x17, x29
	add	x18, x3, x18
	add	x18, x18, x2
	add	x18, x18, x1
	add	x29, x18, x8
	srli	x18, x29, 27
	slli	x1, x29, 5
	or	x1, x18, x1
	and	x18, x31, x17
	xori	x2, x31, -1
	and	x2, x16, x2
	xor	x2, x18, x2
	lw	x3, x7, 56
	srli	x18, x31, 2
	slli	x31, x31, 30
	or	x18, x18, x31
	add	x15, x3, x15
	add	x15, x15, x2
	add	x15, x15, x1
	add	x31, x15, x8
	srli	x15, x31, 27
	slli	x1, x31, 5
	or	x1, x15, x1
	and	x15, x29, x18
	xori	x2, x29, -1
	and	x2, x17, x2
	xor	x2, x15, x2
	lw	x15, x7, 60
	srli	x3, x29, 2
	slli	x29, x29, 30
	or	x29, x3, x29
	add	x16, x15, x16
	add	x16, x16, x2
	add	x16, x16, x1
	add	x1, x16, x8
	srli	x16, x1, 27
	slli	x2, x1, 5
	or	x2, x16, x2
	and	x16, x31, x29
	xori	x3, x31, -1
	and	x3, x18, x3
	xor	x3, x16, x3
	lw	x4, x7, 64
	srli	x16, x31, 2
	slli	x31, x31, 30
	or	x16, x16, x31
	add	x17, x4, x17
	add	x17, x17, x3
	add	x17, x17, x2
	add	x17, x17, x8
	srli	x31, x17, 27
	slli	x2, x17, 5
	or	x2, x31, x2
	and	x31, x1, x16
	xori	x3, x1, -1
	and	x3, x29, x3
	xor	x3, x31, x3
	lw	x4, x7, 68
	srli	x31, x1, 2
	slli	x1, x1, 30
	or	x31, x31, x1
	add	x18, x4, x18
	add	x18, x18, x3
	add	x18, x18, x2
	add	x18, x18, x8
	srli	x1, x18, 27
	slli	x2, x18, 5
	or	x1, x1, x2
	and	x2, x17, x31
	xori	x3, x17, -1
	and	x3, x16, x3
	xor	x3, x2, x3
	lw	x4, x7, 72
	srli	x2, x17, 2
	slli	x17, x17, 30
	or	x2, x2, x17
	add	x17, x4, x29
	add	x17, x17, x3
	add	x17, x17, x1
	add	x29, x17, x8
	srli	x17, x29, 27
	slli	x1, x29, 5
	or	x17, x17, x1
	and	x1, x18, x2
	xori	x3, x18, -1
	and	x3, x31, x3
	xor	x1, x1, x3
	lw	x4, x7, 76
	srli	x3, x18, 2
	slli	x18, x18, 30
	or	x3, x3, x18
	add	x16, x4, x16
	add	x16, x16, x1
	add	x16, x16, x17
	add	x16, x16, x8
	srli	x17, x16, 27
	slli	x18, x16, 5
	or	x1, x17, x18
	and	x17, x29, x3
	xori	x18, x29, -1
	and	x18, x2, x18
	xor	x4, x17, x18
	lw	x17, x7, 80
	srli	x18, x29, 2
	slli	x29, x29, 30
	or	x18, x18, x29
	add	x29, x17, x31
	add	x29, x29, x4
	add	x29, x29, x1
	add	x9, x29, x8
	srli	x29, x9, 27
	slli	x31, x9, 5
	or	x31, x29, x31
	and	x29, x16, x18
	xori	x1, x16, -1
	and	x1, x3, x1
	xor	x4, x29, x1
	lw	x1, x7, 84
	srli	x29, x16, 2
	slli	x16, x16, 30
	or	x29, x29, x16
	add	x16, x1, x2
	add	x16, x16, x4
	add	x16, x16, x31
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x31, x4, 5
	or	x2, x16, x31
	and	x16, x9, x29
	xori	x31, x9, -1
	and	x31, x18, x31
	xor	x19, x16, x31
	lw	x31, x7, 88
	srli	x16, x9, 2
	slli	x9, x9, 30
	or	x16, x16, x9
	add	x3, x31, x3
	add	x3, x3, x19
	add	x2, x3, x2
	add	x2, x2, x8
	srli	x3, x2, 27
	slli	x9, x2, 5
	or	x3, x3, x9
	and	x9, x4, x16
	xori	x19, x4, -1
	and	x19, x29, x19
	xor	x9, x9, x19
	xor	x13, x14, x13
	xor	x13, x13, x15
	xor	x13, x13, x17
	slli	x14, x13, 1
	srli	x13, x13, 31
	or	x17, x14, x13
	sw	x7, x17, 28
	srli	x13, x4, 2
	slli	x14, x4, 30
	or	x13, x13, x14
	add	x14, x17, x18
	add	x14, x14, x9
	add	x14, x14, x3
	add	x15, x14, x8
	srli	x14, x15, 27
	slli	x18, x15, 5
	or	x3, x14, x18
	and	x14, x2, x13
	xori	x18, x2, -1
	and	x18, x16, x18
	xor	x4, x14, x18
	lw	x14, x7, 64
	xor	x14, x14, x1
	lw	x1, x7, 40
	xor	x14, x14, x1
	lw	x18, x7, 32
	xor	x14, x14, x18
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 32
	srli	x14, x2, 2
	slli	x2, x2, 30
	or	x14, x14, x2
	add	x29, x18, x29
	add	x29, x29, x4
	add	x29, x29, x3
	add	x3, x29, x8
	srli	x29, x3, 27
	slli	x2, x3, 5
	or	x2, x29, x2
	and	x29, x15, x14
	xori	x4, x15, -1
	and	x4, x13, x4
	xor	x4, x29, x4
	lw	x29, x7, 68
	xor	x29, x29, x31
	lw	x31, x7, 44
	xor	x29, x29, x31
	lw	x9, x7, 36
	xor	x29, x29, x9
	slli	x9, x29, 1
	srli	x29, x29, 31
	or	x29, x9, x29
	sw	x7, x29, 36
	srli	x9, x15, 2
	slli	x15, x15, 30
	or	x15, x9, x15
	add	x16, x29, x16
	add	x16, x16, x4
	add	x16, x16, x2
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x2, x4, 5
	or	x9, x16, x2
	and	x16, x3, x15
	xori	x2, x3, -1
	and	x2, x14, x2
	xor	x19, x16, x2
	lw	x16, x7, 72
	lw	x2, x7, 48
	xor	x17, x1, x17
	xor	x16, x17, x16
	xor	x16, x16, x2
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 40
	srli	x16, x3, 2
	slli	x1, x3, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x8, x3, 27
	slli	x13, x3, 5
	or	x9, x8, x13
	xor	x8, x16, x15
	xor	x19, x8, x4
	lw	x8, x7, 76
	lw	x1, x7, 52
	xor	x13, x18, x31
	xor	x8, x13, x8
	xor	x8, x8, x1
	slli	x13, x8, 1
	srli	x8, x8, 31
	or	x18, x13, x8
	sw	x7, x18, 44
	movimm 	x8, 	1859775393
	srli	x13, x4, 2
	slli	x31, x4, 30
	or	x13, x13, x31
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x31, x4, 5
	or	x9, x14, x31
	xor	x14, x13, x16
	xor	x19, x14, x3
	lw	x14, x7, 80
	lw	x31, x7, 56
	xor	x29, x29, x2
	xor	x14, x29, x14
	xor	x14, x14, x31
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 48
	srli	x14, x3, 2
	slli	x2, x3, 30
	or	x14, x14, x2
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x3, x15, x8
	srli	x15, x3, 27
	slli	x2, x3, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	xor	x19, x15, x4
	lw	x15, x7, 84
	lw	x2, x7, 60
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x2
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 52
	srli	x15, x4, 2
	slli	x1, x4, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x1, x4, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	xor	x19, x16, x3
	lw	x16, x7, 88
	lw	x1, x7, 64
	xor	x18, x18, x31
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 56
	srli	x16, x3, 2
	slli	x31, x3, 30
	or	x16, x16, x31
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x31, x3, 5
	or	x9, x13, x31
	xor	x13, x16, x15
	xor	x19, x13, x4
	lw	x13, x7, 28
	lw	x31, x7, 68
	xor	x29, x29, x2
	xor	x13, x29, x13
	xor	x13, x13, x31
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 60
	srli	x13, x4, 2
	slli	x2, x4, 30
	or	x13, x13, x2
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x2, x4, 5
	or	x9, x14, x2
	xor	x14, x13, x16
	xor	x19, x14, x3
	lw	x14, x7, 32
	lw	x2, x7, 72
	xor	x17, x17, x1
	xor	x14, x17, x14
	xor	x14, x14, x2
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x7, x17, 64
	srli	x14, x3, 2
	slli	x1, x3, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x3, x15, x8
	srli	x15, x3, 27
	slli	x1, x3, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	xor	x19, x15, x4
	lw	x15, x7, 36
	lw	x1, x7, 76
	xor	x18, x18, x31
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 68
	srli	x15, x4, 2
	slli	x31, x4, 30
	or	x15, x15, x31
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x31, x4, 5
	or	x9, x16, x31
	xor	x16, x15, x14
	xor	x19, x16, x3
	lw	x16, x7, 40
	lw	x31, x7, 80
	xor	x29, x29, x2
	xor	x16, x29, x16
	xor	x16, x16, x31
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 72
	srli	x16, x3, 2
	slli	x2, x3, 30
	or	x16, x16, x2
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x2, x3, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	xor	x19, x13, x4
	lw	x13, x7, 44
	lw	x2, x7, 84
	xor	x17, x17, x1
	xor	x13, x17, x13
	xor	x13, x13, x2
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 76
	srli	x13, x4, 2
	slli	x1, x4, 30
	or	x13, x13, x1
	add	x14, x17, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x1, x4, 5
	or	x9, x14, x1
	xor	x14, x13, x16
	xor	x19, x14, x3
	lw	x14, x7, 48
	lw	x1, x7, 88
	xor	x18, x18, x31
	xor	x14, x18, x14
	xor	x14, x14, x1
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 80
	srli	x14, x3, 2
	slli	x31, x3, 30
	or	x14, x14, x31
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x3, x15, x8
	srli	x15, x3, 27
	slli	x31, x3, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	xor	x19, x15, x4
	lw	x15, x7, 52
	lw	x31, x7, 28
	xor	x29, x29, x2
	xor	x15, x29, x15
	xor	x15, x15, x31
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 84
	srli	x15, x4, 2
	slli	x2, x4, 30
	or	x15, x15, x2
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x2, x4, 5
	or	x9, x16, x2
	xor	x16, x15, x14
	xor	x19, x16, x3
	lw	x16, x7, 56
	lw	x2, x7, 32
	xor	x17, x17, x1
	xor	x16, x17, x16
	xor	x16, x16, x2
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 88
	srli	x16, x3, 2
	slli	x1, x3, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x1, x3, 5
	or	x9, x13, x1
	xor	x13, x16, x15
	xor	x19, x13, x4
	lw	x13, x7, 60
	lw	x1, x7, 36
	xor	x18, x18, x31
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 28
	srli	x13, x4, 2
	slli	x31, x4, 30
	or	x13, x13, x31
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x31, x4, 5
	or	x9, x14, x31
	xor	x14, x13, x16
	xor	x19, x14, x3
	lw	x14, x7, 64
	lw	x31, x7, 40
	xor	x29, x29, x2
	xor	x14, x29, x14
	xor	x14, x14, x31
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 32
	srli	x14, x3, 2
	slli	x2, x3, 30
	or	x14, x14, x2
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x3, x15, x8
	srli	x15, x3, 27
	slli	x2, x3, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	xor	x19, x15, x4
	lw	x15, x7, 68
	lw	x2, x7, 44
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x2
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x15, x17, x15
	sw	x7, x15, 36
	srli	x17, x4, 2
	slli	x1, x4, 30
	or	x17, x17, x1
	add	x16, x15, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x1, x4, 5
	or	x9, x16, x1
	xor	x16, x17, x14
	xor	x19, x16, x3
	lw	x16, x7, 72
	lw	x1, x7, 48
	xor	x18, x18, x31
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x16, x18, x16
	sw	x7, x16, 40
	srli	x18, x3, 2
	slli	x31, x3, 30
	or	x18, x18, x31
	add	x13, x16, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x31, x3, 5
	or	x9, x13, x31
	xor	x13, x18, x17
	xor	x19, x13, x4
	lw	x13, x7, 76
	lw	x31, x7, 52
	xor	x29, x29, x2
	xor	x13, x29, x13
	xor	x13, x13, x31
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x13, x29, x13
	sw	x7, x13, 44
	srli	x29, x4, 2
	slli	x2, x4, 30
	or	x29, x29, x2
	add	x14, x13, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x2, x4, 5
	or	x9, x14, x2
	xor	x14, x29, x18
	xor	x19, x14, x3
	lw	x14, x7, 80
	lw	x2, x7, 56
	xor	x15, x15, x1
	xor	x14, x15, x14
	xor	x14, x14, x2
	slli	x15, x14, 1
	srli	x14, x14, 31
	or	x14, x15, x14
	sw	x7, x14, 48
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x17, x14, x17
	add	x17, x17, x19
	add	x17, x17, x9
	add	x19, x17, x8
	srli	x17, x19, 27
	slli	x1, x19, 5
	or	x17, x17, x1
	xor	x1, x15, x29
	xor	x1, x1, x4
	lw	x9, x7, 84
	lw	x3, x7, 60
	xor	x16, x16, x31
	xor	x16, x16, x9
	xor	x16, x16, x3
	slli	x31, x16, 1
	srli	x16, x16, 31
	or	x31, x31, x16
	sw	x7, x31, 52
	srli	x16, x4, 2
	slli	x4, x4, 30
	or	x16, x16, x4
	add	x18, x31, x18
	add	x18, x18, x1
	add	x17, x18, x17
	add	x9, x17, x8
	srli	x17, x9, 27
	slli	x18, x9, 5
	or	x18, x17, x18
	xor	x17, x16, x15
	xor	x4, x17, x19
	lw	x17, x7, 88
	lw	x1, x7, 64
	xor	x13, x13, x2
	xor	x13, x13, x17
	xor	x13, x13, x1
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 56
	srli	x13, x19, 2
	slli	x2, x19, 30
	or	x13, x13, x2
	add	x29, x17, x29
	add	x29, x29, x4
	add	x18, x29, x18
	add	x4, x18, x8
	srli	x8, x4, 27
	slli	x18, x4, 5
	or	x29, x8, x18
	xor	x8, x13, x16
	and	x8, x9, x8
	and	x18, x13, x16
	xor	x19, x8, x18
	lw	x8, x7, 28
	lw	x2, x7, 68
	xor	x14, x14, x3
	xor	x8, x14, x8
	xor	x8, x8, x2
	slli	x14, x8, 1
	srli	x8, x8, 31
	or	x18, x14, x8
	sw	x7, x18, 60
	movimm 	x8, 	-1894007588
	srli	x14, x9, 2
	slli	x3, x9, 30
	or	x14, x14, x3
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x29
	add	x9, x15, x8
	srli	x15, x9, 27
	slli	x29, x9, 5
	or	x19, x15, x29
	xor	x15, x14, x13
	and	x15, x4, x15
	and	x29, x14, x13
	xor	x20, x15, x29
	lw	x15, x7, 32
	lw	x3, x7, 72
	xor	x29, x31, x1
	xor	x15, x29, x15
	xor	x15, x15, x3
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 64
	srli	x15, x4, 2
	slli	x31, x4, 30
	or	x15, x15, x31
	add	x16, x29, x16
	add	x16, x16, x20
	add	x16, x16, x19
	add	x4, x16, x8
	srli	x16, x4, 27
	slli	x31, x4, 5
	or	x1, x16, x31
	xor	x16, x15, x14
	and	x16, x9, x16
	and	x31, x15, x14
	xor	x19, x16, x31
	lw	x16, x7, 36
	lw	x31, x7, 76
	xor	x17, x17, x2
	xor	x16, x17, x16
	xor	x16, x16, x31
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 68
	srli	x16, x9, 2
	slli	x2, x9, 30
	or	x16, x16, x2
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x1
	add	x9, x13, x8
	srli	x13, x9, 27
	slli	x1, x9, 5
	or	x2, x13, x1
	xor	x13, x16, x15
	and	x13, x4, x13
	and	x1, x16, x15
	xor	x19, x13, x1
	lw	x13, x7, 40
	lw	x1, x7, 80
	xor	x18, x18, x3
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 72
	srli	x13, x4, 2
	slli	x3, x4, 30
	or	x13, x13, x3
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x2
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x2, x3, 5
	or	x4, x14, x2
	xor	x14, x13, x16
	and	x14, x9, x14
	and	x2, x13, x16
	xor	x19, x14, x2
	lw	x14, x7, 44
	lw	x2, x7, 84
	xor	x29, x29, x31
	xor	x14, x29, x14
	xor	x14, x14, x2
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 76
	srli	x14, x9, 2
	slli	x31, x9, 30
	or	x14, x14, x31
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x4
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x31, x14, x13
	xor	x19, x15, x31
	lw	x15, x7, 48
	lw	x31, x7, 88
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x31
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 80
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x1, x15, x14
	xor	x19, x16, x1
	lw	x16, x7, 52
	lw	x1, x7, 28
	xor	x18, x18, x2
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 84
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x2, x4, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x2, x16, x15
	xor	x19, x13, x2
	lw	x13, x7, 56
	lw	x2, x7, 32
	xor	x29, x29, x31
	xor	x13, x29, x13
	xor	x13, x13, x2
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 88
	srli	x13, x3, 2
	slli	x31, x3, 30
	or	x13, x13, x31
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x31, x3, 5
	or	x9, x14, x31
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x31, x13, x16
	xor	x19, x14, x31
	lw	x14, x7, 60
	lw	x31, x7, 36
	xor	x17, x17, x1
	xor	x14, x17, x14
	xor	x14, x14, x31
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x7, x17, 28
	srli	x14, x4, 2
	slli	x1, x4, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x1, x14, x13
	xor	x19, x15, x1
	lw	x15, x7, 64
	lw	x1, x7, 40
	xor	x18, x18, x2
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 32
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x2, x3, 5
	or	x9, x16, x2
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x2, x15, x14
	xor	x19, x16, x2
	lw	x16, x7, 68
	lw	x2, x7, 44
	xor	x29, x29, x31
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 36
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x31, x4, 5
	or	x9, x13, x31
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x31, x16, x15
	xor	x19, x13, x31
	lw	x13, x7, 72
	lw	x31, x7, 48
	xor	x17, x17, x1
	xor	x13, x17, x13
	xor	x13, x13, x31
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 40
	srli	x13, x3, 2
	slli	x1, x3, 30
	or	x13, x13, x1
	add	x14, x17, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x1, x3, 5
	or	x9, x14, x1
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x1, x13, x16
	xor	x19, x14, x1
	lw	x14, x7, 76
	lw	x1, x7, 52
	xor	x18, x18, x2
	xor	x14, x18, x14
	xor	x14, x14, x1
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 44
	srli	x14, x4, 2
	slli	x2, x4, 30
	or	x14, x14, x2
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x2, x4, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x2, x14, x13
	xor	x19, x15, x2
	lw	x15, x7, 80
	lw	x2, x7, 56
	xor	x29, x29, x31
	xor	x15, x29, x15
	xor	x15, x15, x2
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 48
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x31, x3, 5
	or	x9, x16, x31
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x31, x15, x14
	xor	x19, x16, x31
	lw	x16, x7, 84
	lw	x31, x7, 60
	xor	x17, x17, x1
	xor	x16, x17, x16
	xor	x16, x16, x31
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 52
	srli	x16, x4, 2
	slli	x1, x4, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x1, x4, 5
	or	x9, x13, x1
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x1, x16, x15
	xor	x19, x13, x1
	lw	x13, x7, 88
	lw	x1, x7, 64
	xor	x18, x18, x2
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 56
	srli	x13, x3, 2
	slli	x2, x3, 30
	or	x13, x13, x2
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x2, x3, 5
	or	x9, x14, x2
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x2, x13, x16
	xor	x19, x14, x2
	lw	x14, x7, 28
	lw	x2, x7, 68
	xor	x29, x29, x31
	xor	x14, x29, x14
	xor	x14, x14, x2
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 60
	srli	x14, x4, 2
	slli	x31, x4, 30
	or	x14, x14, x31
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x31, x14, x13
	xor	x19, x15, x31
	lw	x15, x7, 32
	lw	x31, x7, 72
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x31
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 64
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x1, x15, x14
	xor	x19, x16, x1
	lw	x16, x7, 36
	lw	x1, x7, 76
	xor	x18, x18, x2
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 68
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x2, x4, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x2, x16, x15
	xor	x19, x13, x2
	lw	x13, x7, 40
	lw	x2, x7, 80
	xor	x29, x29, x31
	xor	x13, x29, x13
	xor	x13, x13, x2
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 72
	srli	x13, x3, 2
	slli	x31, x3, 30
	or	x13, x13, x31
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x8, x3, 27
	slli	x14, x3, 5
	or	x9, x8, x14
	xor	x8, x13, x16
	xor	x19, x8, x4
	lw	x8, x7, 44
	lw	x31, x7, 84
	xor	x14, x17, x1
	xor	x8, x14, x8
	xor	x8, x8, x31
	slli	x14, x8, 1
	srli	x8, x8, 31
	or	x17, x14, x8
	sw	x7, x17, 76
	movimm 	x8, 	-899497514
	srli	x14, x4, 2
	slli	x1, x4, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 48
	lw	x1, x7, 88
	xor	x18, x18, x2
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 80
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x2, x3, 5
	or	x9, x16, x2
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 52
	lw	x2, x7, 28
	xor	x29, x29, x31
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 84
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x31, x4, 5
	or	x9, x13, x31
	xor	x13, x16, x15
	xor	x19, x13, x3
	lw	x13, x7, 56
	lw	x31, x7, 32
	xor	x17, x17, x1
	xor	x13, x17, x13
	xor	x13, x13, x31
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 88
	srli	x13, x3, 2
	slli	x1, x3, 30
	or	x13, x13, x1
	add	x14, x17, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x1, x3, 5
	or	x9, x14, x1
	xor	x14, x13, x16
	xor	x19, x14, x4
	lw	x14, x7, 60
	lw	x1, x7, 36
	xor	x18, x18, x2
	xor	x14, x18, x14
	xor	x14, x14, x1
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 28
	srli	x14, x4, 2
	slli	x2, x4, 30
	or	x14, x14, x2
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x2, x4, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 64
	lw	x2, x7, 40
	xor	x29, x29, x31
	xor	x15, x29, x15
	xor	x15, x15, x2
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 32
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x31, x3, 5
	or	x9, x16, x31
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 68
	lw	x31, x7, 44
	xor	x17, x17, x1
	xor	x16, x17, x16
	xor	x16, x16, x31
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 36
	srli	x16, x4, 2
	slli	x1, x4, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x1, x4, 5
	or	x9, x13, x1
	xor	x13, x16, x15
	xor	x19, x13, x3
	lw	x13, x7, 72
	lw	x1, x7, 48
	xor	x18, x18, x2
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 40
	srli	x13, x3, 2
	slli	x2, x3, 30
	or	x13, x13, x2
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x2, x3, 5
	or	x9, x14, x2
	xor	x14, x13, x16
	xor	x19, x14, x4
	lw	x14, x7, 76
	lw	x2, x7, 52
	xor	x29, x29, x31
	xor	x14, x29, x14
	xor	x14, x14, x2
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 44
	srli	x14, x4, 2
	slli	x31, x4, 30
	or	x14, x14, x31
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 80
	lw	x31, x7, 56
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x31
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 48
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 84
	lw	x1, x7, 60
	xor	x18, x18, x2
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 52
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x2, x4, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	xor	x19, x13, x3
	lw	x13, x7, 88
	lw	x2, x7, 64
	xor	x29, x29, x31
	xor	x13, x29, x13
	xor	x13, x13, x2
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 56
	srli	x13, x3, 2
	slli	x31, x3, 30
	or	x13, x13, x31
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x31, x3, 5
	or	x9, x14, x31
	xor	x14, x13, x16
	xor	x19, x14, x4
	lw	x14, x7, 28
	lw	x31, x7, 68
	xor	x17, x17, x1
	xor	x14, x17, x14
	xor	x14, x14, x31
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x7, x17, 60
	srli	x14, x4, 2
	slli	x1, x4, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 32
	lw	x1, x7, 72
	xor	x18, x18, x2
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 64
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x9, x16, x8
	srli	x16, x9, 27
	slli	x2, x9, 5
	or	x3, x16, x2
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 36
	lw	x2, x7, 76
	xor	x29, x29, x31
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 68
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x3
	add	x13, x13, x8
	srli	x31, x13, 27
	slli	x3, x13, 5
	or	x4, x31, x3
	xor	x31, x16, x15
	xor	x19, x31, x9
	lw	x31, x7, 40
	lw	x3, x7, 80
	xor	x17, x17, x1
	xor	x17, x17, x31
	xor	x17, x17, x3
	slli	x31, x17, 1
	srli	x17, x17, 31
	or	x31, x31, x17
	sw	x7, x31, 72
	srli	x17, x9, 2
	slli	x1, x9, 30
	or	x17, x17, x1
	add	x14, x31, x14
	add	x14, x14, x19
	add	x14, x14, x4
	add	x14, x14, x8
	srli	x1, x14, 27
	slli	x4, x14, 5
	or	x4, x1, x4
	xor	x1, x17, x16
	xor	x9, x1, x13
	lw	x19, x7, 44
	lw	x1, x7, 84
	xor	x18, x18, x2
	xor	x18, x18, x19
	xor	x18, x18, x1
	slli	x2, x18, 1
	srli	x18, x18, 31
	or	x18, x2, x18
	sw	x7, x18, 76
	srli	x2, x13, 2
	slli	x13, x13, 30
	or	x13, x2, x13
	add	x15, x18, x15
	add	x15, x15, x9
	add	x15, x15, x4
	add	x2, x15, x8
	srli	x15, x2, 27
	slli	x4, x2, 5
	or	x4, x15, x4
	xor	x15, x13, x17
	xor	x9, x15, x14
	lw	x19, x7, 48
	lw	x15, x7, 88
	xor	x29, x29, x3
	xor	x29, x29, x19
	xor	x29, x29, x15
	slli	x3, x29, 1
	srli	x29, x29, 31
	or	x29, x3, x29
	sw	x7, x29, 80
	srli	x3, x14, 2
	slli	x14, x14, 30
	or	x14, x3, x14
	add	x16, x29, x16
	add	x16, x16, x9
	add	x16, x16, x4
	add	x16, x16, x8
	srli	x29, x16, 27
	slli	x3, x16, 5
	or	x3, x29, x3
	xor	x29, x14, x13
	xor	x4, x29, x2
	lw	x29, x7, 52
	lw	x9, x7, 28
	xor	x31, x31, x1
	xor	x29, x31, x29
	xor	x29, x29, x9
	slli	x31, x29, 1
	srli	x29, x29, 31
	or	x31, x31, x29
	sw	x7, x31, 84
	srli	x29, x2, 2
	slli	x1, x2, 30
	or	x29, x29, x1
	add	x17, x31, x17
	add	x17, x17, x4
	add	x17, x17, x3
	add	x17, x17, x8
	srli	x31, x17, 27
	slli	x1, x17, 5
	or	x31, x31, x1
	xor	x1, x29, x14
	xor	x1, x1, x16
	lw	x2, x7, 56
	lw	x3, x7, 32
	xor	x15, x18, x15
	xor	x15, x15, x2
	xor	x15, x15, x3
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 88
	srli	x15, x16, 2
	slli	x16, x16, 30
	or	x15, x15, x16
	lw	x16, x7, 8
	add	x16, x16, x18
	add	x13, x16, x13
	add	x13, x13, x1
	add	x13, x13, x31
	add	x8, x13, x8
	sw	x7, x8, 8
	lw	x8, x7, 12
	add	x8, x17, x8
	sw	x7, x8, 12
	lw	x8, x7, 16
	add	x8, x15, x8
	sw	x7, x8, 16
	lw	x8, x7, 20
	add	x8, x29, x8
	sw	x7, x8, 20
	lw	x8, x7, 24
	add	x8, x14, x8
	sw	x7, x8, 24
	addi	x7, x0, 0
	jal x0,	.LBB12_100
.LBB12_90:
	addi	x7, x8, 1
	movimm 	x8, 	13
	bltu	x8, x7, .LBB12_110
	jal x0,	.LBB12_100
.LBB12_100:
	addi	x8, x7, 0
	addi	x7, x8, 1
	slli	x8, x8, 2
	addi	x13, x30, 64
	add	x8, x13, x8
	sw	x8, x0, 28
	movimm 	x8, 	14
	bne	x7, x8, .LBB12_100
	jal x0,	.LBB12_110
.LBB12_110:
	addi	x7, x30, 64
	lw	x8, x7, 4
	slli	x13, x8, 3
	lw	x8, x7, 0
	srli	x14, x8, 29
	or	x13, x14, x13
	sw	x7, x13, 84
	slli	x13, x8, 3
	sw	x7, x13, 88
	lw	x15, x7, 8
	lw	x16, x7, 12
	lw	x29, x7, 16
	lw	x18, x7, 20
	lw	x17, x7, 24
	srli	x8, x15, 27
	slli	x14, x15, 5
	or	x31, x8, x14
	and	x8, x29, x16
	xori	x14, x16, -1
	and	x14, x18, x14
	xor	x1, x14, x8
	lw	x14, x7, 28
	movimm 	x8, 	1518500249
	srli	x2, x16, 2
	slli	x16, x16, 30
	or	x16, x2, x16
	add	x17, x31, x17
	add	x17, x17, x1
	add	x17, x17, x14
	add	x31, x17, x8
	srli	x17, x31, 27
	slli	x1, x31, 5
	or	x1, x17, x1
	and	x17, x16, x15
	xori	x2, x15, -1
	and	x2, x29, x2
	xor	x2, x17, x2
	lw	x3, x7, 32
	srli	x17, x15, 2
	slli	x15, x15, 30
	or	x17, x17, x15
	add	x15, x18, x2
	add	x15, x15, x3
	add	x15, x15, x1
	add	x1, x15, x8
	srli	x15, x1, 27
	slli	x18, x1, 5
	or	x2, x15, x18
	and	x15, x31, x17
	xori	x18, x31, -1
	and	x18, x16, x18
	xor	x3, x15, x18
	lw	x15, x7, 36
	srli	x18, x31, 2
	slli	x31, x31, 30
	or	x18, x18, x31
	add	x29, x29, x15
	add	x29, x29, x3
	add	x29, x29, x2
	add	x31, x29, x8
	srli	x29, x31, 27
	slli	x2, x31, 5
	or	x2, x29, x2
	and	x29, x1, x18
	xori	x3, x1, -1
	and	x3, x17, x3
	xor	x3, x29, x3
	lw	x4, x7, 40
	srli	x29, x1, 2
	slli	x1, x1, 30
	or	x29, x29, x1
	add	x16, x16, x4
	add	x16, x16, x3
	add	x16, x16, x2
	add	x1, x16, x8
	srli	x16, x1, 27
	slli	x2, x1, 5
	or	x2, x16, x2
	and	x16, x31, x29
	xori	x3, x31, -1
	and	x3, x18, x3
	xor	x3, x16, x3
	lw	x4, x7, 44
	srli	x16, x31, 2
	slli	x31, x31, 30
	or	x16, x16, x31
	add	x17, x17, x4
	add	x17, x17, x3
	add	x17, x17, x2
	add	x31, x17, x8
	srli	x17, x31, 27
	slli	x2, x31, 5
	or	x2, x17, x2
	and	x17, x1, x16
	xori	x3, x1, -1
	and	x3, x29, x3
	xor	x3, x17, x3
	lw	x4, x7, 48
	srli	x17, x1, 2
	slli	x1, x1, 30
	or	x17, x17, x1
	add	x18, x18, x4
	add	x18, x18, x3
	add	x18, x18, x2
	add	x1, x18, x8
	srli	x18, x1, 27
	slli	x2, x1, 5
	or	x2, x18, x2
	and	x18, x31, x17
	xori	x3, x31, -1
	and	x3, x16, x3
	xor	x3, x18, x3
	lw	x4, x7, 52
	srli	x18, x31, 2
	slli	x31, x31, 30
	or	x18, x18, x31
	add	x29, x4, x29
	add	x29, x29, x3
	add	x29, x29, x2
	add	x31, x29, x8
	srli	x29, x31, 27
	slli	x2, x31, 5
	or	x2, x29, x2
	and	x29, x1, x18
	xori	x3, x1, -1
	and	x3, x17, x3
	xor	x3, x29, x3
	lw	x4, x7, 56
	srli	x29, x1, 2
	slli	x1, x1, 30
	or	x29, x29, x1
	add	x16, x4, x16
	add	x16, x16, x3
	add	x16, x16, x2
	add	x1, x16, x8
	srli	x16, x1, 27
	slli	x2, x1, 5
	or	x2, x16, x2
	and	x16, x31, x29
	xori	x3, x31, -1
	and	x3, x18, x3
	xor	x3, x16, x3
	lw	x16, x7, 60
	srli	x4, x31, 2
	slli	x31, x31, 30
	or	x31, x4, x31
	add	x17, x16, x17
	add	x17, x17, x3
	add	x17, x17, x2
	add	x2, x17, x8
	srli	x17, x2, 27
	slli	x3, x2, 5
	or	x3, x17, x3
	and	x17, x1, x31
	xori	x4, x1, -1
	and	x4, x29, x4
	xor	x4, x17, x4
	lw	x9, x7, 64
	srli	x17, x1, 2
	slli	x1, x1, 30
	or	x17, x17, x1
	add	x18, x9, x18
	add	x18, x18, x4
	add	x18, x18, x3
	add	x18, x18, x8
	srli	x1, x18, 27
	slli	x3, x18, 5
	or	x3, x1, x3
	and	x1, x2, x17
	xori	x4, x2, -1
	and	x4, x31, x4
	xor	x4, x1, x4
	lw	x9, x7, 68
	srli	x1, x2, 2
	slli	x2, x2, 30
	or	x1, x1, x2
	add	x29, x9, x29
	add	x29, x29, x4
	add	x29, x29, x3
	add	x29, x29, x8
	srli	x2, x29, 27
	slli	x3, x29, 5
	or	x3, x2, x3
	and	x2, x18, x1
	xori	x4, x18, -1
	and	x4, x17, x4
	xor	x4, x2, x4
	lw	x9, x7, 72
	srli	x2, x18, 2
	slli	x18, x18, 30
	or	x2, x2, x18
	add	x18, x9, x31
	add	x18, x18, x4
	add	x18, x18, x3
	add	x18, x18, x8
	srli	x31, x18, 27
	slli	x3, x18, 5
	or	x31, x31, x3
	and	x3, x29, x2
	xori	x4, x29, -1
	and	x4, x1, x4
	xor	x3, x3, x4
	lw	x9, x7, 76
	srli	x4, x29, 2
	slli	x29, x29, 30
	or	x4, x4, x29
	add	x17, x9, x17
	add	x17, x17, x3
	add	x17, x17, x31
	add	x17, x17, x8
	srli	x29, x17, 27
	slli	x31, x17, 5
	or	x29, x29, x31
	and	x31, x18, x4
	xori	x3, x18, -1
	and	x3, x2, x3
	xor	x31, x31, x3
	lw	x3, x7, 80
	srli	x9, x18, 2
	slli	x18, x18, 30
	or	x18, x9, x18
	add	x1, x3, x1
	add	x31, x1, x31
	add	x29, x31, x29
	add	x1, x29, x8
	srli	x29, x1, 27
	slli	x31, x1, 5
	or	x9, x29, x31
	and	x29, x17, x18
	xori	x31, x17, -1
	and	x31, x4, x31
	xor	x19, x29, x31
	lw	x31, x7, 84
	srli	x29, x17, 2
	slli	x17, x17, 30
	or	x29, x29, x17
	add	x17, x31, x2
	add	x17, x17, x19
	add	x17, x17, x9
	add	x2, x17, x8
	srli	x17, x2, 27
	slli	x9, x2, 5
	or	x9, x17, x9
	and	x17, x1, x29
	xori	x19, x1, -1
	and	x19, x18, x19
	xor	x19, x17, x19
	srli	x17, x1, 2
	slli	x1, x1, 30
	or	x17, x17, x1
	add	x13, x13, x4
	add	x13, x13, x19
	add	x13, x13, x9
	add	x1, x13, x8
	srli	x13, x1, 27
	slli	x4, x1, 5
	or	x4, x13, x4
	and	x13, x2, x17
	xori	x9, x2, -1
	and	x9, x29, x9
	xor	x9, x13, x9
	xor	x13, x15, x14
	xor	x13, x13, x16
	xor	x13, x13, x3
	slli	x14, x13, 1
	srli	x13, x13, 31
	or	x15, x14, x13
	sw	x7, x15, 28
	srli	x13, x2, 2
	slli	x14, x2, 30
	or	x13, x13, x14
	add	x14, x15, x18
	add	x14, x14, x9
	add	x14, x14, x4
	add	x16, x14, x8
	srli	x14, x16, 27
	slli	x18, x16, 5
	or	x2, x14, x18
	and	x14, x1, x13
	xori	x18, x1, -1
	and	x18, x17, x18
	xor	x3, x14, x18
	lw	x14, x7, 64
	xor	x14, x14, x31
	lw	x31, x7, 40
	xor	x14, x14, x31
	lw	x18, x7, 32
	xor	x14, x14, x18
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 32
	srli	x14, x1, 2
	slli	x1, x1, 30
	or	x14, x14, x1
	add	x29, x18, x29
	add	x29, x29, x3
	add	x29, x29, x2
	add	x3, x29, x8
	srli	x29, x3, 27
	slli	x1, x3, 5
	or	x2, x29, x1
	and	x29, x16, x14
	xori	x1, x16, -1
	and	x1, x13, x1
	xor	x4, x29, x1
	lw	x29, x7, 88
	lw	x1, x7, 68
	xor	x29, x1, x29
	lw	x1, x7, 44
	xor	x29, x29, x1
	lw	x9, x7, 36
	xor	x29, x29, x9
	slli	x9, x29, 1
	srli	x29, x29, 31
	or	x29, x9, x29
	sw	x7, x29, 36
	srli	x9, x16, 2
	slli	x16, x16, 30
	or	x16, x9, x16
	add	x17, x29, x17
	add	x17, x17, x4
	add	x17, x17, x2
	add	x4, x17, x8
	srli	x17, x4, 27
	slli	x2, x4, 5
	or	x9, x17, x2
	and	x17, x3, x16
	xori	x2, x3, -1
	and	x2, x14, x2
	xor	x19, x17, x2
	lw	x17, x7, 72
	lw	x2, x7, 48
	xor	x15, x31, x15
	xor	x15, x15, x17
	xor	x15, x15, x2
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 40
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x8, x3, 27
	slli	x13, x3, 5
	or	x9, x8, x13
	xor	x8, x15, x16
	xor	x19, x8, x4
	lw	x8, x7, 76
	lw	x31, x7, 52
	xor	x13, x1, x18
	xor	x8, x13, x8
	xor	x8, x8, x31
	slli	x13, x8, 1
	srli	x8, x8, 31
	or	x18, x13, x8
	sw	x7, x18, 44
	movimm 	x8, 	1859775393
	srli	x13, x4, 2
	slli	x1, x4, 30
	or	x13, x13, x1
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x1, x4, 5
	or	x9, x14, x1
	xor	x14, x13, x15
	xor	x19, x14, x3
	lw	x14, x7, 80
	lw	x1, x7, 56
	xor	x29, x29, x2
	xor	x14, x29, x14
	xor	x14, x14, x1
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 48
	srli	x14, x3, 2
	slli	x2, x3, 30
	or	x14, x14, x2
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x2, x3, 5
	or	x9, x16, x2
	xor	x16, x14, x13
	xor	x19, x16, x4
	lw	x16, x7, 84
	lw	x2, x7, 60
	xor	x17, x17, x31
	xor	x16, x17, x16
	xor	x16, x16, x2
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 52
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x16, x14
	xor	x19, x15, x3
	lw	x15, x7, 88
	lw	x31, x7, 64
	xor	x18, x18, x1
	xor	x15, x18, x15
	xor	x15, x15, x31
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 56
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x1, x3, 5
	or	x9, x13, x1
	xor	x13, x15, x16
	xor	x19, x13, x4
	lw	x13, x7, 28
	lw	x1, x7, 68
	xor	x29, x29, x2
	xor	x13, x29, x13
	xor	x13, x13, x1
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 60
	srli	x13, x4, 2
	slli	x2, x4, 30
	or	x13, x13, x2
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x2, x4, 5
	or	x9, x14, x2
	xor	x14, x13, x15
	xor	x19, x14, x3
	lw	x14, x7, 32
	lw	x2, x7, 72
	xor	x17, x17, x31
	xor	x14, x17, x14
	xor	x14, x14, x2
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x7, x17, 64
	srli	x14, x3, 2
	slli	x31, x3, 30
	or	x14, x14, x31
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x31, x3, 5
	or	x9, x16, x31
	xor	x16, x14, x13
	xor	x19, x16, x4
	lw	x16, x7, 36
	lw	x31, x7, 76
	xor	x18, x18, x1
	xor	x16, x18, x16
	xor	x16, x16, x31
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 68
	srli	x16, x4, 2
	slli	x1, x4, 30
	or	x16, x16, x1
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x16, x14
	xor	x19, x15, x3
	lw	x15, x7, 40
	lw	x1, x7, 80
	xor	x29, x29, x2
	xor	x15, x29, x15
	xor	x15, x15, x1
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 72
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x2, x3, 5
	or	x9, x13, x2
	xor	x13, x15, x16
	xor	x19, x13, x4
	lw	x13, x7, 44
	lw	x2, x7, 84
	xor	x17, x17, x31
	xor	x13, x17, x13
	xor	x13, x13, x2
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 76
	srli	x13, x4, 2
	slli	x31, x4, 30
	or	x13, x13, x31
	add	x14, x17, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x31, x4, 5
	or	x9, x14, x31
	xor	x14, x13, x15
	xor	x19, x14, x3
	lw	x14, x7, 48
	lw	x31, x7, 88
	xor	x18, x18, x1
	xor	x14, x18, x14
	xor	x14, x14, x31
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 80
	srli	x14, x3, 2
	slli	x1, x3, 30
	or	x14, x14, x1
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x14, x13
	xor	x19, x16, x4
	lw	x16, x7, 52
	lw	x1, x7, 28
	xor	x29, x29, x2
	xor	x16, x29, x16
	xor	x16, x16, x1
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 84
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x2, x4, 5
	or	x9, x15, x2
	xor	x15, x16, x14
	xor	x19, x15, x3
	lw	x15, x7, 56
	lw	x2, x7, 32
	xor	x17, x17, x31
	xor	x15, x17, x15
	xor	x15, x15, x2
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 88
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x3, x13, x8
	srli	x13, x3, 27
	slli	x31, x3, 5
	or	x9, x13, x31
	xor	x13, x15, x16
	xor	x19, x13, x4
	lw	x13, x7, 60
	lw	x31, x7, 36
	xor	x18, x18, x1
	xor	x13, x18, x13
	xor	x13, x13, x31
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 28
	srli	x13, x4, 2
	slli	x1, x4, 30
	or	x13, x13, x1
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x4, x14, x8
	srli	x14, x4, 27
	slli	x1, x4, 5
	or	x9, x14, x1
	xor	x14, x13, x15
	xor	x19, x14, x3
	lw	x14, x7, 64
	lw	x1, x7, 40
	xor	x29, x29, x2
	xor	x14, x29, x14
	xor	x14, x14, x1
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 32
	srli	x14, x3, 2
	slli	x2, x3, 30
	or	x14, x14, x2
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x2, x3, 5
	or	x9, x16, x2
	xor	x16, x14, x13
	xor	x19, x16, x4
	lw	x16, x7, 68
	lw	x2, x7, 44
	xor	x17, x17, x31
	xor	x16, x17, x16
	xor	x16, x16, x2
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x31, x17, x16
	sw	x7, x31, 36
	srli	x16, x4, 2
	slli	x17, x4, 30
	or	x16, x16, x17
	add	x15, x31, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x17, x4, 5
	or	x9, x15, x17
	xor	x15, x16, x14
	xor	x19, x15, x3
	lw	x17, x7, 72
	lw	x15, x7, 48
	xor	x18, x18, x1
	xor	x17, x18, x17
	xor	x17, x17, x15
	slli	x18, x17, 1
	srli	x17, x17, 31
	or	x1, x18, x17
	sw	x7, x1, 40
	srli	x17, x3, 2
	slli	x18, x3, 30
	or	x17, x17, x18
	add	x13, x1, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x9, x13, x8
	srli	x13, x9, 27
	slli	x18, x9, 5
	or	x19, x13, x18
	xor	x13, x17, x16
	xor	x20, x13, x4
	lw	x13, x7, 76
	lw	x3, x7, 52
	xor	x18, x29, x2
	xor	x13, x18, x13
	xor	x13, x13, x3
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x13, x18, x13
	sw	x7, x13, 44
	srli	x18, x4, 2
	slli	x29, x4, 30
	or	x18, x18, x29
	add	x14, x13, x14
	add	x14, x14, x20
	add	x14, x14, x19
	add	x19, x14, x8
	srli	x14, x19, 27
	slli	x29, x19, 5
	or	x29, x14, x29
	xor	x14, x18, x17
	xor	x2, x14, x9
	lw	x14, x7, 80
	lw	x4, x7, 56
	xor	x15, x31, x15
	xor	x14, x15, x14
	xor	x14, x14, x4
	slli	x15, x14, 1
	srli	x14, x14, 31
	or	x14, x15, x14
	sw	x7, x14, 48
	srli	x15, x9, 2
	slli	x31, x9, 30
	or	x15, x15, x31
	add	x16, x14, x16
	add	x16, x16, x2
	add	x16, x16, x29
	add	x20, x16, x8
	srli	x16, x20, 27
	slli	x29, x20, 5
	or	x31, x16, x29
	xor	x16, x15, x18
	xor	x9, x16, x19
	lw	x16, x7, 84
	lw	x2, x7, 60
	xor	x29, x1, x3
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 52
	srli	x16, x19, 2
	slli	x1, x19, 30
	or	x16, x16, x1
	add	x17, x29, x17
	add	x17, x17, x9
	add	x17, x17, x31
	add	x9, x17, x8
	srli	x17, x9, 27
	slli	x31, x9, 5
	or	x1, x17, x31
	xor	x17, x16, x15
	xor	x3, x17, x20
	lw	x17, x7, 88
	lw	x31, x7, 64
	xor	x13, x13, x4
	xor	x13, x13, x17
	xor	x13, x13, x31
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 56
	srli	x13, x20, 2
	slli	x4, x20, 30
	or	x13, x13, x4
	add	x18, x17, x18
	add	x18, x18, x3
	add	x18, x18, x1
	add	x3, x18, x8
	srli	x8, x3, 27
	slli	x18, x3, 5
	or	x4, x8, x18
	xor	x8, x13, x16
	and	x8, x9, x8
	and	x18, x13, x16
	xor	x19, x8, x18
	lw	x8, x7, 28
	lw	x1, x7, 68
	xor	x14, x14, x2
	xor	x8, x14, x8
	xor	x8, x8, x1
	slli	x14, x8, 1
	srli	x8, x8, 31
	or	x18, x14, x8
	sw	x7, x18, 60
	movimm 	x8, 	-1894007588
	srli	x14, x9, 2
	slli	x2, x9, 30
	or	x14, x14, x2
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x4
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x2, x4, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x2, x14, x13
	xor	x19, x15, x2
	lw	x15, x7, 32
	lw	x2, x7, 72
	xor	x29, x29, x31
	xor	x15, x29, x15
	xor	x15, x15, x2
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 64
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x31, x3, 5
	or	x9, x16, x31
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x31, x15, x14
	xor	x19, x16, x31
	lw	x16, x7, 36
	lw	x31, x7, 76
	xor	x17, x17, x1
	xor	x16, x17, x16
	xor	x16, x16, x31
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 68
	srli	x16, x4, 2
	slli	x1, x4, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x1, x4, 5
	or	x9, x13, x1
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x1, x16, x15
	xor	x19, x13, x1
	lw	x13, x7, 40
	lw	x1, x7, 80
	xor	x18, x18, x2
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 72
	srli	x13, x3, 2
	slli	x2, x3, 30
	or	x13, x13, x2
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x2, x3, 5
	or	x9, x14, x2
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x2, x13, x16
	xor	x19, x14, x2
	lw	x14, x7, 44
	lw	x2, x7, 84
	xor	x29, x29, x31
	xor	x14, x29, x14
	xor	x14, x14, x2
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 76
	srli	x14, x4, 2
	slli	x31, x4, 30
	or	x14, x14, x31
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x31, x14, x13
	xor	x19, x15, x31
	lw	x15, x7, 48
	lw	x31, x7, 88
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x31
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 80
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x1, x15, x14
	xor	x19, x16, x1
	lw	x16, x7, 52
	lw	x1, x7, 28
	xor	x18, x18, x2
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 84
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x2, x4, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x2, x16, x15
	xor	x19, x13, x2
	lw	x13, x7, 56
	lw	x2, x7, 32
	xor	x29, x29, x31
	xor	x13, x29, x13
	xor	x13, x13, x2
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 88
	srli	x13, x3, 2
	slli	x31, x3, 30
	or	x13, x13, x31
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x31, x3, 5
	or	x9, x14, x31
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x31, x13, x16
	xor	x19, x14, x31
	lw	x14, x7, 60
	lw	x31, x7, 36
	xor	x17, x17, x1
	xor	x14, x17, x14
	xor	x14, x14, x31
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x7, x17, 28
	srli	x14, x4, 2
	slli	x1, x4, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x1, x14, x13
	xor	x19, x15, x1
	lw	x15, x7, 64
	lw	x1, x7, 40
	xor	x18, x18, x2
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 32
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x2, x3, 5
	or	x9, x16, x2
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x2, x15, x14
	xor	x19, x16, x2
	lw	x16, x7, 68
	lw	x2, x7, 44
	xor	x29, x29, x31
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 36
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x31, x4, 5
	or	x9, x13, x31
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x31, x16, x15
	xor	x19, x13, x31
	lw	x13, x7, 72
	lw	x31, x7, 48
	xor	x17, x17, x1
	xor	x13, x17, x13
	xor	x13, x13, x31
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 40
	srli	x13, x3, 2
	slli	x1, x3, 30
	or	x13, x13, x1
	add	x14, x17, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x1, x3, 5
	or	x9, x14, x1
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x1, x13, x16
	xor	x19, x14, x1
	lw	x14, x7, 76
	lw	x1, x7, 52
	xor	x18, x18, x2
	xor	x14, x18, x14
	xor	x14, x14, x1
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 44
	srli	x14, x4, 2
	slli	x2, x4, 30
	or	x14, x14, x2
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x2, x4, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x2, x14, x13
	xor	x19, x15, x2
	lw	x15, x7, 80
	lw	x2, x7, 56
	xor	x29, x29, x31
	xor	x15, x29, x15
	xor	x15, x15, x2
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 48
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x31, x3, 5
	or	x9, x16, x31
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x31, x15, x14
	xor	x19, x16, x31
	lw	x16, x7, 84
	lw	x31, x7, 60
	xor	x17, x17, x1
	xor	x16, x17, x16
	xor	x16, x16, x31
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 52
	srli	x16, x4, 2
	slli	x1, x4, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x1, x4, 5
	or	x9, x13, x1
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x1, x16, x15
	xor	x19, x13, x1
	lw	x13, x7, 88
	lw	x1, x7, 64
	xor	x18, x18, x2
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 56
	srli	x13, x3, 2
	slli	x2, x3, 30
	or	x13, x13, x2
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x2, x3, 5
	or	x9, x14, x2
	xor	x14, x13, x16
	and	x14, x4, x14
	and	x2, x13, x16
	xor	x19, x14, x2
	lw	x14, x7, 28
	lw	x2, x7, 68
	xor	x29, x29, x31
	xor	x14, x29, x14
	xor	x14, x14, x2
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 60
	srli	x14, x4, 2
	slli	x31, x4, 30
	or	x14, x14, x31
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	and	x15, x3, x15
	and	x31, x14, x13
	xor	x19, x15, x31
	lw	x15, x7, 32
	lw	x31, x7, 72
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x31
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 64
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	and	x16, x4, x16
	and	x1, x15, x14
	xor	x19, x16, x1
	lw	x16, x7, 36
	lw	x1, x7, 76
	xor	x18, x18, x2
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 68
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x2, x4, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	and	x13, x3, x13
	and	x2, x16, x15
	xor	x19, x13, x2
	lw	x13, x7, 40
	lw	x2, x7, 80
	xor	x29, x29, x31
	xor	x13, x29, x13
	xor	x13, x13, x2
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 72
	srli	x13, x3, 2
	slli	x31, x3, 30
	or	x13, x13, x31
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x8, x3, 27
	slli	x14, x3, 5
	or	x9, x8, x14
	xor	x8, x13, x16
	xor	x19, x8, x4
	lw	x8, x7, 44
	lw	x31, x7, 84
	xor	x14, x17, x1
	xor	x8, x14, x8
	xor	x8, x8, x31
	slli	x14, x8, 1
	srli	x8, x8, 31
	or	x17, x14, x8
	sw	x7, x17, 76
	movimm 	x8, 	-899497514
	srli	x14, x4, 2
	slli	x1, x4, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 48
	lw	x1, x7, 88
	xor	x18, x18, x2
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 80
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x2, x3, 5
	or	x9, x16, x2
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 52
	lw	x2, x7, 28
	xor	x29, x29, x31
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 84
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x31, x4, 5
	or	x9, x13, x31
	xor	x13, x16, x15
	xor	x19, x13, x3
	lw	x13, x7, 56
	lw	x31, x7, 32
	xor	x17, x17, x1
	xor	x13, x17, x13
	xor	x13, x13, x31
	slli	x17, x13, 1
	srli	x13, x13, 31
	or	x17, x17, x13
	sw	x7, x17, 88
	srli	x13, x3, 2
	slli	x1, x3, 30
	or	x13, x13, x1
	add	x14, x17, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x1, x3, 5
	or	x9, x14, x1
	xor	x14, x13, x16
	xor	x19, x14, x4
	lw	x14, x7, 60
	lw	x1, x7, 36
	xor	x18, x18, x2
	xor	x14, x18, x14
	xor	x14, x14, x1
	slli	x18, x14, 1
	srli	x14, x14, 31
	or	x18, x18, x14
	sw	x7, x18, 28
	srli	x14, x4, 2
	slli	x2, x4, 30
	or	x14, x14, x2
	add	x15, x18, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x2, x4, 5
	or	x9, x15, x2
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 64
	lw	x2, x7, 40
	xor	x29, x29, x31
	xor	x15, x29, x15
	xor	x15, x15, x2
	slli	x29, x15, 1
	srli	x15, x15, 31
	or	x29, x29, x15
	sw	x7, x29, 32
	srli	x15, x3, 2
	slli	x31, x3, 30
	or	x15, x15, x31
	add	x16, x29, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x31, x3, 5
	or	x9, x16, x31
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 68
	lw	x31, x7, 44
	xor	x17, x17, x1
	xor	x16, x17, x16
	xor	x16, x16, x31
	slli	x17, x16, 1
	srli	x16, x16, 31
	or	x17, x17, x16
	sw	x7, x17, 36
	srli	x16, x4, 2
	slli	x1, x4, 30
	or	x16, x16, x1
	add	x13, x17, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x1, x4, 5
	or	x9, x13, x1
	xor	x13, x16, x15
	xor	x19, x13, x3
	lw	x13, x7, 72
	lw	x1, x7, 48
	xor	x18, x18, x2
	xor	x13, x18, x13
	xor	x13, x13, x1
	slli	x18, x13, 1
	srli	x13, x13, 31
	or	x18, x18, x13
	sw	x7, x18, 40
	srli	x13, x3, 2
	slli	x2, x3, 30
	or	x13, x13, x2
	add	x14, x18, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x2, x3, 5
	or	x9, x14, x2
	xor	x14, x13, x16
	xor	x19, x14, x4
	lw	x14, x7, 76
	lw	x2, x7, 52
	xor	x29, x29, x31
	xor	x14, x29, x14
	xor	x14, x14, x2
	slli	x29, x14, 1
	srli	x14, x14, 31
	or	x29, x29, x14
	sw	x7, x29, 44
	srli	x14, x4, 2
	slli	x31, x4, 30
	or	x14, x14, x31
	add	x15, x29, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x31, x4, 5
	or	x9, x15, x31
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 80
	lw	x31, x7, 56
	xor	x17, x17, x1
	xor	x15, x17, x15
	xor	x15, x15, x31
	slli	x17, x15, 1
	srli	x15, x15, 31
	or	x17, x17, x15
	sw	x7, x17, 48
	srli	x15, x3, 2
	slli	x1, x3, 30
	or	x15, x15, x1
	add	x16, x17, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x3, x16, x8
	srli	x16, x3, 27
	slli	x1, x3, 5
	or	x9, x16, x1
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 84
	lw	x1, x7, 60
	xor	x18, x18, x2
	xor	x16, x18, x16
	xor	x16, x16, x1
	slli	x18, x16, 1
	srli	x16, x16, 31
	or	x18, x18, x16
	sw	x7, x18, 52
	srli	x16, x4, 2
	slli	x2, x4, 30
	or	x16, x16, x2
	add	x13, x18, x13
	add	x13, x13, x19
	add	x13, x13, x9
	add	x4, x13, x8
	srli	x13, x4, 27
	slli	x2, x4, 5
	or	x9, x13, x2
	xor	x13, x16, x15
	xor	x19, x13, x3
	lw	x13, x7, 88
	lw	x2, x7, 64
	xor	x29, x29, x31
	xor	x13, x29, x13
	xor	x13, x13, x2
	slli	x29, x13, 1
	srli	x13, x13, 31
	or	x29, x29, x13
	sw	x7, x29, 56
	srli	x13, x3, 2
	slli	x31, x3, 30
	or	x13, x13, x31
	add	x14, x29, x14
	add	x14, x14, x19
	add	x14, x14, x9
	add	x3, x14, x8
	srli	x14, x3, 27
	slli	x31, x3, 5
	or	x9, x14, x31
	xor	x14, x13, x16
	xor	x19, x14, x4
	lw	x14, x7, 28
	lw	x31, x7, 68
	xor	x17, x17, x1
	xor	x14, x17, x14
	xor	x14, x14, x31
	slli	x17, x14, 1
	srli	x14, x14, 31
	or	x17, x17, x14
	sw	x7, x17, 60
	srli	x14, x4, 2
	slli	x1, x4, 30
	or	x14, x14, x1
	add	x15, x17, x15
	add	x15, x15, x19
	add	x15, x15, x9
	add	x4, x15, x8
	srli	x15, x4, 27
	slli	x1, x4, 5
	or	x9, x15, x1
	xor	x15, x14, x13
	xor	x19, x15, x3
	lw	x15, x7, 32
	lw	x1, x7, 72
	xor	x18, x18, x2
	xor	x15, x18, x15
	xor	x15, x15, x1
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 64
	srli	x15, x3, 2
	slli	x2, x3, 30
	or	x15, x15, x2
	add	x16, x18, x16
	add	x16, x16, x19
	add	x16, x16, x9
	add	x9, x16, x8
	srli	x16, x9, 27
	slli	x2, x9, 5
	or	x3, x16, x2
	xor	x16, x15, x14
	xor	x19, x16, x4
	lw	x16, x7, 36
	lw	x2, x7, 76
	xor	x29, x29, x31
	xor	x16, x29, x16
	xor	x16, x16, x2
	slli	x29, x16, 1
	srli	x16, x16, 31
	or	x29, x29, x16
	sw	x7, x29, 68
	srli	x16, x4, 2
	slli	x31, x4, 30
	or	x16, x16, x31
	add	x13, x29, x13
	add	x13, x13, x19
	add	x13, x13, x3
	add	x13, x13, x8
	srli	x31, x13, 27
	slli	x3, x13, 5
	or	x4, x31, x3
	xor	x31, x16, x15
	xor	x19, x31, x9
	lw	x31, x7, 40
	lw	x3, x7, 80
	xor	x17, x17, x1
	xor	x17, x17, x31
	xor	x17, x17, x3
	slli	x31, x17, 1
	srli	x17, x17, 31
	or	x31, x31, x17
	sw	x7, x31, 72
	srli	x17, x9, 2
	slli	x1, x9, 30
	or	x17, x17, x1
	add	x14, x31, x14
	add	x14, x14, x19
	add	x14, x14, x4
	add	x14, x14, x8
	srli	x1, x14, 27
	slli	x4, x14, 5
	or	x4, x1, x4
	xor	x1, x17, x16
	xor	x9, x1, x13
	lw	x19, x7, 44
	lw	x1, x7, 84
	xor	x18, x18, x2
	xor	x18, x18, x19
	xor	x18, x18, x1
	slli	x2, x18, 1
	srli	x18, x18, 31
	or	x18, x2, x18
	sw	x7, x18, 76
	srli	x2, x13, 2
	slli	x13, x13, 30
	or	x13, x2, x13
	add	x15, x18, x15
	add	x15, x15, x9
	add	x15, x15, x4
	add	x2, x15, x8
	srli	x15, x2, 27
	slli	x4, x2, 5
	or	x4, x15, x4
	xor	x15, x13, x17
	xor	x9, x15, x14
	lw	x19, x7, 48
	lw	x15, x7, 88
	xor	x29, x29, x3
	xor	x29, x29, x19
	xor	x29, x29, x15
	slli	x3, x29, 1
	srli	x29, x29, 31
	or	x29, x3, x29
	sw	x7, x29, 80
	srli	x3, x14, 2
	slli	x14, x14, 30
	or	x14, x3, x14
	add	x16, x29, x16
	add	x16, x16, x9
	add	x16, x16, x4
	add	x16, x16, x8
	srli	x29, x16, 27
	slli	x3, x16, 5
	or	x3, x29, x3
	xor	x29, x14, x13
	xor	x4, x29, x2
	lw	x29, x7, 52
	lw	x9, x7, 28
	xor	x31, x31, x1
	xor	x29, x31, x29
	xor	x29, x29, x9
	slli	x31, x29, 1
	srli	x29, x29, 31
	or	x31, x31, x29
	sw	x7, x31, 84
	srli	x29, x2, 2
	slli	x1, x2, 30
	or	x29, x29, x1
	add	x17, x31, x17
	add	x17, x17, x4
	add	x17, x17, x3
	add	x17, x17, x8
	srli	x31, x17, 27
	slli	x1, x17, 5
	or	x31, x31, x1
	xor	x1, x29, x14
	xor	x1, x1, x16
	lw	x2, x7, 56
	lw	x3, x7, 32
	xor	x15, x18, x15
	xor	x15, x15, x2
	xor	x15, x15, x3
	slli	x18, x15, 1
	srli	x15, x15, 31
	or	x18, x18, x15
	sw	x7, x18, 88
	srli	x15, x16, 2
	slli	x16, x16, 30
	or	x15, x15, x16
	lw	x16, x7, 8
	add	x16, x16, x18
	add	x13, x16, x13
	add	x13, x13, x1
	add	x13, x13, x31
	add	x8, x13, x8
	sw	x7, x8, 8
	lw	x8, x7, 12
	add	x8, x17, x8
	sw	x7, x8, 12
	lw	x8, x7, 16
	add	x8, x15, x8
	sw	x7, x8, 16
	lw	x8, x7, 20
	add	x8, x29, x8
	sw	x7, x8, 20
	lw	x8, x7, 24
	add	x8, x14, x8
	sw	x7, x8, 24
	addi	x8, x0, 0
.LBB12_120:
	addi	x7, x8, 0
	addi	x8, x30, 64
	andi	x13, x7, -4
	add	x8, x8, x13
	lw	x8, x8, 8
	slli	x13, x7, 3
	andi	x13, x13, 24
	xori	x13, x13, 24
	srl	x13, x8, x13
	addi	x8, x7, 1
	movimm 	x14, 	20
	add	x7, x6, x7
	sb	x7, x13, 12
	bne	x8, x14, .LBB12_120
	jal x0,	.LBB12_130
.LBB12_130:
	sub	x7, x10, x11
	lw	x11, x6, 0
	addi	x8, x30, 160
	sw	x8, x11, 0
	lw	x11, x6, 4
	sw	x8, x11, 4
	lw	x11, x6, 8
	sw	x8, x11, 8
	lbu	x11, x6, 12
	lbu	x13, x6, 13
	lbu	x14, x6, 14
	lbu	x15, x6, 15
	lbu	x16, x6, 16
	lbu	x17, x6, 17
	lbu	x18, x6, 18
	lbu	x29, x6, 19
	lbu	x31, x6, 20
	lbu	x1, x6, 21
	lbu	x2, x6, 22
	lbu	x3, x6, 23
	lbu	x4, x6, 24
	lbu	x9, x6, 25
	lbu	x10, x6, 26
	lbu	x19, x6, 27
	lbu	x20, x6, 28
	lbu	x21, x6, 29
	lbu	x22, x6, 30
	lbu	x6, x6, 31
	sb	x8, x11, 12
	sb	x8, x13, 13
	sb	x8, x14, 14
	sb	x8, x15, 15
	sb	x8, x16, 16
	sb	x8, x17, 17
	sb	x8, x18, 18
	sb	x8, x29, 19
	sb	x8, x31, 20
	sb	x8, x1, 21
	sb	x8, x2, 22
	sb	x8, x3, 23
	sb	x8, x4, 24
	sb	x8, x9, 25
	sb	x8, x10, 26
	sb	x8, x19, 27
	sb	x8, x20, 28
	sb	x8, x21, 29
	sb	x8, x22, 30
	sb	x8, x6, 31
	falloc	x6, x0
	movaddr 	x8, 	.HyOp#14
	fbind 	x8, 	x6
	falloc	x8, x0
	movaddr 	x11, 	.HyOp#6
	fbind 	x11, 	x8
	movaddr 	x11, 	"ga#44"
	movimm 	x13, 	192
	srli	x14, x6, 22
	srli	x15, x6, 12
	andi	x15, x15, 15
	srli	x16, x6, 6
	andi	x16, x16, 63
	addi	x17, x0, 832
	mul	x14, x14, x17
	addi	x17, x0, 52
	mul	x15, x15, x17
	add	x15, x15, x16
	add	x14, x15, x14
	mul	x13, x14, x13
	add	x11, x11, x13
	addi	x13, x0, 0
.LBB12_140:
	addi	x14, x30, 160
	add	x14, x14, x13
	lbu	x14, x14, 0
	add	x15, x11, x13
	addi	x13, x13, 1
	movimm 	x16, 	32
	sb	x15, x14, 0
	bne	x13, x16, .LBB12_140
	jal x0,	.LBB12_150
.LBB12_150:
	writecm	x6, x8, 0
	writecm	x8, x7, 20
	writecm	x8, x12, 16
	end x0
.DATA:
	.word32	0
	.word32	4
	.word32	6
	.word32	1
	.word32	0
	.word32	1
	.float	0.5
	.float	0.234375
	.float	4
	.word32	0
	.word32	1
.END:
;FS = 192
