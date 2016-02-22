; ModuleID = 'Examples/ssa3.ll'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

define i32 @main() nounwind {
entry:
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %a.0 = phi i32 [ 10, %entry ], [ %a.1, %if.end ]
  %c.0 = phi i32 [ undef, %entry ], [ %a.1, %if.end ]
  %cmp = icmp sgt i32 %c.0, 0
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %cmp1 = icmp eq i32 20, 20
  br i1 %cmp1, label %if.then, label %if.end

if.then:                                          ; preds = %while.body
  br label %if.end

if.end:                                           ; preds = %if.then, %while.body
  %a.1 = phi i32 [ 30, %if.then ], [ %a.0, %while.body ]
  br label %while.cond

while.end:                                        ; preds = %while.cond
  ret i32 1
}

