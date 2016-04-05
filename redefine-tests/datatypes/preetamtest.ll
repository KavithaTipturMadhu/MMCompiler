; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/datatypes/preetamtest.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
define i32 @test() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %g = alloca i32, align 4
  %e = alloca i8, align 1
  %f = alloca i8, align 1
  %x = alloca float, align 4
  %y = alloca float, align 4
  store i32 273, i32* %g, align 4
  store i8 119, i8* %e, align 1
  store float 0x400921CAC0000000, float* %x, align 4
  store float 0x3FC2137F40000000, float* %y, align 4
  %0 = load i32* %g, align 4
  %shl = shl i32 %0, 1
  %1 = load i32* %g, align 4
  %shr = ashr i32 %1, 1
  %cmp = icmp sgt i32 %shl, %shr
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %2 = load i8* %e, align 1
  %conv = sext i8 %2 to i32
  %conv1 = sitofp i32 %conv to float
  %3 = load float* %x, align 4
  %cmp2 = fcmp ogt float %conv1, %3
  br i1 %cmp2, label %if.then4, label %if.else

if.then4:                                         ; preds = %if.then
  %4 = load i8* %e, align 1
  %conv5 = sext i8 %4 to i32
  store i32 %conv5, i32* %retval
  br label %return

if.else:                                          ; preds = %if.then
  %5 = load float* %x, align 4
  %6 = load float* %y, align 4
  %mul = fmul float %5, %6
  %7 = load float* %x, align 4
  %cmp6 = fcmp ogt float %mul, %7
  br i1 %cmp6, label %if.then8, label %if.else10

if.then8:                                         ; preds = %if.else
  %8 = load float* %x, align 4
  %conv9 = fptosi float %8 to i32
  store i32 %conv9, i32* %retval
  br label %return

if.else10:                                        ; preds = %if.else
  %9 = load float* %y, align 4
  %conv11 = fptosi float %9 to i32
  store i32 %conv11, i32* %retval
  br label %return

if.end:                                           ; preds = %entry
  %10 = load i32* %a, align 4
  %add = add nsw i32 %10, 10
  store i32 %add, i32* %retval
  br label %return

return:                                           ; preds = %if.end, %if.else10, %if.then8, %if.then4
  %11 = load i32* %retval
  ret i32 %11
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
