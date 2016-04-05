; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/stringtest/stringtest.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

@.str = private unnamed_addr constant [6 x i8] c"hello\00", align 1

; Function Attrs: nounwind
define void @test() #0 {
entry:
  %string = alloca i8*, align 4
  %count = alloca i32, align 4
  %j = alloca i32, align 4
  store i8* getelementptr inbounds ([6 x i8]* @.str, i32 0, i32 0), i8** %string, align 4
  store i32 0, i32* %j, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %0 = load i32* %j, align 4
  %inc = add nsw i32 %0, 1
  store i32 %inc, i32* %j, align 4
  %1 = load i8** %string, align 4
  %arrayidx = getelementptr inbounds i8* %1, i32 %0
  %2 = load i8* %arrayidx, align 1
  %conv = sext i8 %2 to i32
  %cmp = icmp ne i32 %conv, 0
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %3 = load i32* %count, align 4
  %inc2 = add nsw i32 %3, 1
  store i32 %inc2, i32* %count, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  ret void
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
