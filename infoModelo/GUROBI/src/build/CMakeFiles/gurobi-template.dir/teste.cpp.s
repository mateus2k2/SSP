	.file	"teste.cpp"
	.text
	.p2align 4
	.type	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0, @function
_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0:
.LFB2994:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	movq	(%rdi), %rax
	movq	-24(%rax), %rax
	movq	240(%rdi,%rax), %r12
	testq	%r12, %r12
	je	.L7
	cmpb	$0, 56(%r12)
	movq	%rdi, %rbp
	je	.L3
	movsbl	67(%r12), %esi
.L4:
	movq	%rbp, %rdi
	call	_ZNSo3putEc@PLT
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rax, %rdi
	popq	%r12
	.cfi_def_cfa_offset 8
	jmp	_ZNSo5flushEv@PLT
.L3:
	.cfi_restore_state
	movq	%r12, %rdi
	call	_ZNKSt5ctypeIcE13_M_widen_initEv@PLT
	movq	(%r12), %rax
	movl	$10, %esi
	movq	%r12, %rdi
	call	*48(%rax)
	movsbl	%al, %esi
	jmp	.L4
.L7:
	call	_ZSt16__throw_bad_castv@PLT
	.cfi_endproc
.LFE2994:
	.size	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0, .-_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0
	.section	.text._ZN10GRBLinExprD2Ev,"axG",@progbits,_ZN10GRBLinExprD5Ev,comdat
	.align 2
	.p2align 4
	.weak	_ZN10GRBLinExprD2Ev
	.type	_ZN10GRBLinExprD2Ev, @function
_ZN10GRBLinExprD2Ev:
.LFB2225:
	.cfi_startproc
	endbr64
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	movq	32(%rdi), %rdi
	testq	%rdi, %rdi
	je	.L9
	movq	48(%rbx), %rsi
	subq	%rdi, %rsi
	call	_ZdlPvm@PLT
.L9:
	movq	8(%rbx), %rdi
	testq	%rdi, %rdi
	je	.L8
	movq	24(%rbx), %rsi
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	subq	%rdi, %rsi
	jmp	_ZdlPvm@PLT
	.p2align 4,,10
	.p2align 3
.L8:
	.cfi_restore_state
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE2225:
	.size	_ZN10GRBLinExprD2Ev, .-_ZN10GRBLinExprD2Ev
	.weak	_ZN10GRBLinExprD1Ev
	.set	_ZN10GRBLinExprD1Ev,_ZN10GRBLinExprD2Ev
	.section	.text._ZN11GRBQuadExprD2Ev,"axG",@progbits,_ZN11GRBQuadExprD5Ev,comdat
	.align 2
	.p2align 4
	.weak	_ZN11GRBQuadExprD2Ev
	.type	_ZN11GRBQuadExprD2Ev, @function
_ZN11GRBQuadExprD2Ev:
.LFB2227:
	.cfi_startproc
	endbr64
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	movq	112(%rdi), %rdi
	testq	%rdi, %rdi
	je	.L16
	movq	128(%rbx), %rsi
	subq	%rdi, %rsi
	call	_ZdlPvm@PLT
.L16:
	movq	88(%rbx), %rdi
	testq	%rdi, %rdi
	je	.L17
	movq	104(%rbx), %rsi
	subq	%rdi, %rsi
	call	_ZdlPvm@PLT
.L17:
	movq	64(%rbx), %rdi
	testq	%rdi, %rdi
	je	.L18
	movq	80(%rbx), %rsi
	subq	%rdi, %rsi
	call	_ZdlPvm@PLT
.L18:
	movq	40(%rbx), %rdi
	testq	%rdi, %rdi
	je	.L19
	movq	56(%rbx), %rsi
	subq	%rdi, %rsi
	call	_ZdlPvm@PLT
.L19:
	movq	16(%rbx), %rdi
	testq	%rdi, %rdi
	je	.L15
	movq	32(%rbx), %rsi
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	subq	%rdi, %rsi
	jmp	_ZdlPvm@PLT
	.p2align 4,,10
	.p2align 3
.L15:
	.cfi_restore_state
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE2227:
	.size	_ZN11GRBQuadExprD2Ev, .-_ZN11GRBQuadExprD2Ev
	.weak	_ZN11GRBQuadExprD1Ev
	.set	_ZN11GRBQuadExprD1Ev,_ZN11GRBQuadExprD2Ev
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"mip1.log"
.LC1:
	.string	"x"
.LC4:
	.string	"y"
.LC5:
	.string	"z"
.LC9:
	.string	"c0"
.LC10:
	.string	"c1"
.LC11:
	.string	" "
