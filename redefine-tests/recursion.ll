; ModuleID = '/home/kavitha/Desktop/REDEFINE_LLVM/MM_Compiler/redefine-tests/recursion.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

@redefine_out_val = common global i32 0, align 4

; Function Attrs: nounwind
define i32 @factorial(i32 %x) #0 {
entry:
  %retval = alloca i32, align 4
  %x.addr = alloca i32, align 4
  store i32 %x, i32* %x.addr, align 4
  %0 = load i32* %x.addr, align 4
  %cmp = icmp sgt i32 %0, 1
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %1 = load i32* %x.addr, align 4
  %2 = load i32* %x.addr, align 4
  %sub = sub nsw i32 %2, 1
  %call = call i32 @factorial(i32 %sub)
  %mul = mul nsw i32 %1, %call
  store i32 %mul, i32* %retval
  br label %return

if.end:                                           ; preds = %entry
  store i32 1, i32* %retval
  br label %return

return:                                           ; preds = %if.end, %if.then
  %3 = load i32* %retval
  ret i32 %3
}

; Function Attrs: nounwind
define void @redefine_start() #0 {
entry:
  %call = call i32 @factorial(i32 20)
  store i32 %call, i32* @redefine_out_val, align 4
  ret void
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
