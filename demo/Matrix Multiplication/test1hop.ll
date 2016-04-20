; ModuleID = '/home/kavitha/Desktop/tests/Matrix Multiplication/test1.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

@a = common global [4 x [4 x i32]] zeroinitializer, align 4
@b = common global [4 x [4 x i32]] zeroinitializer, align 4
@c = common global [4 x [4 x i32]] zeroinitializer, align 4

; Function Attrs: nounwind
define void @main() #0 {
entry:
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  %k = alloca i32, align 4
  store i32 0, i32* %k, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc15, %entry
  %0 = load i32* %k, align 4
  %cmp = icmp slt i32 %0, 4
  br i1 %cmp, label %for.body, label %for.end17

for.body:                                         ; preds = %for.cond
  store i32 0, i32* %i, align 4
  br label %for.cond1

for.cond1:                                        ; preds = %for.inc12, %for.body
  %1 = load i32* %i, align 4
  %cmp2 = icmp slt i32 %1, 4
  br i1 %cmp2, label %for.body3, label %for.end14

for.body3:                                        ; preds = %for.cond1
  store i32 0, i32* %j, align 4
  br label %for.cond4

for.cond4:                                        ; preds = %for.inc, %for.body3
  %2 = load i32* %j, align 4
  %cmp5 = icmp slt i32 %2, 4
  br i1 %cmp5, label %for.body6, label %for.end

for.body6:                                        ; preds = %for.cond4
  %3 = load i32* %k, align 4
  %4 = load i32* %i, align 4
  %arrayidx = getelementptr inbounds [4 x [4 x i32]]* @a, i32 0, i32 %4
  %arrayidx7 = getelementptr inbounds [4 x i32]* %arrayidx, i32 0, i32 %3
  %5 = load i32* %arrayidx7, align 4
  %6 = load i32* %j, align 4
  %7 = load i32* %k, align 4
  %arrayidx8 = getelementptr inbounds [4 x [4 x i32]]* @b, i32 0, i32 %7
  %arrayidx9 = getelementptr inbounds [4 x i32]* %arrayidx8, i32 0, i32 %6
  %8 = load i32* %arrayidx9, align 4
  %mul = mul nsw i32 %5, %8
  %9 = load i32* %j, align 4
  %10 = load i32* %i, align 4
  %arrayidx10 = getelementptr inbounds [4 x [4 x i32]]* @c, i32 0, i32 %10
  %arrayidx11 = getelementptr inbounds [4 x i32]* %arrayidx10, i32 0, i32 %9
  %11 = load i32* %arrayidx11, align 4
  %add = add nsw i32 %11, %mul
  store i32 %add, i32* %arrayidx11, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body6
  %12 = load i32* %j, align 4
  %inc = add nsw i32 %12, 1
  store i32 %inc, i32* %j, align 4
  br label %for.cond4

for.end:                                          ; preds = %for.cond4
  br label %for.inc12

for.inc12:                                        ; preds = %for.end
  %13 = load i32* %i, align 4
  %inc13 = add nsw i32 %13, 1
  store i32 %inc13, i32* %i, align 4
  br label %for.cond1

for.end14:                                        ; preds = %for.cond1
  br label %for.inc15

for.inc15:                                        ; preds = %for.end14
  %14 = load i32* %k, align 4
  %inc16 = add nsw i32 %14, 1
  store i32 %inc16, i32* %k, align 4
  br label %for.cond

for.end17:                                        ; preds = %for.cond
  ret void
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