.LC12:
	.string	"Obj: "
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC13:
	.string	"basic_string::_M_construct null not valid"
	.section	.rodata.str1.1
.LC14:
	.string	"Error code = "
.LC15:
	.string	"Exception during optimization"
	.section	.text.unlikely,"ax",@progbits
.LCOLDB16:
	.section	.text.startup,"ax",@progbits
.LHOTB16:
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB2235:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA2235
	endbr64
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	movl	$1, %esi
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$1176, %rsp
	.cfi_def_cfa_offset 1232
	movq	%fs:40, %rax
	movq	%rax, 1160(%rsp)
	xorl	%eax, %eax
	leaq	144(%rsp), %rbx
	movq	%rbx, %rdi
	movq	%rbx, 40(%rsp)
.LEHB0:
	call	_ZN6GRBEnvC1Eb@PLT
.LEHE0:
	leaq	1136(%rsp), %r15
	movl	$27753, %eax
	movq	%rbx, %rdi
	movl	$1181183820, 1136(%rsp)
	leaq	1120(%rsp), %rsi
	leaq	.LC0(%rip), %rdx
	movq	%r15, 16(%rsp)
	movq	%rsi, 24(%rsp)
	movq	%r15, 1120(%rsp)
	movw	%ax, 1140(%rsp)
	movb	$101, 1142(%rsp)
	movq	$7, 1128(%rsp)
	movb	$0, 1143(%rsp)
.LEHB1:
	call	_ZN6GRBEnv3setERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEPKc@PLT
