	.file	"/home/kavitha/Desktop/riscv-workspace/redefine-llvm-tests/test2/test3.ll"
	.text
	.globl	test
	.align	4
	.type	test,@function
test:                                   # @test

pHyperOp:
.LBB0_0:
	addi	x5, x0, 0
	sw	x1, 0(x5)
	end 	0
	addi	x0, x0, 0
	addi	x0, x0, 0

	.section	".note.GNU-stack","",@progbits
