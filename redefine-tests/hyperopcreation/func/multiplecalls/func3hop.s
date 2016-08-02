; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/hyperopcreation/func/multiplecalls/func3.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define void @sum1() {
sum1.entry:
  %0 = alloca i32, align 4, !Controls !8
  br label %sum1.return

sum1.return:                                      ; preds = %sum1.entry
  store i32 200, i32* %0, align 4
  ret void
}

define void @sum2() {
sum2.entry:
  %0 = alloca i32, align 4, !Sync !10
  br label %sum2.return

sum2.return:                                      ; preds = %sum2.entry
  store i32 200, i32* %0, align 4
  ret void
}

define void @sum3() {
sum3.entry:
  %0 = alloca i32, align 4, !Controls !10
  br label %sum3.return

sum3.return:                                      ; preds = %sum3.entry
  store i32 200, i32* %0, align 4
  ret void
}

define void @sum4() {
sum4.entry:
  br label %sum4.return

sum4.return:                                      ; preds = %sum4.entry
  ret void
}

!redefine.annotations = !{!0, !1, !2, !3, !4, !5, !6, !7}

!0 = metadata !{metadata !"HyperOp", void ()* @sum1}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void ()* @sum2}
!3 = metadata !{metadata !"Intermediate", metadata !2}
!4 = metadata !{metadata !"HyperOp", void ()* @sum3}
!5 = metadata !{metadata !"Intermediate", metadata !4}
!6 = metadata !{metadata !"HyperOp", void ()* @sum4}
!7 = metadata !{metadata !"Exit", metadata !6}
!8 = metadata !{metadata !9}
!9 = metadata !{metadata !4}
!10 = metadata !{metadata !11}
!11 = metadata !{metadata !6}
