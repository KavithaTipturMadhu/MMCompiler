; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/arraytests/test1/quicksort.ll'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

@test.a = private unnamed_addr constant [8 x i32] [i32 7, i32 6, i32 5, i32 4, i32 3, i32 2, i32 1, i32 0], align 4

; Function Attrs: nounwind
define void @test() #0 {
entry:
  %a = alloca [8 x i32], align 4
  %0 = bitcast [8 x i32]* %a to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %0, i8* bitcast ([8 x i32]* @test.a to i8*), i32 32, i32 4, i1 false)
  br label %for.cond

for.cond:                                         ; preds = %for.inc14, %entry
  %index.0 = phi i32 [ undef, %entry ], [ %index.1, %for.inc14 ]
  %i.0 = phi i32 [ 0, %entry ], [ %inc15, %for.inc14 ]
  %cmp = icmp slt i32 %i.0, 7
  br i1 %cmp, label %for.body, label %for.end16

for.body:                                         ; preds = %for.cond
  %add = add nsw i32 %i.0, 1
  br label %for.cond1

for.cond1:                                        ; preds = %for.inc, %for.body
  %index.1 = phi i32 [ %index.0, %for.body ], [ %index.2, %for.inc ]
  %min.0 = phi i32 [ -1, %for.body ], [ %min.1, %for.inc ]
  %j.0 = phi i32 [ %add, %for.body ], [ %inc, %for.inc ]
  %cmp2 = icmp slt i32 %j.0, 8
  br i1 %cmp2, label %for.body3, label %for.end

for.body3:                                        ; preds = %for.cond1
  %arrayidx = getelementptr inbounds [8 x i32]* %a, i32 0, i32 %j.0
  %1 = load i32* %arrayidx, align 4
  %cmp4 = icmp slt i32 %1, %min.0
  br i1 %cmp4, label %if.then, label %lor.lhs.false

lor.lhs.false:                                    ; preds = %for.body3
  %cmp5 = icmp eq i32 %min.0, -1
  br i1 %cmp5, label %if.then, label %if.end

if.then:                                          ; preds = %lor.lhs.false, %for.body3
  %arrayidx6 = getelementptr inbounds [8 x i32]* %a, i32 0, i32 %j.0
  %2 = load i32* %arrayidx6, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %lor.lhs.false
  %index.2 = phi i32 [ %j.0, %if.then ], [ %index.1, %lor.lhs.false ]
  %min.1 = phi i32 [ %2, %if.then ], [ %min.0, %lor.lhs.false ]
  br label %for.inc

for.inc:                                          ; preds = %if.end
  %inc = add nsw i32 %j.0, 1
  br label %for.cond1

for.end:                                          ; preds = %for.cond1
  %arrayidx7 = getelementptr inbounds [8 x i32]* %a, i32 0, i32 %i.0
  %3 = load i32* %arrayidx7, align 4
  %cmp8 = icmp sgt i32 %3, %min.0
  br i1 %cmp8, label %if.then9, label %if.end13

if.then9:                                         ; preds = %for.end
  %arrayidx10 = getelementptr inbounds [8 x i32]* %a, i32 0, i32 %i.0
  %4 = load i32* %arrayidx10, align 4
  %arrayidx11 = getelementptr inbounds [8 x i32]* %a, i32 0, i32 %i.0
  store i32 %min.0, i32* %arrayidx11, align 4
  %arrayidx12 = getelementptr inbounds [8 x i32]* %a, i32 0, i32 %index.1
  store i32 %4, i32* %arrayidx12, align 4
  br label %if.end13

if.end13:                                         ; preds = %if.then9, %for.end
  br label %for.inc14

for.inc14:                                        ; preds = %if.end13
  %inc15 = add nsw i32 %i.0, 1
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
