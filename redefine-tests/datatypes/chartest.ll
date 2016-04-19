; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/datatypes/chartest.ll'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
define i32 @entry1(i32 inreg, i32*, i32*, i32*) {
entry0:
  %a = alloca i32, align 4
  store i32 %0, i32* %a, align 4
  %4 = load i32* %2, align 4
  %5 = load i32* %3, align 4
  %6 = add nsw i32 %4, %5
  br label %entry1return
  ret i32 %2
}

!redefine.annotations = !{!0}

!0 = metadata !{metadata !"HyperOp", i32 (i32 , i32 )* @entry1}
