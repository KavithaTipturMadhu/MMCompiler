; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/datatypes/floattest.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
define float @test() #0 {
entry:
  %num = alloca float, align 4
  %repeats = alloca i32, align 4
  %out = alloca float, align 4
  %i = alloca i32, align 4
  store float 9.000000e+00, float* %num, align 4
  store i32 10, i32* %repeats, align 4
  store float 1.000000e+00, float* %out, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32* %i, align 4
  %1 = load i32* %repeats, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %2 = load float* %out, align 4
  %3 = load float* %num, align 4
  %mul = fmul float %2, %3
  store float %mul, float* %out, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %4 = load i32* %i, align 4
  %inc = add nsw i32 %4, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %5 = load float* %out, align 4
  ret float %5
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }


!redefine.annotations = !{!0}

!0 = metadata !{metadata !"HyperOp", float ()* @test, i32 0}
