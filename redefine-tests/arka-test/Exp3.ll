; ModuleID = 'arka-test/Exp3.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

@a = common global i32 0, align 4
@b = common global i32 0, align 4
@c = common global i32 0, align 4

; Function Attrs: nounwind
define void @swap(i32 %a, i32 %b, i32* %c) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %c.addr = alloca i32*, align 4
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  store i32* %c, i32** %c.addr, align 4
  %0 = load i32* %a.addr, align 4
  %1 = load i32* %b.addr, align 4
  %add = add nsw i32 %0, %1
  %2 = load i32** %c.addr, align 4
  store i32 %add, i32* %2, align 4
  ret void
}

; Function Attrs: nounwind
define void @redefine_start() #0 {
entry:
  store i32 10, i32* @a, align 4
  store i32 19, i32* @b, align 4
  %0 = load i32* @a, align 4
  %1 = load i32* @b, align 4
  call void @swap(i32 %0, i32 %1, i32* @c)
  ret void
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
