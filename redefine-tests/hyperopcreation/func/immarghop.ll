; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/hyperopcreation/func/bkp/immarg.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define void @redefine_start1() {
redefine_start1.entry:
  %0 = alloca i32, align 4, !ConsumedBy !4
  store i32 30, i32* %0
  %1 = load i32* %0
  br label %redefine_start1.return

redefine_start1.return:                           ; preds = %redefine_start1.entry
  ret void
}

define void @sum2(i32 inreg) {
sum2.entry:
  %1 = alloca i32, align 4
  store i32 %0, i32* %1, align 4
  %2 = load i32* %1, align 4
  %3 = mul nsw i32 %2, 2
  br label %sum2.return

sum2.return:                                      ; preds = %sum2.entry
  ret void
}

!redefine.annotations = !{!0, !1, !2, !3}

!0 = metadata !{metadata !"HyperOp", void ()* @redefine_start1}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void (i32)* @sum2}
!3 = metadata !{metadata !"Exit", metadata !2}
!4 = metadata !{metadata !5}
!5 = metadata !{metadata !2, metadata !"Scalar", i32 0}
