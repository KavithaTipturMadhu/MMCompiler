; ModuleID = '/home/kavitha/Desktop/REDEFINE_LLVM/MM_Compiler/redefine-tests/operatortests/min.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind uwtable
define float @main() #0 {
entry:
  %a = alloca float, align 4
  %b = alloca float, align 4
  %minimum = alloca float, align 4
  %0 = load float* %a, align 4
  %conv = fpext float %0 to double
  %1 = load float* %b, align 4
  %conv1 = fpext float %1 to double
  %2 = call double @llvm.minnum.f64(double %conv, double %conv1)
  %conv2 = fptrunc double %2 to float
  store float %conv2, float* %minimum, align 4
  %3 = load float* %minimum, align 4
  ret float %3
}

; Function Attrs: nounwind readonly
declare double @llvm.minnum.f64(double, double) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readonly }
