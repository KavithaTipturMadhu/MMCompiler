; ModuleID = '/home/kavitha/Desktop/tests/Fib/fib.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
define i32 @fib(i32 %n) #0 {
entry:
  %retval = alloca i32, align 4
  %n.addr = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  %0 = load i32* %n.addr, align 4
  %cmp = icmp ugt i32 %0, 1
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %1 = load i32* %n.addr, align 4
  %sub = sub i32 %1, 1
  %call = call i32 @fib(i32 %sub)
  %2 = load i32* %n.addr, align 4
  %sub1 = sub i32 %2, 2
  %call2 = call i32 @fib(i32 %sub1)
  %add = add i32 %call, %call2
  store i32 %add, i32* %retval
  br label %return

if.end:                                           ; preds = %entry
  %3 = load i32* %n.addr, align 4
  store i32 %3, i32* %retval
  br label %return

return:                                           ; preds = %if.end, %if.then
  %4 = load i32* %retval
  ret i32 %4
}

; Function Attrs: nounwind
define void @main() #0 {
entry:
  %n = alloca i32, align 4
  %call = call i32 @fib(i32 20)
  store i32 %call, i32* %n, align 4
  ret void
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }


!redefine.annotations = !{!0}

!0 = metadata !{metadata !"HyperOp", void ()* @main, i32 0}

