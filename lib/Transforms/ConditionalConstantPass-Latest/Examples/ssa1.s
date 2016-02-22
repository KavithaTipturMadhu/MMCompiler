	.file	"ssa1.ll"
	.text
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
# BB#0:                                 # %entry
	xorl	%eax, %eax
	jmp	.LBB0_1
	.align	16, 0x90
.LBB0_3:                                # %if.else
                                        #   in Loop: Header=BB0_1 Depth=1
	addl	%eax, %eax
.LBB0_1:                                # %while.body
                                        # =>This Inner Loop Header: Depth=1
	cmpl	$101, %eax
	jl	.LBB0_3
# BB#2:                                 # %if.then
                                        #   in Loop: Header=BB0_1 Depth=1
	leal	(%eax,%eax,2), %eax
	jmp	.LBB0_1
.Ltmp0:
	.size	main, .Ltmp0-main


	.section	".note.GNU-stack","",@progbits
