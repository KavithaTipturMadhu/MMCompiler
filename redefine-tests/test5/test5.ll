; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/test5/test5.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "redefine"

; Function Attrs: nounwind uwtable
define i32 @test() #0 {
entry:
  %a = alloca [1 x i32], align 4
  %i = alloca i32, align 4
  %x = alloca [1 x i32], align 4
  %b = alloca i32*, align 8
  %arraydecay = getelementptr inbounds [1 x i32]* %x, i32 0, i32 0
  store i32* %arraydecay, i32** %b, align 8
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32* %i, align 4
  %cmp = icmp slt i32 %0, 1
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %1 = load i32* %i, align 4
  %2 = load i32* %i, align 4
  %mul = mul nsw i32 %1, %2
  %3 = load i32* %i, align 4
  %idxprom = sext i32 %3 to i64
  %arrayidx = getelementptr inbounds [1 x i32]* %a, i32 0, i64 %idxprom
  store i32 %mul, i32* %arrayidx, align 4
  %4 = load i32* %i, align 4
  %idxprom1 = sext i32 %4 to i64
  %arrayidx2 = getelementptr inbounds [1 x i32]* %a, i32 0, i64 %idxprom1
  %5 = load i32* %arrayidx2, align 4
  %6 = load i32* %i, align 4
  %add = add nsw i32 %5, %6
  %7 = load i32** %b, align 8
  %8 = load i32* %i, align 4
  %idx.ext = sext i32 %8 to i64
  %add.ptr = getelementptr inbounds i32* %7, i64 %idx.ext
  store i32 %add, i32* %add.ptr, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %9 = load i32* %i, align 4
  %inc = add nsw i32 %9, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %arrayidx3 = getelementptr inbounds [1 x i32]* %a, i32 0, i64 0
  %10 = load i32* %arrayidx3, align 4
  ret i32 %10
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }

!redefine.annotations = !{!0}

!0 = metadata !{metadata !"HyperOp", i32 ()* @test, i32 0}
