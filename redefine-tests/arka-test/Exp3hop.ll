; ModuleID = 'arka-test/Exp3.ll'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

@a = common global i32 0, align 4
@b = common global i32 0, align 4
@c = common global i32 0, align 4

define void @redefine_start1() {
redefine_start1.entry:
  %0 = alloca i32, align 4, !ConsumedBy !4
  %1 = alloca i32, align 4, !ConsumedBy !6
  %2 = getelementptr inbounds i32* @a, i32 0
  store i32 0, i32* %2, align 4
  %3 = getelementptr inbounds i32* @b, i32 0
  store i32 0, i32* %3, align 4
  %4 = getelementptr inbounds i32* @c, i32 0
  store i32 0, i32* %4, align 4
  %5 = alloca i32*, align 4, !ConsumedBy !8
  store i32 10, i32* @a, align 4
  store i32 19, i32* @b, align 4
  %6 = load i32* @a, align 4
  store i32 %6, i32* %1, align 4
  %7 = load i32* @b, align 4
  store i32 %7, i32* %0, align 4
  store i32* @c, i32** %5
  %8 = load i32** %5
  br label %redefine_start1.return

redefine_start1.return:                           ; preds = %redefine_start1.entry
  ret void
}

define void @swap2(i32 inreg, i32 inreg, i32* inreg) {
swap2.entry:
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32*, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  store i32* %2, i32** %5, align 4
  %6 = load i32* %3, align 4
  %7 = load i32* %4, align 4
  %8 = add nsw i32 %6, %7
  %9 = load i32** %5, align 4
  store i32 %8, i32* %9, align 4
  br label %swap2.return

swap2.return:                                     ; preds = %swap2.entry
  ret void
}

!redefine.annotations = !{!0, !1, !2, !3}

!0 = metadata !{metadata !"HyperOp", void ()* @redefine_start1, metadata !"Static"}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void (i32, i32, i32*)* @swap2, metadata !"Static"}
!3 = metadata !{metadata !"Exit", metadata !2}
!4 = metadata !{metadata !5}
!5 = metadata !{metadata !2, metadata !"Scalar", i32 1}
!6 = metadata !{metadata !7}
!7 = metadata !{metadata !2, metadata !"Scalar", i32 0}
!8 = metadata !{metadata !9}
!9 = metadata !{metadata !2, metadata !"Scalar", i32 2}
