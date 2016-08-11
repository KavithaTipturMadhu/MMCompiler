; ModuleID = 'arka-test/Exp2.c'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

; Function Attrs: nounwind
define i32 @sumRed(i32* %a, i32 %N) #0 {
entry:
  %a.addr = alloca i32*, align 4
  %N.addr = alloca i32, align 4
  %i = alloca i32, align 4
  %sum = alloca i32, align 4
  store i32* %a, i32** %a.addr, align 4
  store i32 %N, i32* %N.addr, align 4
  store i32 0, i32* %sum, align 4
  %0 = load i32** %a.addr, align 4
  %arrayidx = getelementptr inbounds i32* %0, i32 0
  %1 = load i32* %arrayidx, align 4
  %2 = load i32* %N.addr, align 4
  %sub = sub nsw i32 %2, 1
  %3 = load i32** %a.addr, align 4
  %arrayidx1 = getelementptr inbounds i32* %3, i32 %sub
  %4 = load i32* %arrayidx1, align 4
  %add = add nsw i32 %1, %4
  store i32 %add, i32* %sum, align 4
  %5 = load i32* %sum, align 4
  ret i32 %5
}

; Function Attrs: nounwind
define void @redefine_start() #0 {
entry:
  %x = alloca [4 x i32], align 4
  %N = alloca i32, align 4
  %sum = alloca i32, align 4
  %0 = bitcast [4 x i32]* %x to i8*
  call void @llvm.memset.p0i8.i32(i8* %0, i8 0, i32 16, i32 4, i1 false)
  %1 = bitcast i8* %0 to [4 x i32]*
  %2 = getelementptr [4 x i32]* %1, i32 0, i32 1
  store i32 1, i32* %2
  %3 = getelementptr [4 x i32]* %1, i32 0, i32 2
  store i32 2, i32* %3
  %4 = getelementptr [4 x i32]* %1, i32 0, i32 3
  store i32 4, i32* %4
  store i32 4, i32* %N, align 4
  %arraydecay = getelementptr inbounds [4 x i32]* %x, i32 0, i32 0
  %5 = load i32* %N, align 4
  %call = call i32 @sumRed(i32* %arraydecay, i32 %5)
  store i32 %call, i32* %sum, align 4
  ret void
}

; Function Attrs: nounwind
declare void @llvm.memset.p0i8.i32(i8* nocapture, i8, i32, i32, i1) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }
