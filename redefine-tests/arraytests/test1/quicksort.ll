; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/arraytests/test1/test1.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

@test.a = private unnamed_addr constant [8 x i32] [i32 7, i32 6, i32 5, i32 4, i32 3, i32 2, i32 1, i32 0], align 4

; Function Attrs: nounwind
define void @test() #0 {
entry:
  %a = alloca [8 x i32], align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  %min = alloca i32, align 4
  %temp = alloca i32, align 4
  %index = alloca i32, align 4
  %0 = bitcast [8 x i32]* %a to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %0, i8* bitcast ([8 x i32]* @test.a to i8*), i32 32, i32 4, i1 false)
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc14, %entry
  %1 = load i32* %i, align 4
  %cmp = icmp slt i32 %1, 7
  br i1 %cmp, label %for.body, label %for.end16

for.body:                                         ; preds = %for.cond
  store i32 -1, i32* %min, align 4
  %2 = load i32* %i, align 4
  %add = add nsw i32 %2, 1
  store i32 %add, i32* %j, align 4
  br label %for.cond1

for.cond1:                                        ; preds = %for.inc, %for.body
  %3 = load i32* %j, align 4
  %cmp2 = icmp slt i32 %3, 8
  br i1 %cmp2, label %for.body3, label %for.end

for.body3:                                        ; preds = %for.cond1
  %4 = load i32* %j, align 4
  %arrayidx = getelementptr inbounds [8 x i32]* %a, i32 0, i32 %4
  %5 = load i32* %arrayidx, align 4
  %6 = load i32* %min, align 4
  %cmp4 = icmp slt i32 %5, %6
  br i1 %cmp4, label %if.then, label %lor.lhs.false

lor.lhs.false:                                    ; preds = %for.body3
  %7 = load i32* %min, align 4
  %cmp5 = icmp eq i32 %7, -1
  br i1 %cmp5, label %if.then, label %if.end

if.then:                                          ; preds = %lor.lhs.false, %for.body3
  %8 = load i32* %j, align 4
  %arrayidx6 = getelementptr inbounds [8 x i32]* %a, i32 0, i32 %8
  %9 = load i32* %arrayidx6, align 4
  store i32 %9, i32* %min, align 4
  %10 = load i32* %j, align 4
  store i32 %10, i32* %index, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.lhs.false
  br label %for.inc

for.inc:                                          ; preds = %if.end
  %11 = load i32* %j, align 4
  %inc = add nsw i32 %11, 1
  store i32 %inc, i32* %j, align 4
  br label %for.cond1

for.end:                                          ; preds = %for.cond1
  %12 = load i32* %i, align 4
  %arrayidx7 = getelementptr inbounds [8 x i32]* %a, i32 0, i32 %12
  %13 = load i32* %arrayidx7, align 4
  %14 = load i32* %min, align 4
  %cmp8 = icmp sgt i32 %13, %14
  br i1 %cmp8, label %if.then9, label %if.end13

if.then9:                                         ; preds = %for.end
  %15 = load i32* %i, align 4
  %arrayidx10 = getelementptr inbounds [8 x i32]* %a, i32 0, i32 %15
  %16 = load i32* %arrayidx10, align 4
  store i32 %16, i32* %temp, align 4
  %17 = load i32* %min, align 4
  %18 = load i32* %i, align 4
  %arrayidx11 = getelementptr inbounds [8 x i32]* %a, i32 0, i32 %18
  store i32 %17, i32* %arrayidx11, align 4
  %19 = load i32* %temp, align 4
  %20 = load i32* %index, align 4
  %arrayidx12 = getelementptr inbounds [8 x i32]* %a, i32 0, i32 %20
  store i32 %19, i32* %arrayidx12, align 4
  br label %if.end13

if.end13:                                         ; preds = %if.then9, %for.end
  br label %for.inc14

for.inc14:                                        ; preds = %if.end13
  %21 = load i32* %i, align 4
  %inc15 = add nsw i32 %21, 1
  store i32 %inc15, i32* %i, align 4
  br label %for.cond

for.end16:                                        ; preds = %for.cond
  ret void
}

; Function Attrs: nounwind
declare void @llvm.memcpy.p0i8.p0i8.i32(i8* nocapture, i8* nocapture, i32, i32, i1) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!redefine.annotations = !{!0}

!0 = metadata !{metadata !"HyperOp", void ()* @test, i32 0}
