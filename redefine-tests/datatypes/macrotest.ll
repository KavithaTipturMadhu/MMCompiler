; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/datatypes/macrotest.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "x86"

; Function Attrs: nounwind
define i32 @test() #0 {
entry:
  %retval = alloca i32, align 4
  %f = alloca i32, align 4
  store i32 20000, i32* %f, align 4
  %0 = load i32* %f, align 4
  %cmp = icmp sgt i32 10000, %0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store i32 100, i32* %retval
  br label %return

if.end:                                           ; preds = %entry
  %1 = load i32* %f, align 4
  store i32 %1, i32* %retval
  br label %return

return:                                           ; preds = %if.end, %if.then
  %2 = load i32* %retval
  ret i32 %2
}

; Function Attrs: nounwind
define i32 @rest() #0 {
entry:
  ret i32 0
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
