; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/hyperopcreation/test1.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define void @entry(i32 inreg, i32 inreg, i32 inreg, i32 inreg) {
entry:
  %4 = alloca i32, align 4, !ConsumedBy !9
  %5 = alloca i32, align 4, !ConsumedBy !12
  %6 = alloca i32, align 4, !ConsumedBy !14
  %7 = alloca i32, align 4, !ConsumedBy !16
  %8 = alloca i32, align 4, !ConsumedBy !18
  %9 = alloca i32, align 4, !ConsumedBy !21
  %10 = alloca i32, align 4, !ConsumedBy !24
  store i32 %0, i32* %4, align 4
  store i32 %1, i32* %5, align 4
  store i32 %2, i32* %6, align 4
  store i32 %3, i32* %7, align 4
  ret void
}

define void @entry1(i32 inreg, i32*, i32*, i32*) {
entry0:
  store i32 %0, i32* %1, align 4
  %4 = load i32* %2, align 4
  %5 = load i32* %3, align 4
  %6 = add nsw i32 %4, %5
  ret void
}

define void @entry2(i32 inreg, i32*, i32*, i32*) {
entry01:
  store i32 %0, i32* %1, align 4
  %4 = load i32* %2, align 4
  %5 = load i32* %3, align 4
  %6 = add nsw i32 %4, %5
  ret void
}

define void @entry3(i32*, i32 inreg, i32*, i32*) {
entry012:
  %4 = load i32* %0, align 4
  %5 = add nsw i32 %1, %4
  store i32 %5, i32* %2, align 4
  %6 = load i32* %3, align 4
  ret void
}

define void @entry4(i32*, i32 inreg, i32*, i32*) {
entry0123:
  %4 = alloca i32, align 4
  %5 = load i32* %0, align 4
  %6 = mul nsw i32 %1, %5
  %7 = load i32* %2, align 4
  %8 = add nsw i32 %6, %7
  store i32 %8, i32* %4, align 4
  ret void
}

!redefine.annotations = !{!0, !1, !2, !3, !4, !5, !6, !7, !8}

!0 = metadata !{metadata !"HyperOp", void (i32, i32, i32, i32)* @entry}
!1 = metadata !{metadata !"HyperOp", void (i32, i32*, i32*, i32*)* @entry1}
!3 = metadata !{metadata !"HyperOp", void (i32, i32*, i32*, i32*)* @entry2}
!5 = metadata !{metadata !"HyperOp", void (i32*, i32, i32*, i32*)* @entry3}
!7 = metadata !{metadata !"HyperOp", void (i32*, i32, i32*, i32*)* @entry4}

!2 = metadata !{metadata !"Predicates", metadata !0, metadata !1, i32 0}
!4 = metadata !{metadata !"Predicates", metadata !1, metadata !3, i32 0}
!6 = metadata !{metadata !"Predicates", metadata !3, metadata !5, i32 0}
!8 = metadata !{metadata !"Predicates", metadata !5, metadata !7, i32 0}
!9 = metadata !{metadata !10, metadata !11}
!10 = metadata !{metadata !1, metadata !"Reference", i32 2}
!11 = metadata !{metadata !3, metadata !"Reference", i32 2}
!12 = metadata !{metadata !13}
!13 = metadata !{metadata !1, metadata !"Reference", i32 3}
!14 = metadata !{metadata !15}
!15 = metadata !{metadata !3, metadata !"Reference", i32 3}
!16 = metadata !{metadata !17}
!17 = metadata !{metadata !5, metadata !"Reference", i32 0}
!18 = metadata !{metadata !19, metadata !20}
!19 = metadata !{metadata !1, metadata !"Reference", i32 1}
!20 = metadata !{metadata !5, metadata !"Reference", i32 3}
!21 = metadata !{metadata !22, metadata !23}
!22 = metadata !{metadata !3, metadata !"Reference", i32 1}
!23 = metadata !{metadata !7, metadata !"Reference", i32 0}
!24 = metadata !{metadata !25, metadata !26, metadata !26}
!25 = metadata !{metadata !5, metadata !"Reference", i32 2}
!26 = metadata !{metadata !7, metadata !"Reference", i32 2}
