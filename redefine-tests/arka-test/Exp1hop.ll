; ModuleID = 'arka-test/Exp1.ll'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

define void @redefine_start1() {
redefine_start1.entry:
  %0 = alloca i1, align 4, !Controls !10
  %1 = alloca i8, align 1
  %2 = alloca i32, align 4, !ConsumedBy !12
  %3 = alloca i32, align 4, !ConsumedBy !15
  %4 = alloca i32, align 4, !ConsumedBy !18
  store i8 43, i8* %1, align 1
  store i32 0, i32* %2, align 4
  store i32 1, i32* %3, align 4
  store i32 2, i32* %4, align 4
  %5 = load i8* %1, align 1
  %6 = sext i8 %5 to i32
  %7 = icmp eq i32 %6, 42
  store i1 %7, i1* %0, align 4
  br i1 %7, label %redefine_start1.if.then, label %redefine_start1.return

redefine_start1.if.then:                          ; preds = %redefine_start1.entry
  %8 = load i32* %3, align 4
  %9 = load i32* %4, align 4
  br label %redefine_start1.return

redefine_start1.return:                           ; preds = %redefine_start1.if.then, %redefine_start1.entry
  ret void
}

define void @redefine_start2(i32*, i32*, i32*) {
redefine_start2.if.else:
  %3 = alloca i1, align 4, !Controls !21
  %4 = load i32* %0, align 4
  %5 = load i32* %1, align 4
  %6 = add nsw i32 %4, %5
  store i32 %6, i32* %2, align 4
  br label %redefine_start2.return

redefine_start2.return:                           ; preds = %redefine_start2.if.else
  store i1 true, i1* %3, align 4
  ret void
}

define void @mul3(i32 inreg, i32 inreg) {
mul3.entry:
  %2 = alloca i32, align 4, !ConsumedBy !23
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  %5 = load i32* %3, align 4
  %6 = load i32* %4, align 4
  %7 = mul nsw i32 %5, %6
  store i32 %7, i32* %2, align 4
  br label %mul3.return

mul3.return:                                      ; preds = %mul3.entry
  ret void
}

define void @redefine_start4() {
redefine_start4.if.end:
  br label %redefine_start4.return

redefine_start4.return:                           ; preds = %redefine_start4.if.end
  ret void
}

define void @redefine_start5(i32 inreg, i32*) {
redefine_start5.newName01:
  %2 = alloca i1, align 4, !Controls !21
  store i32 %0, i32* %1, align 4
  br label %redefine_start5.return

redefine_start5.return:                           ; preds = %redefine_start5.newName01
  store i1 true, i1* %2, align 4
  ret void
}

!redefine.annotations = !{!0, !1, !2, !3, !4, !5, !6, !7, !8, !9}

!0 = metadata !{metadata !"HyperOp", void ()* @redefine_start1}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void (i32*, i32*, i32*)* @redefine_start2}
!3 = metadata !{metadata !"Intermediate", metadata !2}
!4 = metadata !{metadata !"HyperOp", void (i32, i32)* @mul3}
!5 = metadata !{metadata !"Intermediate", metadata !4}
!6 = metadata !{metadata !"HyperOp", void ()* @redefine_start4}
!7 = metadata !{metadata !"Exit", metadata !6}
!8 = metadata !{metadata !"HyperOp", void (i32, i32*)* @redefine_start5}
!9 = metadata !{metadata !"Intermediate", metadata !8}
!10 = metadata !{metadata !11}
!11 = metadata !{metadata !2}
!12 = metadata !{metadata !13, metadata !14}
!13 = metadata !{metadata !2, metadata !"LocalReference", i32 2}
!14 = metadata !{metadata !8, metadata !"LocalReference", i32 1}
!15 = metadata !{metadata !16, metadata !17}
!16 = metadata !{metadata !2, metadata !"LocalReference", i32 0}
!17 = metadata !{metadata !4, metadata !"Scalar", i32 0}
!18 = metadata !{metadata !19, metadata !20}
!19 = metadata !{metadata !2, metadata !"LocalReference", i32 1}
!20 = metadata !{metadata !4, metadata !"Scalar", i32 1}
!21 = metadata !{metadata !22}
!22 = metadata !{metadata !6}
!23 = metadata !{metadata !24}
!24 = metadata !{metadata !8, metadata !"Scalar", i32 0}
