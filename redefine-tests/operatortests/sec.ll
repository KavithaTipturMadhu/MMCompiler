; ModuleID = 'sec.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
define float @sec() #0 {
entry:
  %x = alloca float, align 4
  %sin_x = alloca float, align 4
  %cos_x = alloca float, align 4
  %tan_x = alloca float, align 4
  %sec_x = alloca float, align 4
  store float 0x3FF851EB80000000, float* %x, align 4
  %0 = load float* %x, align 4
  %1 = call float @llvm.sin.f32(float %0)
  store float %1, float* %sin_x, align 4
  %2 = load float* %x, align 4
  %3 = call float @llvm.cos.f32(float %2)
  store float %3, float* %cos_x, align 4
  %4 = load float* %sin_x, align 4
  %5 = load float* %cos_x, align 4
  %div = fdiv float %4, %5
  store float %div, float* %tan_x, align 4
  %6 = load float* %tan_x, align 4
  %7 = load float* %tan_x, align 4
  %mul = fmul float %6, %7
  %add = fadd float 1.000000e+00, %mul
  %8 = call float @llvm.sqrt.f32(float %add)
  store float %8, float* %sec_x, align 4
  %9 = load float* %sec_x, align 4
  ret float %9
}

; Function Attrs: nounwind readonly
declare float @llvm.sin.f32(float) #1

; Function Attrs: nounwind readonly
declare float @llvm.cos.f32(float) #1

; Function Attrs: nounwind readonly
declare float @llvm.sqrt.f32(float) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readonly }

!redefine.annotations = !{!0}
!0 = metadata !{metadata !"HyperOp", float ()* @sec, i32 0}
