; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/hyperopcreation/test2-control/test2.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "redefine"

define void @redefine_start1(i32 inreg, i32 inreg, i32 inreg, i32 inreg) {
redefine_start1.entry:
  %4 = alloca i32, align 4, !ConsumedBy !10
  %5 = alloca i32, align 4, !ConsumedBy !12
  %6 = alloca i32, align 4, !ConsumedBy !14
  %7 = alloca i32, align 4, !ConsumedBy !17
  %8 = alloca i32, align 4, !ConsumedBy !20
  store i32 %0, i32* %4, align 4
  store i32 %1, i32* %5, align 4
  store i32 %2, i32* %6, align 4
  store i32 %3, i32* %7, align 4
  br label %redefine_start1.return

redefine_start1.return:                           ; preds = %redefine_start1.entry
  ret void
}

define void @redefine_start2(i32 inreg, i32* inreg, i32* inreg, i32* inreg) {
redefine_start2.entry0:
  %4 = alloca i1, align 4, !ControlledBy !24
  store i32 %0, i32* %1, align 4
  %5 = load i32* %2, align 4
  %6 = load i32* %3, align 4
  %7 = icmp sgt i32 %5, %6
  store i1 %7, i1* %4, align 4
  br label %redefine_start2.return

redefine_start2.return:                           ; preds = %redefine_start2.entry0
  ret void
}

define void @redefine_start3(i32* inreg, i32* inreg, i32* inreg) {
redefine_start3.if.then:
  %3 = alloca i1, align 4, !ControlledBy !27
  %4 = load i32* %0, align 4
  %5 = load i32* %1, align 4
  %6 = add nsw i32 %4, %5
  store i32 %6, i32* %2, align 4
  br label %redefine_start3.return

redefine_start3.return:                           ; preds = %redefine_start3.if.then
  store i1 true, i1* %3, align 4
  ret void
}

define void @redefine_start4(i32* inreg, i32* inreg, i32* inreg) {
redefine_start4.if.else:
  %3 = alloca i1, align 4, !ControlledBy !27
  %4 = load i32* %0, align 4
  %5 = load i32* %1, align 4
  %6 = sub nsw i32 %4, %5
  store i32 %6, i32* %2, align 4
  br label %redefine_start4.return

redefine_start4.return:                           ; preds = %redefine_start4.if.else
  store i1 true, i1* %3, align 4
  ret void
}

define void @redefine_start5() {
redefine_start5.if.end:
  ret void
}

!redefine.annotations = !{!0, !1, !2, !3, !4, !5, !6, !7, !8, !9}

!0 = metadata !{metadata !"HyperOp", void (i32, i32, i32, i32)* @redefine_start1}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void (i32, i32*, i32*, i32*)* @redefine_start2}
!3 = metadata !{metadata !"Intermediate", metadata !2}
!4 = metadata !{metadata !"HyperOp", void (i32*, i32*, i32*)* @redefine_start3}
!5 = metadata !{metadata !"Intermediate", metadata !4}
!6 = metadata !{metadata !"HyperOp", void (i32*, i32*, i32*)* @redefine_start4}
!7 = metadata !{metadata !"Intermediate", metadata !6}
!8 = metadata !{metadata !"HyperOp", void ()* @redefine_start5}
!9 = metadata !{metadata !"Exit", metadata !8}
!10 = metadata !{metadata !11}
!11 = metadata !{metadata !2, metadata !"Scalar", i32 2}
!12 = metadata !{metadata !13}
!13 = metadata !{metadata !2, metadata !"Scalar", i32 3}
!14 = metadata !{metadata !15, metadata !16}
!15 = metadata !{metadata !4, metadata !"Scalar", i32 2}
!16 = metadata !{metadata !6, metadata !"Scalar", i32 2}
!17 = metadata !{metadata !18, metadata !19}
!18 = metadata !{metadata !4, metadata !"Scalar", i32 0}
!19 = metadata !{metadata !6, metadata !"Scalar", i32 0}
!20 = metadata !{metadata !21, metadata !22, metadata !23}
!21 = metadata !{metadata !2, metadata !"Scalar", i32 1}
!22 = metadata !{metadata !4, metadata !"Scalar", i32 1}
!23 = metadata !{metadata !6, metadata !"Scalar", i32 1}
!24 = metadata !{metadata !25, metadata !26}
!25 = metadata !{metadata !4}
!26 = metadata !{metadata !6}
!27 = metadata !{metadata !28}
!28 = metadata !{metadata !8}
