; ModuleID = '/home/kavitha/Desktop/REDEFINE_LLVM/MM_Compiler/redefine-tests/hyperopcreation/test2-control/test2.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "redefine"

define void @redefine_start1(i32 inreg, i32 inreg, i32 inreg, i32 inreg) {
redefine_start1.entry:
  %4 = alloca i32, align 4, !ConsumedBy !6
  %5 = alloca i32, align 4, !ConsumedBy !8
  %6 = alloca i32, align 4, !ConsumedBy !10
  %7 = alloca i32, align 4, !ConsumedBy !12
  %8 = alloca i32, align 4, !ConsumedBy !14
  store i32 %0, i32* %4, align 4
  store i32 %1, i32* %5, align 4
  store i32 %2, i32* %6, align 4
  store i32 %3, i32* %7, align 4
  br label %redefine_start1.return

redefine_start1.return:                           ; preds = %redefine_start1.entry
  ret void
}

define void @redefine_start2(i32 inreg, i32*, i32*, i32*) {
redefine_start2.entry0:
  store i32 %0, i32* %1, align 4
  %4 = load i32* %2, align 4
  %5 = load i32* %3, align 4
  %6 = icmp sgt i32 %4, %5
  %7 = alloca i1, align 4, !ControlledBy !17
  store i1 %6, i1* %7, align 4
  br i1 %6, label %redefine_start2.if.then, label %redefine_start2.return

redefine_start2.if.then:                          ; preds = %redefine_start2.entry0
  br label %redefine_start2.return

redefine_start2.return:                           ; preds = %redefine_start2.if.then, %redefine_start2.entry0
  %8 = alloca i32, !ControlledBy !17
  store i32 1, i32* %8
  ret void
}

define void @redefine_start3(i32*, i32*, i32*) {
redefine_start3.if.else:
  %3 = load i32* %0, align 4
  %4 = load i32* %1, align 4
  %5 = sub nsw i32 %3, %4
  store i32 %5, i32* %2, align 4
  br label %redefine_start3.if.end

redefine_start3.if.then1:                         ; No predecessors!
  %6 = load i32* %0, align 4
  %7 = load i32* %1, align 4
  %8 = add nsw i32 %6, %7
  store i32 %8, i32* %2, align 4
  br label %redefine_start3.if.end

redefine_start3.if.end:                           ; preds = %redefine_start3.if.then1, %redefine_start3.if.else
  ret void

redefine_start3.return:                           ; No predecessors!
  ret void
}

!redefine.annotations = !{!0, !1, !2, !3, !4, !5}

!0 = metadata !{metadata !"HyperOp", void (i32, i32, i32, i32)* @redefine_start1}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void (i32, i32*, i32*, i32*)* @redefine_start2}
!3 = metadata !{metadata !"Intermediate", metadata !2}
!4 = metadata !{metadata !"HyperOp", void (i32*, i32*, i32*)* @redefine_start3}
!5 = metadata !{metadata !"Exit", metadata !4}
!6 = metadata !{metadata !7}
!7 = metadata !{metadata !2, metadata !"LocalReference", i32 2}
!8 = metadata !{metadata !9}
!9 = metadata !{metadata !2, metadata !"LocalReference", i32 3}
!10 = metadata !{metadata !11}
!11 = metadata !{metadata !4, metadata !"LocalReference", i32 2}
!12 = metadata !{metadata !13}
!13 = metadata !{metadata !4, metadata !"LocalReference", i32 0}
!14 = metadata !{metadata !15, metadata !16}
!15 = metadata !{metadata !2, metadata !"LocalReference", i32 1}
!16 = metadata !{metadata !4, metadata !"LocalReference", i32 1}
!17 = metadata !{metadata !18}
!18 = metadata !{metadata !4}
