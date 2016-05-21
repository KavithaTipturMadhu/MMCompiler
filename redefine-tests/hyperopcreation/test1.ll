; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/redefine-tests/test1/test1.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind uwtable
define void @redefine_start(i32 inreg %a, i32 inreg %b, i32 inreg %c, i32 inreg %d, i32 inreg %e) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %c.addr = alloca i32, align 4
  %d.addr = alloca i32, align 4
  %e.addr = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %z = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  store i32 %c, i32* %c.addr, align 4
  store i32 %d, i32* %d.addr, align 4
  store i32 %e, i32* %e.addr, align 4
  %0 = load i32* %a.addr, align 4
  %1 = load i32* %b.addr, align 4
  %add = add nsw i32 %0, %1
  store i32 %add, i32* %x, align 4
  %2 = load i32* %a.addr, align 4
  %3 = load i32* %c.addr, align 4
  %add1 = add nsw i32 %2, %3
  %4 = load i32* %d.addr, align 4
  %add2 = add nsw i32 %add1, %4
  store i32 %add2, i32* %y, align 4
  %5 = load i32* %e.addr, align 4
  %6 = load i32* %x, align 4
  %mul = mul nsw i32 %5, %6
  %7 = load i32* %y, align 4
  %add3 = add nsw i32 %mul, %7
  store i32 %add3, i32* %z, align 4
  ret void
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }


