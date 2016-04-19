; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/datatypes/structtest.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

%struct.a = type { i32, i32 }

; Function Attrs: nounwind
define void @main() #0 {
entry:
  %x = alloca %struct.a, align 4
  %b = getelementptr inbounds %struct.a* %x, i32 0, i32 0
  store i32 10, i32* %b, align 4
  %c = getelementptr inbounds %struct.a* %x, i32 0, i32 1
  store i32 20, i32* %c, align 4
  ret void
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }


!redefine.annotations = !{!0}

!0 = metadata !{metadata !"HyperOp", void ()* @main, i32 0}
