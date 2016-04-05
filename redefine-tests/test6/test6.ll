; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/test6/test6.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

%struct.s = type { i32, i8* }

@.str = private unnamed_addr constant [8 x i8] c"preetam\00", align 1

; Function Attrs: nounwind
define i32 @test() #0 {
entry:
  %retval = alloca i32, align 4
  %s1 = alloca %struct.s, align 4
  %s2 = alloca %struct.s, align 4
  %a = getelementptr inbounds %struct.s* %s1, i32 0, i32 0
  store i32 12, i32* %a, align 4
  %b = getelementptr inbounds %struct.s* %s1, i32 0, i32 1
  store i8* getelementptr inbounds ([8 x i8]* @.str, i32 0, i32 0), i8** %b, align 4
  %a1 = getelementptr inbounds %struct.s* %s1, i32 0, i32 0
  %0 = load i32* %a1, align 4
  %inc = add nsw i32 %0, 1
  store i32 %inc, i32* %a1, align 4
  %a2 = getelementptr inbounds %struct.s* %s2, i32 0, i32 0
  store i32 %0, i32* %a2, align 4
  %b3 = getelementptr inbounds %struct.s* %s2, i32 0, i32 1
  store i8* getelementptr inbounds ([8 x i8]* @.str, i32 0, i32 0), i8** %b3, align 4
  %a4 = getelementptr inbounds %struct.s* %s1, i32 0, i32 0
  %1 = load i32* %a4, align 4
  %a5 = getelementptr inbounds %struct.s* %s2, i32 0, i32 0
  %2 = load i32* %a5, align 4
  %cmp = icmp eq i32 %1, %2
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %a6 = getelementptr inbounds %struct.s* %s1, i32 0, i32 0
  %3 = load i32* %a6, align 4
  store i32 %3, i32* %retval
  br label %return

if.else:                                          ; preds = %entry
  %a7 = getelementptr inbounds %struct.s* %s2, i32 0, i32 0
  %4 = load i32* %a7, align 4
  store i32 %4, i32* %retval
  br label %return

return:                                           ; preds = %if.else, %if.then
  %5 = load i32* %retval
  ret i32 %5
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }


!redefine.annotations = !{!0}

!0 = metadata !{metadata !"HyperOp", i32 ()* @test, i32 0}
