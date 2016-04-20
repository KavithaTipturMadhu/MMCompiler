; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/test1/test1.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

@a = common global [10 x [10 x i32]] zeroinitializer, align 4
@b = common global [10 x [10 x i32]] zeroinitializer, align 4
@c = common global [10 x [10 x i32]] zeroinitializer, align 4

; Function Attrs: nounwind
define void @main() #0 {
entry:
  %0 = alloca i32, align 4
  store i32 0, i32* %0, align 4, !ConsumedBy !6
  %1 = alloca i32, align 4
  store i32 1, i32* %1, align 4, !ConsumedBy !7
  %2 = alloca i32, align 4
  store i32 2, i32* %2, align 4, !ConsumedBy !8
  %3 = alloca i32, align 4
  store i32 3, i32* %3, align 4, !ConsumedBy !9
  ret void
}

; Function Attrs: nounwind
define void @main1(i32 inreg) #1 {
for.cond:                                         ; preds = %for.inc15, %entry
  %1 = load i32* %i, align 4
  %cmp = icmp slt i32 %0, 4
  br i1 %cmp, label %for.body, label %for.end17

for.body:                                         ; preds = %for.cond
  store i32 0, i32* %i, align 4
  br label %for.cond4

for.cond1:                                        ; preds = %for.inc12, %for.body
  %2 = load i32* %j, align 4
  %cmp2 = icmp slt i32 %1, 4
  br i1 %cmp2, label %for.body3, label %for.end14

for.body1:                                        ; preds = %for.cond1
  %3 = load i32* %i, align 4
  %arrayidx = getelementptr inbounds [10 x [10 x i32]]* @a, i32 0, i32 %4
  %arrayidx7 = getelementptr inbounds [10 x i32]* %arrayidx, i32 0, i32 %3
  %5 = load i32* %arrayidx7, align 4
  %6 = load i32* %j, align 4
  %arrayidx8 = getelementptr inbounds [10 x [10 x i32]]* @b, i32 0, i32 %0
  %arrayidx9 = getelementptr inbounds [10 x i32]* %arrayidx8, i32 0, i32 %1
  %8 = load i32* %arrayidx9, align 4
  %add = add nsw i32 %5, %8
  %9 = load i32* %j, align 4
  %10 = load i32* %i, align 4
  %arrayidx10 = getelementptr inbounds [10 x [10 x i32]]* @c, i32 0, i32 %10
  %arrayidx11 = getelementptr inbounds [10 x i32]* %arrayidx10, i32 0, i32 %9
  store i32 %add, i32* %arrayidx11, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body6
  %11 = load i32* %j, align 4
  %inc = add nsw i32 %11, 1
  store i32 %inc, i32* %j, align 4
  br label %for.cond4

for.end:                                          ; preds = %for.cond4
  br label %for.inc12

for.inc12:                                        ; preds = %for.end
  %12 = load i32* %i, align 4
  %inc13 = add nsw i32 %12, 1
  store i32 %inc13, i32* %i, align 4
  br label %for.cond1

for.end14:                                        ; preds = %for.cond1
  br label %for.end16

for.end16:                                        
  %13 = alloca i32, align 4
  store i32 1, i32* %13, align 4, !ConsumedBy !14
  ret void
}


; Function Attrs: nounwind
define void @main0(i32 inreg) #1 {
for.cond:                                         ; preds = %for.inc12
  %1 = load i32* %i, align 4
  %cmp = icmp slt i32 %0, 4
  br i1 %cmp, label %for.body, label %for.end17

for.body:                                         ; preds = %for.cond
  store i32 0, i32* %i, align 4
  br label %for.cond4

for.cond1:                                        ; preds = %for.inc, %for.body
  %2 = load i32* %j, align 4
  %cmp2 = icmp slt i32 %1, 4
  br i1 %cmp2, label %for.body3, label %for.end14

for.body1:                                        ; preds = %for.cond1
  %3 = load i32* %i, align 4
  %arrayidx = getelementptr inbounds [10 x [10 x i32]]* @a, i32 0, i32 %4
  %arrayidx7 = getelementptr inbounds [10 x i32]* %arrayidx, i32 0, i32 %3
  %5 = load i32* %arrayidx7, align 4
  %6 = load i32* %j, align 4
  %arrayidx8 = getelementptr inbounds [10 x [10 x i32]]* @b, i32 0, i32 %0
  %arrayidx9 = getelementptr inbounds [10 x i32]* %arrayidx8, i32 0, i32 %1
  %8 = load i32* %arrayidx9, align 4
  %add = add nsw i32 %5, %8
  %9 = load i32* %j, align 4
  %10 = load i32* %i, align 4
  %arrayidx10 = getelementptr inbounds [10 x [10 x i32]]* @c, i32 0, i32 %10
  %arrayidx11 = getelementptr inbounds [10 x i32]* %arrayidx10, i32 0, i32 %9
  store i32 %add, i32* %arrayidx11, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body6
  %11 = load i32* %j, align 4
  %inc = add nsw i32 %11, 1
  store i32 %inc, i32* %j, align 4
  br label %for.cond4

