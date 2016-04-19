; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/datatypes/chartest.ll'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
define i32 @test(i8*) #0 {
entry:
  %retval = alloca i32, align 4
  %e = alloca i8, align 1
  %f = alloca i8, align 1
  store i8 1, i8* %e, align 1
  store i8 2, i8* %f, align 1
  %1 = load i8* %0, align 1
  %conv = sext i8 %1 to i32
  %2 = load i8* %f, align 1
  %conv1 = sext i8 %2 to i32
  %cmp = icmp sgt i32 %conv, %conv1
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %3 = load i8* %e, align 1
  %conv3 = sext i8 %3 to i32
  store i32 %conv3, i32* %retval
  br label %return

if.end:                                           ; preds = %entry
  %4 = load i8* %f, align 1
  %conv4 = sext i8 %4 to i32
  store i32 %conv4, i32* %retval
  br label %return

return:                                           ; preds = %if.end, %if.then
  %5 = load i32* %retval
  ret i32 %5
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }

!redefine.annotations = !{!0}

!0 = metadata !{metadata !"HyperOp", i32 (i8*)* @test, i32 0}
