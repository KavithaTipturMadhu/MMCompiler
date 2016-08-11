; ModuleID = 'arka-test/Exp2.ll'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
declare void @llvm.memset.p0i8.i32(i8* nocapture, i8, i32, i32, i1) #0

define void @redefine_start1() {
redefine_start1.entry:
  %0 = alloca i32*, align 4, !ConsumedBy !6
  %1 = alloca [4 x i32], align 4
  %2 = alloca i32, align 4, !ConsumedBy !8
  %3 = bitcast [4 x i32]* %1 to i8*
  call void @llvm.memset.p0i8.i32(i8* %3, i8 0, i32 16, i32 4, i1 false)
  %4 = bitcast i8* %3 to [4 x i32]*
  %5 = getelementptr [4 x i32]* %4, i32 0, i32 1
  store i32 1, i32* %5
  %6 = getelementptr [4 x i32]* %4, i32 0, i32 2
  store i32 2, i32* %6
  %7 = getelementptr [4 x i32]* %4, i32 0, i32 3
  store i32 4, i32* %7
  store i32 4, i32* %2, align 4
  %8 = getelementptr inbounds [4 x i32]* %1, i32 0, i32 0
  store i32* %8, i32** %0, align 4
  %9 = load i32* %2, align 4
  br label %redefine_start1.return

redefine_start1.return:                           ; preds = %redefine_start1.entry
  ret void
}

define void @sumRed2(i32 inreg, i32*) {
sumRed2.entry:
  %2 = alloca i32, align 4, !ConsumedBy !10
  %3 = alloca i32*, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32* %1, i32** %3, align 4
  store i32 %0, i32* %4, align 4
  store i32 0, i32* %6, align 4
  %7 = load i32** %3, align 4
  %8 = getelementptr inbounds i32* %7, i32 0
  %9 = load i32* %8, align 4
  %10 = load i32* %4, align 4
  %11 = sub nsw i32 %10, 1
  %12 = load i32** %3, align 4
  %13 = getelementptr inbounds i32* %12, i32 %11
  %14 = load i32* %13, align 4
  %15 = add nsw i32 %9, %14
  store i32 %15, i32* %6, align 4
  %16 = load i32* %6, align 4
  store i32 %16, i32* %2, align 4
  br label %sumRed2.return

sumRed2.return:                                   ; preds = %sumRed2.entry
  ret void
}

define void @redefine_start3(i32 inreg) {
redefine_start3.newName01:
  %1 = alloca i32, align 4
  store i32 %0, i32* %1, align 4
  br label %redefine_start3.return

redefine_start3.return:                           ; preds = %redefine_start3.newName01
  ret void
}

attributes #0 = { nounwind }

!redefine.annotations = !{!0, !1, !2, !3, !4, !5}

!0 = metadata !{metadata !"HyperOp", void ()* @redefine_start1}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void (i32, i32*)* @sumRed2}
!3 = metadata !{metadata !"Intermediate", metadata !2}
!4 = metadata !{metadata !"HyperOp", void (i32)* @redefine_start3}
!5 = metadata !{metadata !"Exit", metadata !4}
!6 = metadata !{metadata !7}
!7 = metadata !{metadata !2, metadata !"LocalReference", i32 1}
!8 = metadata !{metadata !9}
!9 = metadata !{metadata !2, metadata !"Scalar", i32 0}
!10 = metadata !{metadata !11}
!11 = metadata !{metadata !4, metadata !"Scalar", i32 0}