for.end:                                          ; preds = %for.cond4
  br label %for.inc12

for.inc12:                                        ; preds = %for.end
  %12 = load i32* %i, align 4
  %inc13 = add nsw i32 %12, 1
  store i32 %inc13, i32* %i, align 4
  br label %for.cond1

for.end14:                                        ; preds = %for.cond1
  br label %for.end16

for.end16:                                        
  %13 = alloca i32, align 4
  store i32 1, i32* %13, align 4, !ConsumedBy !15
  ret void
}

; Function Attrs: nounwind
define void @main1(i32 inreg) #2 {
for.cond:                                         ; preds = %for.inc15, %entry
  %1 = load i32* %i, align 4
  %cmp = icmp slt i32 %0, 4
  br i1 %cmp, label %for.body, label %for.end17

for.body:                                         ; preds = %for.cond
  store i32 0, i32* %i, align 4
  br label %for.cond4

for.cond1:                                        ; preds = %for.inc12, %for.body
  %2 = load i32* %j, align 4
  %cmp2 = icmp slt i32 %1, 4
  br i1 %cmp2, label %for.body3, label %for.end14

for.body1:                                        ; preds = %for.cond1
  %3 = load i32* %i, align 4
  %arrayidx = getelementptr inbounds [10 x [10 x i32]]* @a, i32 0, i32 %4
  %arrayidx7 = getelementptr inbounds [10 x i32]* %arrayidx, i32 0, i32 %3
  %5 = load i32* %arrayidx7, align 4
  %6 = load i32* %j, align 4
  %arrayidx8 = getelementptr inbounds [10 x [10 x i32]]* @b, i32 0, i32 %0
  %arrayidx9 = getelementptr inbounds [10 x i32]* %arrayidx8, i32 0, i32 %1
  %8 = load i32* %arrayidx9, align 4
  %add = add nsw i32 %5, %8
  %9 = load i32* %j, align 4
  %10 = load i32* %i, align 4
  %arrayidx10 = getelementptr inbounds [10 x [10 x i32]]* @c, i32 0, i32 %10
  %arrayidx11 = getelementptr inbounds [10 x i32]* %arrayidx10, i32 0, i32 %9
  store i32 %add, i32* %arrayidx11, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body6
  %11 = load i32* %j, align 4
  %inc = add nsw i32 %11, 1
  store i32 %inc, i32* %j, align 4
  br label %for.cond4

for.end:                                          ; preds = %for.cond4
  br label %for.inc12

for.inc12:                                        ; preds = %for.end
  %12 = load i32* %i, align 4
  %inc13 = add nsw i32 %12, 1
  store i32 %inc13, i32* %i, align 4
  br label %for.cond1

for.end14:                                        ; preds = %for.cond1
  br label %for.end16

for.end16:                                        ; preds = %for.cond
  ret void
}


; Function Attrs: nounwind
define void @main2(i32 inreg) #3 {
for.cond:                                         ; preds = %for.inc15, %entry
  %1 = load i32* %i, align 4
  %cmp = icmp slt i32 %0, 4
  br i1 %cmp, label %for.body, label %for.end17

for.body:                                         ; preds = %for.cond
  store i32 0, i32* %i, align 4
  br label %for.cond4

for.cond1:                                        ; preds = %for.inc12, %for.body
  %2 = load i32* %j, align 4
  %cmp2 = icmp slt i32 %1, 4
  br i1 %cmp2, label %for.body3, label %for.end14

for.body1:                                        ; preds = %for.cond1
  %3 = load i32* %i, align 4
  %arrayidx = getelementptr inbounds [10 x [10 x i32]]* @a, i32 0, i32 %4
  %arrayidx7 = getelementptr inbounds [10 x i32]* %arrayidx, i32 0, i32 %3
  %5 = load i32* %arrayidx7, align 4
  %6 = load i32* %j, align 4
  %arrayidx8 = getelementptr inbounds [10 x [10 x i32]]* @b, i32 0, i32 %0
  %arrayidx9 = getelementptr inbounds [10 x i32]* %arrayidx8, i32 0, i32 %1
  %8 = load i32* %arrayidx9, align 4
  %add = add nsw i32 %5, %8
  %9 = load i32* %j, align 4
  %10 = load i32* %i, align 4
  %arrayidx10 = getelementptr inbounds [10 x [10 x i32]]* @c, i32 0, i32 %10
  %arrayidx11 = getelementptr inbounds [10 x i32]* %arrayidx10, i32 0, i32 %9
  store i32 %add, i32* %arrayidx11, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body6
  %11 = load i32* %j, align 4
  %inc = add nsw i32 %11, 1
  store i32 %inc, i32* %j, align 4
  br label %for.cond4

