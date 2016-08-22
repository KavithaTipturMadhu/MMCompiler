; ModuleID = 'arka-test/Exp1.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
define i32 @mul(i32 %a, i32 %b) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  %0 = load i32* %a.addr, align 4
  %1 = load i32* %b.addr, align 4
  %mul = mul nsw i32 %0, %1
  ret i32 %mul
}

; Function Attrs: nounwind
define void @redefine_start() #0 {
entry:
  %op = alloca i8, align 1
  %c = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i8 43, i8* %op, align 1
  store i32 0, i32* %c, align 4
  store i32 1, i32* %a, align 4
  store i32 2, i32* %b, align 4
  %0 = load i8* %op, align 1
  %conv = sext i8 %0 to i32
  %cmp = icmp eq i32 %conv, 42
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %1 = load i32* %a, align 4
  %2 = load i32* %b, align 4
  %call = call i32 @mul(i32 %1, i32 %2)
  store i32 %call, i32* %c, align 4
  br label %if.end

if.else:                                          ; preds = %entry
  %3 = load i32* %a, align 4
  %4 = load i32* %b, align 4
  %add = add nsw i32 %3, %4
  store i32 %add, i32* %c, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
