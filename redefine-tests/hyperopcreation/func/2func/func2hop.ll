; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/hyperopcreation/func/func2.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define void @redefine_start1() {
redefine_start1.entry:
  %0 = alloca i1, align 4, !ControlledBy !6
  br label %redefine_start1.return

redefine_start1.return:                           ; preds = %redefine_start1.entry
  store i1 true, i1* %0, align 4
  ret void
}

define void @sum2() {
sum2.entry:
  %0 = alloca i32, align 4, !ConsumedBy !8
  br label %sum2.return

sum2.return:                                      ; preds = %sum2.entry
  store i32 200, i32* %0, align 4
  ret void
}

define void @redefine_start3(i32 inreg) {
redefine_start3.newName1:
  %1 = alloca i32, align 4
  store i32 %0, i32* %1, align 4
  ret void
}

!redefine.annotations = !{!0, !1, !2, !3, !4, !5}

!0 = metadata !{metadata !"HyperOp", void ()* @redefine_start1}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void ()* @sum2}
!3 = metadata !{metadata !"Intermediate", metadata !2}
!4 = metadata !{metadata !"HyperOp", void (i32)* @redefine_start3}
!5 = metadata !{metadata !"Exit", metadata !4}
!6 = metadata !{metadata !7}
!7 = metadata !{metadata !2}
!8 = metadata !{metadata !9}
!9 = metadata !{metadata !4, metadata !"Scalar", i32 0}
