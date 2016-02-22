; ModuleID = 'Examples/ssa4.ll'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

define i32 @main() nounwind {
entry:
  br label %while.body

while.body:                                       ; preds = %if.end, %entry
  %c.0 = phi i32 [ 10, %entry ], [ %c.1, %if.end ]
  %cmp = icmp sgt i32 %c.0, 100
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %while.body
  %mul = mul nsw i32 %c.0, 3
  br label %if.end

if.else:                                          ; preds = %while.body
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %c.1 = phi i32 [ %mul, %if.then ], [ 30, %if.else ]
  br label %while.body

return:                                           ; No predecessors!
  ret i32 0
}
