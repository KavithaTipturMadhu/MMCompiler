; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/hyperopcreation/test1.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define void @redefine_start1(i32 inreg, i32 inreg, i32 inreg, i32 inreg) {
entry:
  %4 = alloca i32, align 4, !ConsumedBy !10
  %5 = alloca i32, align 4, !ConsumedBy !13
  %6 = alloca i32, align 4, !ConsumedBy !15
  %7 = alloca i32, align 4, !ConsumedBy !17
  %8 = alloca i32, align 4, !ConsumedBy !19
  %9 = alloca i32, align 4, !ConsumedBy !22
  %10 = alloca i32, align 4, !ConsumedBy !25
  store i32 %0, i32* %4, align 4
  store i32 %1, i32* %5, align 4
  store i32 %2, i32* %6, align 4
  store i32 %3, i32* %7, align 4
  br label %redefine_start1return

redefine_start1return:                            ; preds = %entry
  ret void
}

define void @redefine_start2(i32 inreg, i32*, i32*, i32*) {
entry0:
  store i32 %0, i32* %1, align 4
  %4 = load i32* %2, align 4
  %5 = load i32* %3, align 4
  %6 = add nsw i32 %4, %5, !ConsumedBy !28
  %7 = alloca i32, !ConsumedBy !30
  store i32 %6, i32* %7
  br label %redefine_start2return

redefine_start2return:                            ; preds = %entry0
  ret void
}

define void @redefine_start3(i32 inreg, i32*, i32*, i32*) {
entry01:
  store i32 %0, i32* %1, align 4
  %4 = load i32* %2, align 4
  %5 = load i32* %3, align 4
  %6 = add nsw i32 %4, %5, !ConsumedBy !31
  %7 = alloca i32, !ConsumedBy !33
  store i32 %6, i32* %7
  br label %redefine_start3return

redefine_start3return:                            ; preds = %entry01
  ret void
}

define void @redefine_start4(i32*, i32 inreg, i32*, i32*) {
entry012:
  %4 = load i32* %0, align 4
  %5 = add nsw i32 %1, %4
  store i32 %5, i32* %2, align 4
  %6 = load i32* %3, align 4, !ConsumedBy !34
  %7 = alloca i32, !ConsumedBy !36
  store i32 %6, i32* %7
  br label %redefine_start4return

redefine_start4return:                            ; preds = %entry012
  ret void
}

define void @redefine_start5(i32*, i32 inreg, i32*, i32*) {
entry0123:
  %4 = alloca i32, align 4, !ConsumedBy !37
  %5 = load i32* %0, align 4
  %6 = mul nsw i32 %1, %5
  %7 = load i32* %2, align 4
  %8 = add nsw i32 %6, %7
  store i32 %8, i32* %4, align 4
  ret void

redefine_start5return:                            ; No predecessors!
  ret void
}

!redefine.annotations = !{!0, !1, !2, !3, !4, !5, !6, !7, !8, !9}

!0 = metadata !{metadata !"HyperOp", void (i32, i32, i32, i32)* @redefine_start1}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void (i32, i32*, i32*, i32*)* @redefine_start2}
!3 = metadata !{metadata !"Intermediate", metadata !2}
!4 = metadata !{metadata !"HyperOp", void (i32, i32*, i32*, i32*)* @redefine_start3}
!5 = metadata !{metadata !"Intermediate", metadata !4}
!6 = metadata !{metadata !"HyperOp", void (i32*, i32, i32*, i32*)* @redefine_start4}
!7 = metadata !{metadata !"Intermediate", metadata !6}
!8 = metadata !{metadata !"HyperOp", void (i32*, i32, i32*, i32*)* @redefine_start5}
!9 = metadata !{metadata !"Exit", metadata !8}
!10 = metadata !{metadata !11, metadata !11, metadata !12, metadata !12, metadata !12}
!11 = metadata !{metadata !2, metadata !"LocalReference", i32 2}
!12 = metadata !{metadata !4, metadata !"LocalReference", i32 2}
!13 = metadata !{metadata !14, metadata !14}
!14 = metadata !{metadata !2, metadata !"LocalReference", i32 3}
!15 = metadata !{metadata !16, metadata !16, metadata !16}
!16 = metadata !{metadata !4, metadata !"LocalReference", i32 3}
!17 = metadata !{metadata !18, metadata !18, metadata !18, metadata !18}
!18 = metadata !{metadata !6, metadata !"LocalReference", i32 0}
!19 = metadata !{metadata !20, metadata !20, metadata !21, metadata !21, metadata !21, metadata !21}
!20 = metadata !{metadata !2, metadata !"LocalReference", i32 1}
!21 = metadata !{metadata !6, metadata !"LocalReference", i32 3}
!22 = metadata !{metadata !23, metadata !23, metadata !23, metadata !24, metadata !24, metadata !24, metadata !24, metadata !24}
!23 = metadata !{metadata !4, metadata !"LocalReference", i32 1}
!24 = metadata !{metadata !8, metadata !"LocalReference", i32 0}
!25 = metadata !{metadata !26, metadata !26, metadata !26, metadata !26, metadata !27, metadata !27, metadata !27, metadata !27, metadata !27, metadata !27, metadata !27, metadata !27, metadata !27, metadata !27}
!26 = metadata !{metadata !6, metadata !"LocalReference", i32 2}
!27 = metadata !{metadata !8, metadata !"LocalReference", i32 2}
!28 = metadata !{metadata !29}
!29 = metadata !{metadata !4, metadata !"Scalar", i32 0}
!30 = metadata !{metadata !29, metadata !29}
!31 = metadata !{metadata !32}
!32 = metadata !{metadata !6, metadata !"Scalar", i32 1}
!33 = metadata !{metadata !32, metadata !32, metadata !32}
!34 = metadata !{metadata !35}
!35 = metadata !{metadata !8, metadata !"Scalar", i32 1}
!36 = metadata !{metadata !35, metadata !35, metadata !35, metadata !35}
!37 = metadata !{metadata !38, metadata !38, metadata !38, metadata !38}
!38 = metadata !{metadata !8, metadata !"LocalReference", i32 3}
