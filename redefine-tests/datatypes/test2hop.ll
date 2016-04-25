; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/test1/test1.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define void @main1(i32 inreg, i32 inreg, i32 inreg, i32 inreg, i32 inreg) {
entry:
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  store i32 %0, i32* %5, align 4
  store i32 %1, i32* %6, align 4
  store i32 %2, i32* %7, align 4
  store i32 %3, i32* %8, align 4
  store i32 %4, i32* %9, align 4
  %13 = load i32* %5, align 4
  %14 = load i32* %6, align 4
  %15 = add nsw i32 %13, %14
  store i32 %15, i32* %10, align 4
  %16 = load i32* %5, align 4
  %17 = load i32* %7, align 4
  %18 = add nsw i32 %16, %17
  %19 = load i32* %8, align 4
  %20 = add nsw i32 %18, %19
  store i32 %20, i32* %11, align 4
  %21 = load i32* %9, align 4
  %22 = load i32* %10, align 4
  %23 = mul nsw i32 %21, %22
  %24 = load i32* %11, align 4
  %25 = add nsw i32 %23, %24
  store i32 %25, i32* %12, align 4
  ret void

main1return:                                      ; No predecessors!
  ret void
}

!redefine.annotations = !{!0, !1, !2}

!0 = metadata !{metadata !"HyperOp", void (i32, i32, i32, i32, i32)* @main1}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"Exit", metadata !0}
