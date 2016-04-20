; ModuleID = '/home/kavitha/Desktop/tests/Fib/fib.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
define void @fib0(i32 inreg) #1 {
entry:
  %cmp = icmp ugt i32 %0, 1
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %sub = sub i32 %0, 1
  %1 = alloca i32, align 4, !ConsumedBy !10
  store i32 %sub, i32* %1, align 4
  %sub1 = sub i32 %0, 2
  %2 = alloca i32, align 4, !ConsumedBy !12
  store i32 %sub1, i32* %2, align 4
  br label %return

if.end:                                           ; preds = %entry
  %retval = alloca i32, align 4, !ConsumedBy !14
  store i32 %0, i32* %retval
  br label %return

return:						 ; preds = %entry, %if.end
  ret void
}

define void @fib1(i32 inreg, i32 inreg) #2 {
fib1:
  %2 =  i32, align 4, !ConsumedBy !16
  %add = add i32 %0, %1
  store i32 %add, i32* %2
  ret void
}

; Function Attrs: nounwind
define void @main() #0 {
entry:
  %n = alloca i32, align 4, !ConsumedBy !6
  %m = alloca i32, align 4, !ConsumedBy !8
  store i32 20, i32* %n, align 4
  ret void
}

; Function Attrs: nounwind
define void @main0(i32 inreg, i32*) #0 {
entry:
  store i32 0, i32* %1, align 4
  ret void
}


attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }


!redefine.annotations = !{!0, !1, !2, !3, !4, !5}

!0 = metadata !{metadata !"HyperOp", void ()* @main}
!1 = metadata !{metadata !"HyperOp", void (i32, i32*)* @main0}
!2 = metadata !{metadata !"HyperOp", void (i32 )* @fib0}
!3 = metadata !{metadata !"HyperOp", void (i32, i32 )* @fib1}
!4 = metadata !{metadata !"CreatesInstance", !1, !1}
!5 = metadata !{metadata !"CreatesInstance", !1, !1}
!6 = metadata !{metadata !7}
!7 = metadata !{metadata !2, metadata !"Scalar", metadata !"1"}
!8 = metadata !{metadata !9}
!9 = metadata !{metadata !1, metadata !"Reference", metadata !"0"}
!10 = metadata !{metadata !11}
!11 = metadata !{metadata !3, metadata !"Scalar", metadata !"0", !4}
!12 = metadata !{metadata !13}
!13 = metadata !{metadata !3, metadata !"Scalar", metadata !"0", !5}
!14 = metadata !{metadata !15}
!15 = metadata !{metadata !1, metadata !"Scalar", metadata !"0"}
!16 = metadata !{metadata !17}
!17 = metadata !{metadata !1, metadata !"Scalar", metadata !"0"}