.LEHE1:
	movq	1120(%rsp), %rdi
	cmpq	%r15, %rdi
	je	.L35
	movq	1136(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
.L35:
	movq	40(%rsp), %rbx
	movq	%rbx, %rdi
.LEHB2:
	call	_ZN6GRBEnv5startEv@PLT
	leaq	912(%rsp), %rbp
	movq	%rbx, %rsi
	movq	%rbp, %rdi
	call	_ZN8GRBModelC1ERK6GRBEnv@PLT
.LEHE2:
	leaq	88(%rsp), %rbx
	pxor	%xmm2, %xmm2
	pxor	%xmm0, %xmm0
	movq	%rbp, %rsi
	movq	.LC2(%rip), %rax
	movl	$66, %edx
	movq	%rbx, %rdi
	movq	%rbx, 56(%rsp)
	leaq	.LC1(%rip), %rcx
	movq	%rax, %xmm1
.LEHB3:
	call	_ZN8GRBModel6addVarEdddcPKc@PLT
	movq	.LC2(%rip), %rsi
	leaq	96(%rsp), %rax
	leaq	.LC4(%rip), %rcx
	movl	$66, %edx
	pxor	%xmm2, %xmm2
	pxor	%xmm0, %xmm0
	movq	%rax, %rdi
	movq	%rax, 64(%rsp)
	movq	%rsi, %xmm1
	movq	%rbp, %rsi
	call	_ZN8GRBModel6addVarEdddcPKc@PLT
	movq	.LC2(%rip), %rsi
	leaq	104(%rsp), %rax
	leaq	.LC5(%rip), %rcx
	movl	$66, %edx
	pxor	%xmm2, %xmm2
	pxor	%xmm0, %xmm0
	movq	%rax, %rdi
	movq	%rax, 72(%rsp)
	movq	%rsi, %xmm1
	movq	%rbp, %rsi
	call	_ZN8GRBModel6addVarEdddcPKc@PLT
	movq	104(%rsp), %rax
	leaq	624(%rsp), %r12
	leaq	416(%rsp), %r15
	movq	%r15, %rsi
	movq	%r12, %rdi
	movq	%rax, 416(%rsp)
	movq	.LC6(%rip), %rax
	movq	%rax, %xmm0
	call	_Zmld6GRBVar@PLT
.LEHE3:
	movq	96(%rsp), %rax
	leaq	480(%rsp), %r13
	leaq	288(%rsp), %rbx
	movq	%rbx, %rsi
	movq	%r13, %rdi
	movq	%rax, 352(%rsp)
	movq	88(%rsp), %rax
	movq	%rax, 288(%rsp)
	leaq	352(%rsp), %rax
	movq	%rax, %rdx
	movq	%rax, 8(%rsp)
.LEHB4:
	call	_Zpl6GRBVarS_@PLT
.LEHE4:
	leaq	768(%rsp), %r14
	movq	%r12, %rdx
	movq	%r13, %rsi
	movq	%r14, %rdi
.LEHB5:
	call	_ZN10GRBLinExprplERKS_@PLT
.LEHE5:
	movl	$-1, %edx
	movq	%r14, %rsi
	movq	%rbp, %rdi
.LEHB6:
	call	_ZN8GRBModel12setObjectiveE10GRBLinExpri@PLT
.LEHE6:
	movq	%r14, %rdi
	call	_ZN10GRBLinExprD1Ev
	movq	%r13, %rdi
	call	_ZN10GRBLinExprD1Ev
	movq	%r12, %rdi
	call	_ZN10GRBLinExprD1Ev
	movsd	.LC7(%rip), %xmm0
	movq	%r12, %rdi
.LEHB7:
	call	_ZN11GRBQuadExprC1Ed@PLT
.LEHE7:
	movq	104(%rsp), %rax
	movsd	.LC8(%rip), %xmm0
	leaq	128(%rsp), %rsi
	movq	8(%rsp), %rdi
	movq	%rax, 128(%rsp)
.LEHB8:
	call	_Zmld6GRBVar@PLT
.LEHE8:
	movq	.LC6(%rip), %rdx
	movq	96(%rsp), %rax
	leaq	112(%rsp), %rsi
	leaq	160(%rsp), %rcx
	movq	%rcx, %rdi
	movq	%rcx, 48(%rsp)
	movq	%rdx, %xmm0
	movq	%rax, 112(%rsp)
.LEHB9:
	call	_Zmld6GRBVar@PLT
.LEHE9:
	movq	.LC2(%rip), %rdx
	movq	88(%rsp), %rax
	leaq	120(%rsp), %rsi
	leaq	224(%rsp), %rcx
	movq	%rcx, %rdi
	movq	%rcx, 32(%rsp)
	movq	%rdx, %xmm0
	movq	%rax, 120(%rsp)
.LEHB10:
	call	_ZN10GRBLinExprC1E6GRBVard@PLT
.LEHE10:
	movq	48(%rsp), %rdx
	movq	32(%rsp), %rsi
	movq	%rbx, %rdi
.LEHB11:
	call	_ZplRK10GRBLinExprS1_@PLT
.LEHE11:
	movq	8(%rsp), %rdx
	movq	%rbx, %rsi
	movq	%r15, %rdi
.LEHB12:
	call	_ZN10GRBLinExprplERKS_@PLT
.LEHE12:
	movq	%r15, %rsi
	movq	%r13, %rdi
.LEHB13:
	call	_ZN11GRBQuadExprC1E10GRBLinExpr@PLT
.LEHE13:
	movq	%r12, %rdx
	movq	%r13, %rsi
	movq	%r14, %rdi
.LEHB14:
	call	_Zle11GRBQuadExprS_@PLT
.LEHE14:
	leaq	136(%rsp), %rdi
	leaq	.LC9(%rip), %rcx
	movq	%r14, %rdx
	movq	%rbp, %rsi
.LEHB15:
	call	_ZN8GRBModel9addConstrERK13GRBTempConstrPKc@PLT
.LEHE15:
	movq	%r14, %rdi
	call	_ZN11GRBQuadExprD1Ev
	movq	%r13, %rdi
	call	_ZN11GRBQuadExprD1Ev
	movq	%r15, %rdi
	call	_ZN10GRBLinExprD1Ev
	movq	%rbx, %rdi
	call	_ZN10GRBLinExprD1Ev
	movq	32(%rsp), %rdi
	call	_ZN10GRBLinExprD1Ev
	movq	48(%rsp), %rdi
	call	_ZN10GRBLinExprD1Ev
	movq	8(%rsp), %rdi
	call	_ZN10GRBLinExprD1Ev
	movq	%r12, %rdi
	call	_ZN11GRBQuadExprD1Ev
	movq	.LC2(%rip), %rcx
	movq	%r12, %rdi
	movq	%rcx, %xmm0
.LEHB16:
	call	_ZN11GRBQuadExprC1Ed@PLT
.LEHE16:
	movq	96(%rsp), %rax
	movq	32(%rsp), %rsi
	movq	%rbx, %rdx
	movq	%r15, %rdi
	movq	%rax, 288(%rsp)
	movq	88(%rsp), %rax
	movq	%rax, 224(%rsp)
.LEHB17:
	call	_Zpl6GRBVarS_@PLT
.LEHE17:
	movq	%r15, %rsi
	movq	%r13, %rdi
.LEHB18:
	call	_ZN11GRBQuadExprC1E10GRBLinExpr@PLT
.LEHE18:
	movq	%r12, %rdx
	movq	%r13, %rsi
	movq	%r14, %rdi
.LEHB19:
	call	_Zge11GRBQuadExprS_@PLT
.LEHE19:
	movq	8(%rsp), %rdi
	leaq	.LC10(%rip), %rcx
	movq	%r14, %rdx
	movq	%rbp, %rsi
.LEHB20:
	call	_ZN8GRBModel9addConstrERK13GRBTempConstrPKc@PLT
.LEHE20:
	movq	%r14, %rdi
	call	_ZN11GRBQuadExprD1Ev
	movq	%r13, %rdi
	call	_ZN11GRBQuadExprD1Ev
	movq	%r15, %rdi
	call	_ZN10GRBLinExprD1Ev
	movq	%r12, %rdi
	call	_ZN11GRBQuadExprD1Ev
	movq	%rbp, %rdi
.LEHB21:
	call	_ZN8GRBModel8optimizeEv@PLT
	movq	56(%rsp), %rbx
	movq	24(%rsp), %rdi
	movl	$1, %edx
	movq	%rbx, %rsi
	call	_ZNK6GRBVar3getB5cxx11E14GRB_StringAttr@PLT
.LEHE21:
	leaq	_ZSt4cout(%rip), %r12
	movq	1128(%rsp), %rdx
	movq	1120(%rsp), %rsi
	movq	%r12, %rdi
.LEHB22:
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	leaq	.LC11(%rip), %r14
	movl	$1, %edx
	movq	%rax, %rdi
	movq	%rax, %r13
	movq	%r14, %rsi
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movl	$32, %esi
	movq	%rbx, %rdi
	call	_ZNK6GRBVar3getE14GRB_DoubleAttr@PLT
	movq	%r13, %rdi
	call	_ZNSo9_M_insertIdEERSoT_@PLT
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0
.LEHE22:
	movq	1120(%rsp), %rdi
	cmpq	16(%rsp), %rdi
	je	.L36
	movq	1136(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
.L36:
	movq	64(%rsp), %rbx
	movq	24(%rsp), %rdi
	movl	$1, %edx
	movq	%rbx, %rsi
.LEHB23:
	call	_ZNK6GRBVar3getB5cxx11E14GRB_StringAttr@PLT
.LEHE23:
	movq	1128(%rsp), %rdx
	movq	1120(%rsp), %rsi
	movq	%r12, %rdi
.LEHB24:
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movl	$1, %edx
	movq	%r14, %rsi
	movq	%rax, %rdi
	movq	%rax, %r13
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movl	$32, %esi
	movq	%rbx, %rdi
	call	_ZNK6GRBVar3getE14GRB_DoubleAttr@PLT
	movq	%r13, %rdi
	call	_ZNSo9_M_insertIdEERSoT_@PLT
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0
.LEHE24:
	movq	1120(%rsp), %rdi
	cmpq	16(%rsp), %rdi
	je	.L37
	movq	1136(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
.L37:
	movq	72(%rsp), %rbx
	movq	24(%rsp), %rdi
	movl	$1, %edx
	movq	%rbx, %rsi
.LEHB25:
	call	_ZNK6GRBVar3getB5cxx11E14GRB_StringAttr@PLT
.LEHE25:
	movq	1128(%rsp), %rdx
	movq	1120(%rsp), %rsi
	movq	%r12, %rdi
.LEHB26:
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movl	$1, %edx
	movq	%r14, %rsi
	movq	%rax, %rdi
	movq	%rax, %r13
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movl	$32, %esi
	movq	%rbx, %rdi
	call	_ZNK6GRBVar3getE14GRB_DoubleAttr@PLT
	movq	%r13, %rdi
	call	_ZNSo9_M_insertIdEERSoT_@PLT
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0
.LEHE26:
	movq	1120(%rsp), %rdi
	cmpq	16(%rsp), %rdi
	je	.L38
	movq	1136(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
.L38:
	movl	$5, %edx
	leaq	.LC12(%rip), %rsi
	movq	%r12, %rdi
.LEHB27:
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movl	$26, %esi
	movq	%rbp, %rdi
	call	_ZNK8GRBModel3getE14GRB_DoubleAttr@PLT
	movq	%r12, %rdi
	call	_ZNSo9_M_insertIdEERSoT_@PLT
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0
.LEHE27:
	movq	%rbp, %rdi
	call	_ZN8GRBModelD1Ev@PLT
	movq	40(%rsp), %rdi
	call	_ZN6GRBEnvD1Ev@PLT
.L73:
	movq	1160(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L116
	addq	$1176, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.L116:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
.L101:
	endbr64
	movq	%rax, %r12
	movq	%rdx, %rbx
	jmp	.L62
.L100:
	endbr64
	movq	%rax, %r12
	movq	%rdx, %rbx
	jmp	.L60
.L99:
	endbr64
	movq	%rax, %r12
	movq	%rdx, %rbx
	jmp	.L58
.L98:
	endbr64
	movq	%rax, 8(%rsp)
	movq	%rdx, %rbx
	jmp	.L54
.L97:
	endbr64
	movq	%rax, %r14
	movq	%rdx, %rbx
	jmp	.L55
.L96:
	endbr64
	movq	%rax, %r13
	movq	%rdx, %rbx
	jmp	.L56
.L95:
	endbr64
	movq	%rax, %r13
	movq	%rdx, %rbx
	jmp	.L57
.L94:
	endbr64
	movq	%rax, 16(%rsp)
	movq	%rdx, 24(%rsp)
	jmp	.L46
.L93:
	endbr64
	movq	%rdx, %r14
	jmp	.L47
.L92:
	endbr64
	movq	%rdx, %r13
	jmp	.L48
.L91:
	endbr64
	movq	%rax, %r14
	movq	%rdx, %r13
	jmp	.L49
.L89:
	endbr64
	movq	%rax, %r13
	movq	%rdx, %rbx
	jmp	.L51
.L88:
	endbr64
	movq	%rax, %r13
	movq	%rdx, %rbx
	jmp	.L52
.L90:
	endbr64
	movq	%rax, %r14
	movq	%rdx, %rbx
	jmp	.L50
.L87:
	endbr64
	movq	%rax, %r13
	movq	%rdx, %rbx
	jmp	.L53
.L86:
	endbr64
	movq	%rax, %rbx
	movq	%rdx, %r15
	jmp	.L42
.L85:
	endbr64
	movq	%rax, %r14
	movq	%rdx, %rbx
	jmp	.L43
.L84:
	endbr64
	movq	%rax, %r14
	movq	%rdx, %rbx
	jmp	.L44
.L83:
	endbr64
	movq	%rax, %r12
	movq	%rdx, %rbx
	jmp	.L45
.L82:
	endbr64
	movq	%rax, %rbp
	movq	%rdx, %rbx
	jmp	.L41
.L81:
	endbr64
	movq	%rax, %rbp
	movq	%rdx, %rbx
	jmp	.L39
.L80:
	endbr64
	movq	%rax, %rbp
	movq	%rdx, %rbx
	jmp	.L64
	.globl	__gxx_personality_v0
	.section	.gcc_except_table,"a",@progbits
	.align 4
.LLSDA2235:
	.byte	0xff
	.byte	0x9b
	.uleb128 .LLSDATT2235-.LLSDATTD2235
.LLSDATTD2235:
	.byte	0x1
	.uleb128 .LLSDACSE2235-.LLSDACSB2235
.LLSDACSB2235:
	.uleb128 .LEHB0-.LFB2235
	.uleb128 .LEHE0-.LEHB0
	.uleb128 .L80-.LFB2235
	.uleb128 0x3
	.uleb128 .LEHB1-.LFB2235
	.uleb128 .LEHE1-.LEHB1
	.uleb128 .L81-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB2-.LFB2235
	.uleb128 .LEHE2-.LEHB2
	.uleb128 .L82-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB3-.LFB2235
	.uleb128 .LEHE3-.LEHB3
	.uleb128 .L83-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB4-.LFB2235
	.uleb128 .LEHE4-.LEHB4
	.uleb128 .L84-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB5-.LFB2235
	.uleb128 .LEHE5-.LEHB5
	.uleb128 .L85-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB6-.LFB2235
	.uleb128 .LEHE6-.LEHB6
	.uleb128 .L86-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB7-.LFB2235
	.uleb128 .LEHE7-.LEHB7
	.uleb128 .L83-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB8-.LFB2235
	.uleb128 .LEHE8-.LEHB8
	.uleb128 .L87-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB9-.LFB2235
	.uleb128 .LEHE9-.LEHB9
	.uleb128 .L88-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB10-.LFB2235
	.uleb128 .LEHE10-.LEHB10
	.uleb128 .L89-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB11-.LFB2235
	.uleb128 .LEHE11-.LEHB11
	.uleb128 .L90-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB12-.LFB2235
	.uleb128 .LEHE12-.LEHB12
	.uleb128 .L91-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB13-.LFB2235
	.uleb128 .LEHE13-.LEHB13
	.uleb128 .L92-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB14-.LFB2235
	.uleb128 .LEHE14-.LEHB14
	.uleb128 .L93-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB15-.LFB2235
	.uleb128 .LEHE15-.LEHB15
	.uleb128 .L94-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB16-.LFB2235
	.uleb128 .LEHE16-.LEHB16
	.uleb128 .L83-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB17-.LFB2235
	.uleb128 .LEHE17-.LEHB17
	.uleb128 .L95-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB18-.LFB2235
	.uleb128 .LEHE18-.LEHB18
	.uleb128 .L96-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB19-.LFB2235
	.uleb128 .LEHE19-.LEHB19
	.uleb128 .L97-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB20-.LFB2235
	.uleb128 .LEHE20-.LEHB20
	.uleb128 .L98-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB21-.LFB2235
	.uleb128 .LEHE21-.LEHB21
	.uleb128 .L83-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB22-.LFB2235
	.uleb128 .LEHE22-.LEHB22
	.uleb128 .L99-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB23-.LFB2235
	.uleb128 .LEHE23-.LEHB23
	.uleb128 .L83-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB24-.LFB2235
	.uleb128 .LEHE24-.LEHB24
	.uleb128 .L100-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB25-.LFB2235
	.uleb128 .LEHE25-.LEHB25
	.uleb128 .L83-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB26-.LFB2235
	.uleb128 .LEHE26-.LEHB26
	.uleb128 .L101-.LFB2235
	.uleb128 0x5
	.uleb128 .LEHB27-.LFB2235
	.uleb128 .LEHE27-.LEHB27
	.uleb128 .L83-.LFB2235
	.uleb128 0x5
.LLSDACSE2235:
	.byte	0x2
	.byte	0
	.byte	0x1
	.byte	0x7d
	.byte	0
	.byte	0x7d
	.align 4
	.long	0

	.long	DW.ref._ZTI12GRBException-.
.LLSDATT2235:
	.section	.text.startup
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDAC2235
	.type	main.cold, @function
main.cold:
.LFSB2235:
.L62:
	.cfi_def_cfa_offset 1232
	.cfi_offset 3, -56
	.cfi_offset 6, -48
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	movq	1120(%rsp), %rdi
	cmpq	16(%rsp), %rdi
	je	.L45
	movq	1136(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
.L45:
	movq	%rbp, %rdi
	movq	%r12, %rbp
	call	_ZN8GRBModelD1Ev@PLT
.L41:
	movq	40(%rsp), %rdi
	call	_ZN6GRBEnvD1Ev@PLT
.L64:
	subq	$1, %rbx
	movq	%rbp, %rdi
	je	.L117
	call	__cxa_begin_catch@PLT
	leaq	_ZSt4cout(%rip), %r12
	movl	$29, %edx
	leaq	.LC15(%rip), %rsi
	movq	%r12, %rdi
.LEHB28:
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movq	%r12, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0
.LEHE28:
.LEHB29:
	call	__cxa_end_catch@PLT
.LEHE29:
	jmp	.L73
.L60:
	movq	1120(%rsp), %rdi
	cmpq	16(%rsp), %rdi
	je	.L45
	movq	1136(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
	jmp	.L45
.L58:
	movq	1120(%rsp), %rdi
	cmpq	16(%rsp), %rdi
	je	.L45
	movq	1136(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
	jmp	.L45
.L54:
	movq	%r14, %rdi
	call	_ZN11GRBQuadExprD1Ev
	movq	8(%rsp), %r14
.L55:
	movq	%r13, %rdi
	movq	%r14, %r13
	call	_ZN11GRBQuadExprD1Ev
.L56:
	movq	%r15, %rdi
	call	_ZN10GRBLinExprD1Ev
.L57:
	movq	%r12, %rdi
	movq	%r13, %r12
	call	_ZN11GRBQuadExprD1Ev
	jmp	.L45
.L117:
	call	__cxa_get_exception_ptr@PLT
	movq	%rax, %rbx
	leaq	1136(%rsp), %rax
	movq	%rax, 1120(%rsp)
	movq	(%rbx), %r13
	movq	8(%rbx), %r12
	movq	%rax, 16(%rsp)
	movq	%r13, %rax
	addq	%r12, %rax
	je	.L66
	testq	%r13, %r13
	jne	.L66
	leaq	.LC13(%rip), %rdi
	call	_ZSt19__throw_logic_errorPKc@PLT
.L104:
	endbr64
	movq	%rax, %rbp
	call	__cxa_end_catch@PLT
	movq	%rbp, %rdi
.LEHB30:
	call	_Unwind_Resume@PLT
.LEHE30:
.L66:
	movq	%r12, 912(%rsp)
	cmpq	$15, %r12
	jbe	.L67
	leaq	1120(%rsp), %rdi
	leaq	912(%rsp), %rsi
	xorl	%edx, %edx
	call	_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERmm@PLT
	movq	%rax, 1120(%rsp)
	movq	%rax, %rdi
	movq	912(%rsp), %rax
	movq	%rax, 1136(%rsp)
.L68:
	movq	%r12, %rdx
	movq	%r13, %rsi
	call	memcpy@PLT
.L70:
	movq	912(%rsp), %rax
	movq	1120(%rsp), %rdx
	movq	%rbp, %rdi
	leaq	_ZSt4cout(%rip), %r12
	movq	%rax, 1128(%rsp)
	movb	$0, (%rdx,%rax)
	movl	32(%rbx), %eax
	movl	%eax, 1152(%rsp)
	call	__cxa_begin_catch@PLT
	movl	$13, %edx
	leaq	.LC14(%rip), %rsi
	movq	%r12, %rdi
.LEHB31:
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	leaq	1120(%rsp), %rax
	movq	%rax, %rdi
	movq	%rax, 24(%rsp)
	call	_ZNK12GRBException12getErrorCodeEv@PLT
	movl	%eax, %esi
	movq	%r12, %rdi
	call	_ZNSolsEi@PLT
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0
	movq	24(%rsp), %rsi
	leaq	1088(%rsp), %rdi
	call	_ZNK12GRBException10getMessageB5cxx11Ev@PLT
.LEHE31:
	movq	1096(%rsp), %rdx
	movq	1088(%rsp), %rsi
	movq	%r12, %rdi
.LEHB32:
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0
.LEHE32:
	movq	1088(%rsp), %rdi
	leaq	1104(%rsp), %rax
	cmpq	%rax, %rdi
	je	.L71
	movq	1104(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
.L71:
	movq	1120(%rsp), %rdi
	cmpq	16(%rsp), %rdi
	je	.L72
	movq	1136(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
.L72:
.LEHB33:
	call	__cxa_end_catch@PLT
.LEHE33:
	jmp	.L73
.L67:
	cmpq	$1, %r12
	jne	.L69
	movzbl	0(%r13), %eax
	movb	%al, 1136(%rsp)
	jmp	.L70
.L69:
	testq	%r12, %r12
	je	.L70
	movq	16(%rsp), %rdi
	jmp	.L68
.L103:
	endbr64
	movq	1088(%rsp), %rdi
	movq	%rax, %rbp
	leaq	1104(%rsp), %rax
	cmpq	%rax, %rdi
	je	.L76
	movq	1104(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
.L76:
	movq	1120(%rsp), %rdi
	cmpq	16(%rsp), %rdi
	je	.L77
	movq	1136(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
.L77:
	call	__cxa_end_catch@PLT
	movq	%rbp, %rdi
.LEHB34:
	call	_Unwind_Resume@PLT
.LEHE34:
.L102:
	endbr64
	movq	%rax, %rbp
	jmp	.L76
.L46:
	movq	%r14, %rdi
	call	_ZN11GRBQuadExprD1Ev
	movq	16(%rsp), %rax
	movq	24(%rsp), %r14
.L47:
	movq	%r13, %rdi
	movq	%rax, 16(%rsp)
	movq	%r14, %r13
	call	_ZN11GRBQuadExprD1Ev
	movq	16(%rsp), %rax
.L48:
	movq	%r15, %rdi
	movq	%rax, 16(%rsp)
	call	_ZN10GRBLinExprD1Ev
	movq	16(%rsp), %r14
.L49:
	movq	%rbx, %rdi
	movq	%r13, %rbx
	call	_ZN10GRBLinExprD1Ev
.L50:
	movq	32(%rsp), %rdi
	movq	%r14, %r13
	call	_ZN10GRBLinExprD1Ev
.L51:
	movq	48(%rsp), %rdi
	call	_ZN10GRBLinExprD1Ev
.L52:
	movq	8(%rsp), %rdi
	call	_ZN10GRBLinExprD1Ev
.L53:
	movq	%r12, %rdi
	movq	%r13, %r12
	call	_ZN11GRBQuadExprD1Ev
	jmp	.L45
.L42:
	movq	%r14, %rdi
	movq	%rbx, %r14
	movq	%r15, %rbx
	call	_ZN10GRBLinExprD1Ev
.L43:
	movq	%r13, %rdi
	call	_ZN10GRBLinExprD1Ev
.L44:
	movq	%r12, %rdi
	movq	%r14, %r12
	call	_ZN10GRBLinExprD1Ev
	jmp	.L45
.L39:
	movq	1120(%rsp), %rdi
	cmpq	16(%rsp), %rdi
	je	.L41
	movq	1136(%rsp), %rax
	leaq	1(%rax), %rsi
	call	_ZdlPvm@PLT
	jmp	.L41
	.cfi_endproc
.LFE2235:
	.section	.gcc_except_table
	.align 4
.LLSDAC2235:
	.byte	0xff
	.byte	0x9b
	.uleb128 .LLSDATTC2235-.LLSDATTDC2235
.LLSDATTDC2235:
	.byte	0x1
	.uleb128 .LLSDACSEC2235-.LLSDACSBC2235
.LLSDACSBC2235:
	.uleb128 .LEHB28-.LCOLDB16
	.uleb128 .LEHE28-.LEHB28
	.uleb128 .L104-.LCOLDB16
	.uleb128 0
	.uleb128 .LEHB29-.LCOLDB16
	.uleb128 .LEHE29-.LEHB29
	.uleb128 0
	.uleb128 0
	.uleb128 .LEHB30-.LCOLDB16
	.uleb128 .LEHE30-.LEHB30
	.uleb128 0
	.uleb128 0
	.uleb128 .LEHB31-.LCOLDB16
	.uleb128 .LEHE31-.LEHB31
	.uleb128 .L102-.LCOLDB16
	.uleb128 0
	.uleb128 .LEHB32-.LCOLDB16
	.uleb128 .LEHE32-.LEHB32
	.uleb128 .L103-.LCOLDB16
	.uleb128 0
	.uleb128 .LEHB33-.LCOLDB16
	.uleb128 .LEHE33-.LEHB33
	.uleb128 0
	.uleb128 0
	.uleb128 .LEHB34-.LCOLDB16
	.uleb128 .LEHE34-.LEHB34
	.uleb128 0
	.uleb128 0
.LLSDACSEC2235:
	.byte	0x2
	.byte	0
	.byte	0x1
	.byte	0x7d
	.byte	0
	.byte	0x7d
	.align 4
	.long	0

	.long	DW.ref._ZTI12GRBException-.
.LLSDATTC2235:
	.section	.text.unlikely
	.section	.text.startup
	.size	main, .-main
	.section	.text.unlikely
	.size	main.cold, .-main.cold
.LCOLDE16:
	.section	.text.startup
.LHOTE16:
	.p2align 4
	.type	_GLOBAL__sub_I_main, @function
_GLOBAL__sub_I_main:
.LFB2990:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	leaq	_ZStL8__ioinit(%rip), %rbp
	movq	%rbp, %rdi
	call	_ZNSt8ios_base4InitC1Ev@PLT
	movq	_ZNSt8ios_base4InitD1Ev@GOTPCREL(%rip), %rdi
	movq	%rbp, %rsi
	popq	%rbp
	.cfi_def_cfa_offset 8
	leaq	__dso_handle(%rip), %rdx
	jmp	__cxa_atexit@PLT
	.cfi_endproc
.LFE2990:
	.size	_GLOBAL__sub_I_main, .-_GLOBAL__sub_I_main
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I_main
	.weak	_ZTS12GRBException
	.section	.rodata._ZTS12GRBException,"aG",@progbits,_ZTS12GRBException,comdat
	.align 8
	.type	_ZTS12GRBException, @object
	.size	_ZTS12GRBException, 15
_ZTS12GRBException:
	.string	"12GRBException"
	.weak	_ZTI12GRBException
	.section	.data.rel.ro._ZTI12GRBException,"awG",@progbits,_ZTI12GRBException,comdat
	.align 8
	.type	_ZTI12GRBException, @object
	.size	_ZTI12GRBException, 16
_ZTI12GRBException:
	.quad	_ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	_ZTS12GRBException
	.local	_ZStL8__ioinit
	.comm	_ZStL8__ioinit,1,1
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC2:
	.long	0
	.long	1072693248
	.align 8
.LC6:
	.long	0
	.long	1073741824
	.align 8
.LC7:
	.long	0
	.long	1074790400
	.align 8
.LC8:
	.long	0
	.long	1074266112
	.hidden	DW.ref._ZTI12GRBException
	.weak	DW.ref._ZTI12GRBException
	.section	.data.rel.local.DW.ref._ZTI12GRBException,"awG",@progbits,DW.ref._ZTI12GRBException,comdat
	.align 8
	.type	DW.ref._ZTI12GRBException, @object
	.size	DW.ref._ZTI12GRBException, 8
DW.ref._ZTI12GRBException:
	.quad	_ZTI12GRBException
	.hidden	DW.ref.__gxx_personality_v0
	.weak	DW.ref.__gxx_personality_v0
	.section	.data.rel.local.DW.ref.__gxx_personality_v0,"awG",@progbits,DW.ref.__gxx_personality_v0,comdat
	.align 8
	.type	DW.ref.__gxx_personality_v0, @object
	.size	DW.ref.__gxx_personality_v0, 8
DW.ref.__gxx_personality_v0:
	.quad	__gxx_personality_v0
	.hidden	__dso_handle
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