for.end:                                          ; preds = %for.cond4
  br label %for.inc12

for.inc12:                                        ; preds = %for.end
  %12 = load i32* %i, align 4
  %inc13 = add nsw i32 %12, 1
  store i32 %inc13, i32* %i, align 4
  br label %for.cond1

for.end14:                                        ; preds = %for.cond1
  br label %for.end16

for.end16:                                        
  %13 = alloca i32, align 4
  store i32 1, i32* %13, align 4, !ConsumedBy !16
  ret void
}

; Function Attrs: nounwind
define void @main3(i32 inreg) #4 {
for.cond:                                         ; preds = %for.inc15, %entry
  %1 = load i32* %i, align 4
  %cmp = icmp slt i32 %0, 4
  br i1 %cmp, label %for.body, label %for.end17

for.body:                                         ; preds = %for.cond
  store i32 0, i32* %i, align 4
  br label %for.cond4

for.cond1:                                        ; preds = %for.inc12, %for.body
  %2 = load i32* %j, align 4
  %cmp2 = icmp slt i32 %1, 4
  br i1 %cmp2, label %for.body3, label %for.end14

for.body1:                                        ; preds = %for.cond1
  %3 = load i32* %i, align 4
  %arrayidx = getelementptr inbounds [10 x [10 x i32]]* @a, i32 0, i32 %4
  %arrayidx7 = getelementptr inbounds [10 x i32]* %arrayidx, i32 0, i32 %3
  %5 = load i32* %arrayidx7, align 4
  %6 = load i32* %j, align 4
  %arrayidx8 = getelementptr inbounds [10 x [10 x i32]]* @b, i32 0, i32 %0
  %arrayidx9 = getelementptr inbounds [10 x i32]* %arrayidx8, i32 0, i32 %1
  %8 = load i32* %arrayidx9, align 4
  %add = add nsw i32 %5, %8
  %9 = load i32* %j, align 4
  %10 = load i32* %i, align 4
  %arrayidx10 = getelementptr inbounds [10 x [10 x i32]]* @c, i32 0, i32 %10
  %arrayidx11 = getelementptr inbounds [10 x i32]* %arrayidx10, i32 0, i32 %9
  store i32 %add, i32* %arrayidx11, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body6
  %11 = load i32* %j, align 4
  %inc = add nsw i32 %11, 1
  store i32 %inc, i32* %j, align 4
  br label %for.cond4

for.end:                                          ; preds = %for.cond4
  br label %for.inc12

for.inc12:                                        ; preds = %for.end
  %12 = load i32* %i, align 4
  %inc13 = add nsw i32 %12, 1
  store i32 %inc13, i32* %i, align 4
  br label %for.cond1

for.end14:                                        ; preds = %for.cond1
  br label %for.end16

for.end16:                                        
  %13 = alloca i32, align 4
  store i32 1, i32* %13, align 4, !ConsumedBy !17
  ret void
}

; Function Attrs: nounwind
define void @main4(i32 inreg, i32 inreg, i32 inreg, i32 inreg) #5 {
exit:
  ret void
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }

!redefine.annotations = !{!0, !1, !2, !3, !4, !5}

!0 = metadata !{metadata !"HyperOp", void ()* @main}
!1 = metadata !{metadata !"HyperOp", void (i32 )* @main0}
!2 = metadata !{metadata !"HyperOp", void (i32 )* @main1}
!3 = metadata !{metadata !"HyperOp", void (i32 )* @main2}
!4 = metadata !{metadata !"HyperOp", void (i32 )* @main3}
!5 = metadata !{metadata !"HyperOp", void (i32 )* @main4}
!6 = metadata !{metadata !10}
!7 = metadata !{metadata !11}
!8 = metadata !{metadata !12}
!9 = metadata !{metadata !13}
!10 = metadata !{metadata !1, metadata !"Scalar", metadata!"0"}
!11 = metadata !{metadata !2, metadata !"Scalar", metadata!"0"}
!12 = metadata !{metadata !3, metadata !"Scalar", metadata!"0"}
!13 = metadata !{metadata !4, metadata !"Scalar", metadata!"0"}
!14 = metadata !{metadata !18}
!15 = metadata !{metadata !19}
!16 = metadata !{metadata !20}
!17 = metadata !{metadata !21}
!18 = metadata !{metadata !5, metadata!"Scalar", metadata!"0"}
!19 = metadata !{metadata !5, metadata!"Scalar", metadata!"1"}
!20 = metadata !{metadata !5, metadata!"Scalar", metadata!"2"}
!21 = metadata !{metadata !5, metadata!"Scalar", metadata!"3"}
!22 = metadata !{metadata !0, "Entry"}
!23 = metadata !{metadata !0, "Exit"}
