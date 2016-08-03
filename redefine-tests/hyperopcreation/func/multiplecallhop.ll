; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/hyperopcreation/func/multiplecall.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define void @redefine_start1() {
redefine_start1.entry:
  %0 = alloca i32, align 4, !ConsumedBy !10
  %1 = alloca i32, align 4, !ConsumedBy !13
  %2 = alloca i32, align 4, !ConsumedBy !15
  store i32 10, i32* %0, align 4
  store i32 20, i32* %1, align 4
  %3 = load i32* %0, align 4
  %4 = load i32* %1, align 4
  br label %redefine_start1.return

redefine_start1.return:                           ; preds = %redefine_start1.entry
  ret void
}

define void @sum2(i32 inreg, i32 inreg) {
sum2.entry:
  %2 = alloca i32, align 4, !ConsumedBy !18
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  %5 = load i32* %3, align 4
  %6 = load i32* %4, align 4
  %7 = add nsw i32 %5, %6
  ret void
}

define void @redefine_start3(i32 inreg, i32* inreg, i32* inreg) {
redefine_start3.newName1:
  store i32 %0, i32* %1, align 4
  %3 = load i32* %1, align 4, !ConsumedBy !20
  %4 = load i32* %2, align 4, !ConsumedBy !22
  br label %redefine_start3.return

redefine_start3.return:                           ; preds = %redefine_start3.newName1
  ret void
}

define void @diff4(i32 inreg, i32 inreg) {
diff4.entry:
  %2 = alloca i32, align 4, !ConsumedBy !24
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  %5 = load i32* %3, align 4
  %6 = load i32* %4, align 4
  %7 = sub nsw i32 %5, %6
  ret void
}

define void @redefine_start5(i32 inreg, i32* inreg) {
redefine_start5.newName12:
  store i32 %0, i32* %1, align 4
  ret void
}

!redefine.annotations = !{!0, !1, !2, !3, !4, !5, !6, !7, !8, !9}

!0 = metadata !{metadata !"HyperOp", void ()* @redefine_start1}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void (i32, i32)* @sum2}
!3 = metadata !{metadata !"Intermediate", metadata !2}
!4 = metadata !{metadata !"HyperOp", void (i32, i32*, i32*)* @redefine_start3}
!5 = metadata !{metadata !"Intermediate", metadata !4}
!6 = metadata !{metadata !"HyperOp", void (i32, i32)* @diff4}
!7 = metadata !{metadata !"Intermediate", metadata !6}
!8 = metadata !{metadata !"HyperOp", void (i32, i32*)* @redefine_start5}
!9 = metadata !{metadata !"Exit", metadata !8}
!10 = metadata !{metadata !11, metadata !12}
!11 = metadata !{metadata !2, metadata !"Scalar", i32 0}
!12 = metadata !{metadata !4, metadata !"Scalar", i32 2}
!13 = metadata !{metadata !14}
!14 = metadata !{metadata !2, metadata !"Scalar", i32 1}
!15 = metadata !{metadata !16, metadata !17}
!16 = metadata !{metadata !4, metadata !"Scalar", i32 1}
!17 = metadata !{metadata !8, metadata !"Scalar", i32 1}
!18 = metadata !{metadata !19}
!19 = metadata !{metadata !4, metadata !"Scalar", i32 0}
!20 = metadata !{metadata !21}
!21 = metadata !{metadata !6, metadata !"Scalar", i32 0}
!22 = metadata !{metadata !23}
!23 = metadata !{metadata !6, metadata !"Scalar", i32 1}
!24 = metadata !{metadata !25}
!25 = metadata !{metadata !8, metadata !"Scalar", i32 0}
