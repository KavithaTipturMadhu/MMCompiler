; ModuleID = 'Examples/ssa2.ll'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

define i32 @main() nounwind {
entry:
  br label %while.body

while.body:                                       ; preds = %if.end5, %entry
  %b.0 = phi i32 [ 1, %entry ], [ %b.1, %if.end5 ]
  %c.0 = phi i32 [ 1, %entry ], [ %c.1, %if.end5 ]
  %cmp = icmp slt i32 %c.0, 100
  br i1 %cmp, label %if.then, label %if.else4

if.then:                                          ; preds = %while.body
  %cmp1 = icmp slt i32 %b.0, 20
  br i1 %cmp1, label %if.then2, label %if.else

if.then2:                                         ; preds = %if.then
  %add = add nsw i32 %c.0, 1
  br label %if.end

if.else:                                          ; preds = %if.then
  %add3 = add nsw i32 %c.0, 1
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then2
  %b.1 = phi i32 [ 1, %if.then2 ], [ %c.0, %if.else ]
  %c.1 = phi i32 [ %add, %if.then2 ], [ %add3, %if.else ]
  br label %if.end5

if.else4:                                         ; preds = %while.body
  ret i32 1

if.end5:                                          ; preds = %if.end
  br label %while.body
}

