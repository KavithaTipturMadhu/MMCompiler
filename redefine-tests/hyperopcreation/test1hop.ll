; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests//hyperopcreation/test1.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define void @entry(i32 inreg, i32 inreg, i32 inreg, i32 inreg) {
entry:
  %4 = alloca i32, align 4, !ConsumedBy !5
  %5 = alloca i32, align 4, !ConsumedBy !8
  %6 = alloca i32, align 4, !ConsumedBy !10
  %7 = alloca i32, align 4, !ConsumedBy !12
  %8 = alloca i32, align 4, !ConsumedBy !14
  %9 = alloca i32, align 4, !ConsumedBy !17
  %10 = alloca i32, align 4, !ConsumedBy !20
  store i32 %0, i32* %4, align 4
  store i32 %1, i32* %5, align 4
  store i32 %2, i32* %6, align 4
  store i32 %3, i32* %7, align 4
  br label %entryreturn

entryreturn:                                      ; preds = %entry
  ret void, !ControlledBy !23
}

define void @entry1(i32 inreg, i32*, i32*, i32*) {
entry0:
  store i32 %0, i32* %1, align 4
  %4 = load i32* %2, align 4
  %5 = load i32* %3, align 4
  %6 = add nsw i32 %4, %5
  br label %entry1return

entry1return:                                     ; preds = %entry0
  ret void, !ControlledBy !25
}

define void @entry2(i32 inreg, i32*, i32*, i32*) {
entry01:
  store i32 %0, i32* %1, align 4
  %4 = load i32* %2, align 4
  %5 = load i32* %3, align 4
  %6 = add nsw i32 %4, %5
  br label %entry2return

entry2return:                                     ; preds = %entry01
  ret void, !ControlledBy !27
}

define void @entry3(i32*, i32 inreg, i32*, i32*) {
entry012:
  %4 = load i32* %0, align 4
  %5 = add nsw i32 %1, %4
  store i32 %5, i32* %2, align 4
  %6 = load i32* %3, align 4
  br label %entry3return

entry3return:                                     ; preds = %entry012
  ret void, !ControlledBy !29
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

entry4return:                                     ; No predecessors!
  ret void
}

!redefine.annotations = !{!0, !1, !2, !3, !4}

!0 = metadata !{metadata !"HyperOp", void (i32, i32, i32, i32)* @entry}
!1 = metadata !{metadata !"HyperOp", void (i32, i32*, i32*, i32*)* @entry1}
!2 = metadata !{metadata !"HyperOp", void (i32, i32*, i32*, i32*)* @entry2}
!3 = metadata !{metadata !"HyperOp", void (i32*, i32, i32*, i32*)* @entry3}
!4 = metadata !{metadata !"HyperOp", void (i32*, i32, i32*, i32*)* @entry4}
!5 = metadata !{metadata !6, metadata !7}
!6 = metadata !{metadata !1, metadata !"Reference", i32 2}
!7 = metadata !{metadata !2, metadata !"Reference", i32 2}
!8 = metadata !{metadata !9}
!9 = metadata !{metadata !1, metadata !"Reference", i32 3}
!10 = metadata !{metadata !11}
!11 = metadata !{metadata !2, metadata !"Reference", i32 3}
!12 = metadata !{metadata !13}
!13 = metadata !{metadata !3, metadata !"Reference", i32 0}
!14 = metadata !{metadata !15, metadata !16}
!15 = metadata !{metadata !1, metadata !"Reference", i32 1}
!16 = metadata !{metadata !3, metadata !"Reference", i32 3}
!17 = metadata !{metadata !18, metadata !19}
!18 = metadata !{metadata !2, metadata !"Reference", i32 1}
!19 = metadata !{metadata !4, metadata !"Reference", i32 0}
!20 = metadata !{metadata !21, metadata !22, metadata !22}
!21 = metadata !{metadata !3, metadata !"Reference", i32 2}
!22 = metadata !{metadata !4, metadata !"Reference", i32 2}
!23 = metadata !{metadata !24}
!24 = metadata !{metadata !1}
!25 = metadata !{metadata !26}
!26 = metadata !{metadata !2}
!27 = metadata !{metadata !28}
!28 = metadata !{metadata !3}
!29 = metadata !{metadata !30}
!30 = metadata !{metadata !4}
