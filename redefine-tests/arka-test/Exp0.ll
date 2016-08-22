; ModuleID = 'arka-test/Exp0.c'
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
define i32 @fmaX(i32 %a, i32 %b, i32 %c) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %c.addr = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  store i32 %c, i32* %c.addr, align 4
  %0 = load i32* %a.addr, align 4
  %1 = load i32* %b.addr, align 4
  %call = call i32 @mul(i32 %0, i32 %1)
  ret i32 %call
}

; Function Attrs: nounwind
define void @redefine_start() #0 {
entry:
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %w = alloca i32, align 4
  %z = alloca i32, align 4
  store i32 10, i32* %x, align 4
  store i32 11, i32* %y, align 4
  store i32 10, i32* %w, align 4
  store i32 20, i32* %z, align 4
  %0 = load i32* %x, align 4
  %1 = load i32* %y, align 4
  %call = call i32 @mul(i32 %0, i32 %1)
  store i32 %call, i32* %z, align 4
  %2 = load i32* %z, align 4
  %inc = add nsw i32 %2, 1
  store i32 %inc, i32* %z, align 4
  store i32 %2, i32* %w, align 4
  %3 = load i32* %y, align 4
  %4 = load i32* %w, align 4
  %5 = load i32* %z, align 4
  %call1 = call i32 @fmaX(i32 %3, i32 %4, i32 %5)
  store i32 %call1, i32* %x, align 4
  ret void
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
