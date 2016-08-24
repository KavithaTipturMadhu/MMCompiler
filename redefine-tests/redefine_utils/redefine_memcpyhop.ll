; ModuleID = 'redefine_utils/redefine_memcpy.ll'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
define i8* @redefine_memcpy(i8* %dest, i8* %src, i32 %count) #0 {
entry:
  %dest.addr = alloca i8*, align 4
  %src.addr = alloca i8*, align 4
  %count.addr = alloca i32, align 4
  %tmp = alloca i8*, align 4
  %s = alloca i8*, align 4
  store i8* %dest, i8** %dest.addr, align 4
  store i8* %src, i8** %src.addr, align 4
  store i32 %count, i32* %count.addr, align 4
  %0 = load i8** %dest.addr, align 4
  store i8* %0, i8** %tmp, align 4
  %1 = load i8** %src.addr, align 4
  store i8* %1, i8** %s, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %2 = load i32* %count.addr, align 4
  %dec = add i32 %2, -1
  store i32 %dec, i32* %count.addr, align 4
  %tobool = icmp ne i32 %2, 0
  br i1 %tobool, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %3 = load i8** %s, align 4
  %incdec.ptr = getelementptr inbounds i8* %3, i32 1
  store i8* %incdec.ptr, i8** %s, align 4
  %4 = load i8* %3, align 1
  %5 = load i8** %tmp, align 4
  %incdec.ptr2 = getelementptr inbounds i8* %5, i32 1
  store i8* %incdec.ptr2, i8** %tmp, align 4
  store i8 %4, i8* %5, align 1
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %6 = load i8** %dest.addr, align 4
  ret i8* %6
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }

!redefine.annotations = !{}
