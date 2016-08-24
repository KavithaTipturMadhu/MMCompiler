; ModuleID = 'arka-test/Exp0.ll'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

define void @redefine_start1() {
redefine_start1.entry:
  %0 = alloca i32, align 4, !ConsumedBy !12
  %1 = alloca i32, align 4, !ConsumedBy !15
  %2 = alloca i32, align 4, !ConsumedBy !18
  %3 = alloca i32, align 4, !ConsumedBy !20
  store i32 10, i32* %0, align 4
  store i32 11, i32* %1, align 4
  store i32 10, i32* %2, align 4
  store i32 20, i32* %3, align 4
  %4 = load i32* %0, align 4
  %5 = load i32* %1, align 4
  br label %redefine_start1.return

redefine_start1.return:                           ; preds = %redefine_start1.entry
  ret void
}

define void @mul2(i32 inreg, i32 inreg) {
mul2.entry:
  %2 = alloca i32, align 4, !ConsumedBy !22
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  %5 = load i32* %3, align 4
  %6 = load i32* %4, align 4
  %7 = mul nsw i32 %5, %6
  store i32 %7, i32* %2, align 4
  br label %mul2.return

mul2.return:                                      ; preds = %mul2.entry
  ret void
}

define void @redefine_start3(i32 inreg, i32* inreg, i32* inreg, i32* inreg) {
redefine_start3.newName01:
  %4 = alloca i32, align 4, !ConsumedBy !24
  %5 = alloca i32, align 4, !ConsumedBy !26
  %6 = alloca i32, align 4, !ConsumedBy !28
  store i32 %0, i32* %1, align 4
  %7 = load i32* %1, align 4
  %8 = add nsw i32 %7, 1
  store i32 %8, i32* %1, align 4
  store i32 %7, i32* %2, align 4
  %9 = load i32* %3, align 4
  store i32 %9, i32* %6, align 4
  %10 = load i32* %2, align 4
  store i32 %10, i32* %5, align 4
  %11 = load i32* %1, align 4
  store i32 %11, i32* %4, align 4
  br label %redefine_start3.return

redefine_start3.return:                           ; preds = %redefine_start3.newName01
  ret void
}

define void @fmaX4(i32 inreg, i32 inreg, i32 inreg) {
fmaX4.entry:
  %3 = alloca i32, align 4, !ConsumedBy !30
  %4 = alloca i32, align 4, !ConsumedBy !32
  %5 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  store i32 %2, i32* %5, align 4
  %6 = load i32* %3, align 4
  %7 = load i32* %4, align 4
  br label %fmaX4.return

fmaX4.return:                                     ; preds = %fmaX4.entry
  ret void
}

define void @mul5(i32 inreg, i32 inreg) {
mul5.entry:
  %2 = alloca i32, align 4, !ConsumedBy !34
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  %5 = load i32* %3, align 4
  %6 = load i32* %4, align 4
  %7 = mul nsw i32 %5, %6
  store i32 %7, i32* %2, align 4
  br label %mul5.return

mul5.return:                                      ; preds = %mul5.entry
  ret void
}

define void @redefine_start6(i32 inreg, i32* inreg) {
redefine_start6.newName03:
  store i32 %0, i32* %1, align 4
  br label %redefine_start6.return

redefine_start6.return:                           ; preds = %redefine_start6.newName03
  ret void
}

!redefine.annotations = !{!0, !1, !2, !3, !4, !5, !6, !7, !8, !9, !10, !11}

!0 = metadata !{metadata !"HyperOp", void ()* @redefine_start1}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void (i32, i32)* @mul2}
!3 = metadata !{metadata !"Intermediate", metadata !2}
!4 = metadata !{metadata !"HyperOp", void (i32, i32*, i32*, i32*)* @redefine_start3}
!5 = metadata !{metadata !"Intermediate", metadata !4}
!6 = metadata !{metadata !"HyperOp", void (i32, i32, i32)* @fmaX4}
!7 = metadata !{metadata !"Intermediate", metadata !6}
!8 = metadata !{metadata !"HyperOp", void (i32, i32)* @mul5}
!9 = metadata !{metadata !"Intermediate", metadata !8}
!10 = metadata !{metadata !"HyperOp", void (i32, i32*)* @redefine_start6}
!11 = metadata !{metadata !"Exit", metadata !10}
!12 = metadata !{metadata !13, metadata !14}
!13 = metadata !{metadata !2, metadata !"Scalar", i32 0}
!14 = metadata !{metadata !10, metadata !"Scalar", i32 1}
!15 = metadata !{metadata !16, metadata !17}
!16 = metadata !{metadata !2, metadata !"Scalar", i32 1}
!17 = metadata !{metadata !4, metadata !"Scalar", i32 3}
!18 = metadata !{metadata !19}
!19 = metadata !{metadata !4, metadata !"Scalar", i32 2}
!20 = metadata !{metadata !21}
!21 = metadata !{metadata !4, metadata !"Scalar", i32 1}
!22 = metadata !{metadata !23}
!23 = metadata !{metadata !4, metadata !"Scalar", i32 0}
!24 = metadata !{metadata !25}
!25 = metadata !{metadata !6, metadata !"Scalar", i32 2}
!26 = metadata !{metadata !27}
!27 = metadata !{metadata !6, metadata !"Scalar", i32 1}
!28 = metadata !{metadata !29}
!29 = metadata !{metadata !6, metadata !"Scalar", i32 0}
!30 = metadata !{metadata !31}
!31 = metadata !{metadata !8, metadata !"Scalar", i32 0}
!32 = metadata !{metadata !33}
!33 = metadata !{metadata !8, metadata !"Scalar", i32 1}
!34 = metadata !{metadata !35}
!35 = metadata !{metadata !10, metadata !"Scalar", i32 0}
