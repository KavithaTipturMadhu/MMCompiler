; ModuleID = '/home/kavitha/Desktop/riscv-workspace/redefine-llvm-tests/test4/test4.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "redefine"

; Function Attrs: nounwind uwtable
define i32 @test() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %e = alloca i32, align 4
  %f = alloca i32, align 4
  %g = alloca i32, align 4
  %t = alloca i32, align 4
  %maxVal = alloca i32, align 4
  %"reg2mem alloca point" = bitcast i32 0 to i32
  store i32 8192, i32* %a, align 4
  store i32 5, i32* %b, align 4
  store i32 2, i32* %c, align 4
  store i32 8196, i32* %d, align 4
  store i32 1, i32* %t, align 4
  %0 = load i32* %b, align 4
  store i32 %0, i32* %maxVal, align 4
  br label %while.cond

while.cond:                                       ; preds = %sw.epilog, %entry
  %1 = load i32* %t, align 4
  %2 = load i32* %maxVal, align 4
  %cmp = icmp slt i32 %1, %2
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %3 = load i32* %t, align 4
  switch i32 %3, label %sw.default [
    i32 1, label %sw.bb
    i32 2, label %sw.bb1
  ]

sw.bb:                                            ; preds = %while.body
  %4 = load i32* %t, align 4
  %5 = load i32* %a, align 4
  %mul = mul nsw i32 %4, %5
  store i32 %mul, i32* %e, align 4
  br label %sw.epilog

sw.bb1:                                           ; preds = %while.body
  %6 = load i32* %t, align 4
  %div = sdiv i32 %6, 3
  store i32 %div, i32* %e, align 4
  br label %sw.epilog

sw.default:                                       ; preds = %while.body
  %7 = load i32* %e, align 4
  %8 = load i32* %t, align 4
  %mul2 = mul nsw i32 %7, %8
  store i32 %mul2, i32* %e, align 4
  br label %sw.epilog

sw.epilog:                                        ; preds = %sw.default, %sw.bb1, %sw.bb
  %9 = load i32* %t, align 4
  %10 = load i32* %b, align 4
  %add = add nsw i32 %9, %10
  store i32 %add, i32* %f, align 4
  %11 = load i32* %e, align 4
  %12 = load i32* %f, align 4
  %add3 = add nsw i32 %11, %12
  store i32 %add3, i32* %g, align 4
  %13 = load i32* %t, align 4
  %inc = add nsw i32 %13, 1
  store i32 %inc, i32* %t, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %14 = load i32* %g, align 4
  %cmp4 = icmp sgt i32 %14, 20
  br i1 %cmp4, label %if.then, label %if.else

if.then:                                          ; preds = %while.end
  %15 = load i32* %g, align 4
  store i32 %15, i32* %retval
  br label %return

if.else:                                          ; preds = %while.end
  store i32 20, i32* %retval
  br label %return

return:                                           ; preds = %if.else, %if.then
  %16 = load i32* %retval
  ret i32 %16
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }

!redefine.annotations = !{!0}

!0 = metadata !{metadata !"HyperOp", i32 ()* @test, i32 0}
