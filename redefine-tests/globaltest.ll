; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/globaltest.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

%struct.mytest = type { i32, i32 }

@a = common global [10 x %struct.mytest] zeroinitializer, align 4
@b = common global [10 x %struct.mytest] zeroinitializer, align 4

; Function Attrs: nounwind
define void @main() #0 {
entry:
  %i = alloca i32, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32* %i, align 4
  %cmp = icmp slt i32 %0, 10
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %1 = load i32* %i, align 4
  %2 = load i32* %i, align 4
  %arrayidx = getelementptr inbounds [10 x %struct.mytest]* @a, i32 0, i32 %2
  %x = getelementptr inbounds %struct.mytest* %arrayidx, i32 0, i32 0
  store i32 %1, i32* %x, align 4
  %3 = load i32* %i, align 4
  %add = add nsw i32 %3, 10
  %4 = load i32* %i, align 4
  %arrayidx1 = getelementptr inbounds [10 x %struct.mytest]* @a, i32 0, i32 %4
  %y = getelementptr inbounds %struct.mytest* %arrayidx1, i32 0, i32 1
  store i32 %add, i32* %y, align 4
  %5 = load i32* %i, align 4
  %add2 = add nsw i32 %5, 1
  %6 = load i32* %i, align 4
  %arrayidx3 = getelementptr inbounds [10 x %struct.mytest]* @b, i32 0, i32 %6
  %x4 = getelementptr inbounds %struct.mytest* %arrayidx3, i32 0, i32 0
  store i32 %add2, i32* %x4, align 4
  %7 = load i32* %i, align 4
  %mul = mul nsw i32 %7, 18
  %8 = load i32* %i, align 4
  %arrayidx5 = getelementptr inbounds [10 x %struct.mytest]* @b, i32 0, i32 %8
  %y6 = getelementptr inbounds %struct.mytest* %arrayidx5, i32 0, i32 1
  store i32 %mul, i32* %y6, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %9 = load i32* %i, align 4
  %inc = add nsw i32 %9, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret void
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }


!redefine.annotations = !{!0}

!0 = metadata !{metadata !"HyperOp", void ()* @main}
