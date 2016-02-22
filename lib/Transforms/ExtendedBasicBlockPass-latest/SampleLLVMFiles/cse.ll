; ModuleID = '/home/kavitha/Desktop/cdtestcases/cse.c'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

define i32 @main() nounwind {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  store i32 0, i32* %retval
  %0 = load i32* %b, align 4
  %1 = load i32* %j, align 4
  %add = add nsw i32 %0, %1
  store i32 %add, i32* %a, align 4
  %2 = load i32* %j, align 4
  %3 = load i32* %b, align 4
  %add1 = add nsw i32 %2, %3
  store i32 %add1, i32* %d, align 4
  store i32 10, i32* %i, align 4
  %4 = load i32* %b, align 4
  %5 = load i32* %i, align 4
  %add2 = add nsw i32 %4, %5
  store i32 %add2, i32* %a, align 4
  %6 = load i32* %b, align 4
  %7 = load i32* %j, align 4
  %add3 = add nsw i32 %6, %7
  store i32 %add3, i32* %d, align 4
  %8 = load i32* %b, align 4
  %9 = load i32* %c, align 4
  %add4 = add nsw i32 %8, %9
  store i32 %add4, i32* %a, align 4
  ret i32 0
}
