; ModuleID = '/home/kavitha/Desktop/test.c'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

define i32 @main() nounwind {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %x = alloca i32, align 4
  store i32 0, i32* %retval
  store i32 1, i32* %a, align 4
  store i32 0, i32* %b, align 4
  store i32 1, i32* %c, align 4
  %0 = load i32* %b, align 4
  %1 = load i32* %c, align 4
  %add = add nsw i32 %0, %1
  store i32 %add, i32* %a, align 4
  %2 = load i32* %a, align 4
  store i32 %2, i32* %c, align 4
  %3 = load i32* %x, align 4
  store i32 %3, i32* %a, align 4
  %4 = load i32* %c, align 4
  ret i32 %4
}
