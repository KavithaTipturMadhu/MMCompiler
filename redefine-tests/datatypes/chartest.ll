; ModuleID = '/home/kavitha/Desktop/REDEFINE_MM_REPO/MM_Compiler/redefine-tests/datatypes/chartest.ll'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
define void @entry1(i32 inreg, i32*, i32*, i32*) {
entry:
  store i32 %0, i32* %1, align 4
  %4 = load i32* %2, align 4
  %5 = load i32* %3, align 4
  %6 = add nsw i32 %4, %5
  store i32 %6, i32* %2, align 4
  ret void
}

!redefine.annotations = !{!0}

!0 = metadata !{metadata !"HyperOp", void (i32 , i32*, i32*, i32*)* @entry1}
