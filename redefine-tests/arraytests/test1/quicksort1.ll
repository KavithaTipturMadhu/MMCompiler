; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/arraytests/test1/quicksort.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
define void @test() #0 {
entry:
  %a = alloca [6 x i32], align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  %min = alloca i32, align 4
  %temp = alloca i32, align 4
  %index = alloca i32, align 4
  %0 = bitcast [6 x i32]* %a to i8*
  call void @llvm.memset.p0i8.i32(i8* %0, i8 0, i32 24, i32 4, i1 false)
  %1 = bitcast i8* %0 to [6 x i32]*
  %2 = getelementptr [6 x i32]* %1, i32 0, i32 0
  store i32 25, i32* %2
  %3 = getelementptr [6 x i32]* %1, i32 0, i32 1
  store i32 10, i32* %3
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc14, %entry
  %4 = load i32* %i, align 4
  %cmp = icmp slt i32 %4, 1
  br i1 %cmp, label %for.body, label %for.end16

for.body:                                         ; preds = %for.cond
  store i32 -1, i32* %min, align 4
  %5 = load i32* %i, align 4
  %add = add nsw i32 %5, 1
  store i32 %add, i32* %j, align 4
  br label %for.cond1

for.cond1:                                        ; preds = %for.inc, %for.body
  %6 = load i32* %j, align 4
  %cmp2 = icmp slt i32 %6, 2
  br i1 %cmp2, label %for.body3, label %for.end

for.body3:                                        ; preds = %for.cond1
  %7 = load i32* %j, align 4
  %arrayidx = getelementptr inbounds [6 x i32]* %a, i32 0, i32 %7
  %8 = load i32* %arrayidx, align 4
  %9 = load i32* %min, align 4
  %cmp4 = icmp slt i32 %8, %9
  br i1 %cmp4, label %if.then, label %lor.lhs.false

lor.lhs.false:                                    ; preds = %for.body3
  %10 = load i32* %min, align 4
  %cmp5 = icmp eq i32 %10, -1
  br i1 %cmp5, label %if.then, label %if.end

if.then:                                          ; preds = %lor.lhs.false, %for.body3
  %11 = load i32* %j, align 4
  %arrayidx6 = getelementptr inbounds [6 x i32]* %a, i32 0, i32 %11
  %12 = load i32* %arrayidx6, align 4
  store i32 %12, i32* %min, align 4
  %13 = load i32* %j, align 4
  store i32 %13, i32* %index, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.lhs.false
  br label %for.inc

for.inc:                                          ; preds = %if.end
  %14 = load i32* %j, align 4
  %inc = add nsw i32 %14, 1
  store i32 %inc, i32* %j, align 4
  br label %for.cond1

for.end:                                          ; preds = %for.cond1
  %15 = load i32* %i, align 4
  %arrayidx7 = getelementptr inbounds [6 x i32]* %a, i32 0, i32 %15
  %16 = load i32* %arrayidx7, align 4
  %17 = load i32* %min, align 4
  %cmp8 = icmp sgt i32 %16, %17
  br i1 %cmp8, label %if.then9, label %if.end13

if.then9:                                         ; preds = %for.end
  %18 = load i32* %i, align 4
  %arrayidx10 = getelementptr inbounds [6 x i32]* %a, i32 0, i32 %18
  %19 = load i32* %arrayidx10, align 4
  store i32 %19, i32* %temp, align 4
  %20 = load i32* %min, align 4
  %21 = load i32* %i, align 4
  %arrayidx11 = getelementptr inbounds [6 x i32]* %a, i32 0, i32 %21
  store i32 %20, i32* %arrayidx11, align 4
  %22 = load i32* %temp, align 4
  %23 = load i32* %index, align 4
  %arrayidx12 = getelementptr inbounds [6 x i32]* %a, i32 0, i32 %23
  store i32 %22, i32* %arrayidx12, align 4
  br label %if.end13

if.end13:                                         ; preds = %if.then9, %for.end
  br label %for.inc14

for.inc14:                                        ; preds = %if.end13
  %24 = load i32* %i, align 4
  %inc15 = add nsw i32 %24, 1
  store i32 %inc15, i32* %i, align 4
  br label %for.cond

for.end16:                                        ; preds = %for.cond
  ret void
}

; Function Attrs: nounwind
declare void @llvm.memset.p0i8.i32(i8* nocapture, i8, i32, i32, i1) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!redefine.annotations = !{!0}

!0 = metadata !{metadata !"HyperOp", void ()* @test, i32 0}
