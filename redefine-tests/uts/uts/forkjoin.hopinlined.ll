; ModuleID = '/Users/kavithamadhu/Compiler/REDEFINETests/forkjoin.hop.ll'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

%struct.node_t = type { i32, i32, i32, %struct.state_t }
%struct.state_t = type { [20 x i8] }

@rootId = global i32 0, align 4
@nonLeafBF = global i32 4, align 4
@gen_mx = global i32 6, align 4
@computeGranularity = global i32 1, align 4
@debug = global i32 0, align 4
@verbose = global i32 1, align 4
@shiftDepth = global float 5.000000e-01, align 4
@nonLeafProb = global float 2.343750e-01, align 4
@b_0 = global float 4.000000e+00, align 4
@shape_fn = global i32 0, align 4
@type = global i32 1, align 4

; Function Attrs: alwaysinline
declare void @uts_initRoot(%struct.node_t*, i32) #0

; Function Attrs: alwaysinline
declare void @rng_spawn(i8*, i8*, i32) #0

; Function Attrs: alwaysinline
declare i32 @uts_numChildren(%struct.node_t* nocapture) #0

; Function Attrs: alwaysinline
declare i32 @uts_childType(%struct.node_t* nocapture) #0

define void @redefine_start1() {
redefine_start1.entry:
  %0 = alloca %struct.node_t, align 4, !ConsumedBy !26
  store i32 1, i32* @type, align 4
  store float 4.000000e+00, float* @b_0, align 4
  store i32 34, i32* @rootId, align 4
  store i32 20, i32* @gen_mx, align 4
  store i32 0, i32* @shape_fn, align 4
  store i32 1, i32* @computeGranularity, align 4
  store i32 4, i32* @nonLeafBF, align 4
  store float 5.000000e-01, float* @shiftDepth, align 4
  store float 2.343750e-01, float* @nonLeafProb, align 4
  %1 = load i32* @type, align 4
  call void @uts_initRoot(%struct.node_t* %0, i32 %1)
  br label %redefine_start1.return

redefine_start1.return:                           ; preds = %redefine_start1.entry
  ret void
}

define void @uts_function2(i32) {
uts_function2.entry:
  br label %uts_function2.return1

uts_function2.return1:                            ; preds = %uts_function2.entry
  %1 = phi i32 [ %0, %uts_function2.entry ]
  br label %uts_function2.return

uts_function2.return:                             ; preds = %uts_function2.return1
  ret void
}

define void @uts_function4() {
uts_function4.if.else:
  %0 = alloca i32, align 4
  store i32 1, i32* %0, align 4
  %1 = load i32* %0
  %2 = alloca i32, align 4, !ConsumedBy !28
  store i32 %1, i32* %2, align 4
  br label %uts_function4.return

uts_function4.return:                             ; preds = %uts_function4.if.else
  ret void
}

define void @uts_function5(i32) {
uts_function5.entry:
  br label %uts_function5.for.cond

uts_function5.for.cond:                           ; preds = %uts_function5.entry
  %1 = phi i32 [ 0, %uts_function5.entry ]
  %2 = alloca i1, align 4
  %3 = alloca i1, align 4, !Controls !30
  %4 = icmp slt i32 %1, %0
  %5 = alloca i32, align 4, !ConsumedBy !32
  store i32 %1, i32* %5, align 4
  store i1 %4, i1* %3, align 4
  br i1 %4, label %uts_function5.return, label %uts_function5.return

uts_function5.return:                             ; preds = %uts_function5.for.cond, %uts_function5.for.cond
  store i1 true, i1* %2, align 4
  ret void
}

define void @uts_function6(%struct.node_t*) {
uts_function6.entry:
  %1 = alloca i1, align 4, !Controls !34
  %2 = alloca %struct.node_t, !ConsumedBy !37
  %type.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 0
  %3 = load i32* %type.i, align 4
  %type1.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 0
  store i32 %3, i32* %type1.i, align 4
  %height.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 1
  %4 = load i32* %height.i, align 4
  %height2.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 1
  store i32 %4, i32* %height2.i, align 4
  %numChildren.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 2
  %5 = load i32* %numChildren.i, align 4
  %numChildren3.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 2
  store i32 %5, i32* %numChildren3.i, align 4
  %arrayidx.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 0
  %6 = load i8* %arrayidx.i, align 1
  %arrayidx8.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 0
  store i8 %6, i8* %arrayidx8.i, align 1
  %arrayidx.1.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 1
  %7 = load i8* %arrayidx.1.i, align 1
  %arrayidx8.1.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 1
  store i8 %7, i8* %arrayidx8.1.i, align 1
  %arrayidx.2.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 2
  %8 = load i8* %arrayidx.2.i, align 1
  %arrayidx8.2.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 2
  store i8 %8, i8* %arrayidx8.2.i, align 1
  %arrayidx.3.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 3
  %9 = load i8* %arrayidx.3.i, align 1
  %arrayidx8.3.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 3
  store i8 %9, i8* %arrayidx8.3.i, align 1
  %arrayidx.4.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 4
  %10 = load i8* %arrayidx.4.i, align 1
  %arrayidx8.4.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 4
  store i8 %10, i8* %arrayidx8.4.i, align 1
  %arrayidx.5.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 5
  %11 = load i8* %arrayidx.5.i, align 1
  %arrayidx8.5.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 5
  store i8 %11, i8* %arrayidx8.5.i, align 1
  %arrayidx.6.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 6
  %12 = load i8* %arrayidx.6.i, align 1
  %arrayidx8.6.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 6
  store i8 %12, i8* %arrayidx8.6.i, align 1
  %arrayidx.7.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 7
  %13 = load i8* %arrayidx.7.i, align 1
  %arrayidx8.7.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 7
  store i8 %13, i8* %arrayidx8.7.i, align 1
  %arrayidx.8.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 8
  %14 = load i8* %arrayidx.8.i, align 1
  %arrayidx8.8.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 8
  store i8 %14, i8* %arrayidx8.8.i, align 1
  %arrayidx.9.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 9
  %15 = load i8* %arrayidx.9.i, align 1
  %arrayidx8.9.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 9
  store i8 %15, i8* %arrayidx8.9.i, align 1
  %arrayidx.10.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 10
  %16 = load i8* %arrayidx.10.i, align 1
  %arrayidx8.10.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 10
  store i8 %16, i8* %arrayidx8.10.i, align 1
  %arrayidx.11.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 11
  %17 = load i8* %arrayidx.11.i, align 1
  %arrayidx8.11.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 11
  store i8 %17, i8* %arrayidx8.11.i, align 1
  %arrayidx.12.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 12
  %18 = load i8* %arrayidx.12.i, align 1
  %arrayidx8.12.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 12
  store i8 %18, i8* %arrayidx8.12.i, align 1
  %arrayidx.13.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 13
  %19 = load i8* %arrayidx.13.i, align 1
  %arrayidx8.13.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 13
  store i8 %19, i8* %arrayidx8.13.i, align 1
  %arrayidx.14.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 14
  %20 = load i8* %arrayidx.14.i, align 1
  %arrayidx8.14.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 14
  store i8 %20, i8* %arrayidx8.14.i, align 1
  %arrayidx.15.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 15
  %21 = load i8* %arrayidx.15.i, align 1
  %arrayidx8.15.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 15
  store i8 %21, i8* %arrayidx8.15.i, align 1
  %arrayidx.16.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 16
  %22 = load i8* %arrayidx.16.i, align 1
  %arrayidx8.16.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 16
  store i8 %22, i8* %arrayidx8.16.i, align 1
  %arrayidx.17.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 17
  %23 = load i8* %arrayidx.17.i, align 1
  %arrayidx8.17.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 17
  store i8 %23, i8* %arrayidx8.17.i, align 1
  %arrayidx.18.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 18
  %24 = load i8* %arrayidx.18.i, align 1
  %arrayidx8.18.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 18
  store i8 %24, i8* %arrayidx8.18.i, align 1
  %arrayidx.19.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 19
  %25 = load i8* %arrayidx.19.i, align 1
  %arrayidx8.19.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 19
  store i8 %25, i8* %arrayidx8.19.i, align 1
  %26 = alloca %struct.node_t, align 4, !ConsumedBy !39
  %27 = alloca i32, align 4
  %call = call i32 @uts_numChildren(%struct.node_t* %0)
  store i32 %call, i32* %27
  %28 = load i32* %27, align 4
  %29 = icmp sgt i32 %28, 0
  %upperBound = alloca i32, !ConsumedBy !41
  store i32 0, i32* %upperBound
  %upperBound1 = alloca i32
  store i32 0, i32* %upperBound1
  %30 = alloca i32, align 4, !ConsumedBy !43
  %31 = alloca i32, align 4, !ConsumedBy !45
  store i1 %29, i1* %1, align 4
  br i1 %29, label %uts_function6.if.then, label %uts_function6.return

uts_function6.if.then:                            ; preds = %uts_function6.entry
  %32 = alloca i32, align 4
  %call2 = call i32 @uts_childType(%struct.node_t* %0)
  store i32 %call2, i32* %32
  %33 = load i32* %32, align 4
  %34 = add nsw i32 %28, 0
  store i32 %34, i32* %31, align 4
  store i32 %34, i32* %upperBound1
  store i32 %34, i32* %upperBound
  %35 = add nsw i32 %33, 0
  store i32 %35, i32* %30, align 4
  br label %uts_function6.return

uts_function6.return:                             ; preds = %uts_function6.if.then, %uts_function6.entry
  ret void
}

; Function Attrs: alwaysinline
define void @uts_function_for.body7(i32* %arr, i32 %size) #0 {
entry:
  %arr.addr = alloca i32*, align 8
  %size.addr = alloca i32, align 4
  %i = alloca i32, align 4
  %sum = alloca i32, align 4, !ConsumedBy !28
  store i32* %arr, i32** %arr.addr, align 8
  store i32 %size, i32* %size.addr, align 4
  store i32 0, i32* %sum, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32* %i, align 4
  %1 = load i32* %size.addr, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %2 = load i32* %i, align 4
  %idxprom = sext i32 %2 to i64
  %3 = load i32** %arr.addr, align 8
  %arrayidx = getelementptr inbounds i32* %3, i64 %idxprom
  %4 = load i32* %arrayidx, align 4
  %5 = load i32* %sum, align 4
  %add = add nsw i32 %5, %4
  store i32 %add, i32* %sum, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %6 = load i32* %i, align 4
  %inc = add nsw i32 %6, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret void
}

define void @uts_function_for.body8(i32, i32, %struct.node_t*, %struct.node_t*) {
uts_function_for.body8.newFuncRoot:
  %4 = alloca %struct.node_t, !ConsumedBy !47
  br label %uts_function_for.body8.for.body

uts_function_for.body8.for.body:                  ; preds = %uts_function_for.body8.newFuncRoot
  %5 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 0
  store i32 %0, i32* %5, align 4
  %6 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 1
  %7 = load i32* %6, align 4
  %8 = add nsw i32 %7, 1
  %9 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 1
  store i32 %8, i32* %9, align 4
  %10 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 2
  store i32 -1, i32* %10, align 4
  %11 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3
  %12 = getelementptr inbounds %struct.state_t* %11, i32 0, i32 0
  %13 = getelementptr inbounds [20 x i8]* %12, i32 0, i32 0
  %14 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3
  %15 = getelementptr inbounds %struct.state_t* %14, i32 0, i32 0
  %16 = getelementptr inbounds [20 x i8]* %15, i32 0, i32 0
  call void @rng_spawn(i8* %13, i8* %16, i32 %1)
  br label %uts_function_for.body8.return

uts_function_for.body8.return:                    ; preds = %uts_function_for.body8.for.body
  %17 = alloca i32, !ConsumedBy !49
  %type.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 0
  %18 = load i32* %type.i, align 4
  %type1.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 0
  store i32 %18, i32* %type1.i, align 4
  %height.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 1
  %19 = load i32* %height.i, align 4
  %height2.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 1
  store i32 %19, i32* %height2.i, align 4
  %numChildren.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 2
  %20 = load i32* %numChildren.i, align 4
  %numChildren3.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 2
  store i32 %20, i32* %numChildren3.i, align 4
  %arrayidx.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 0
  %21 = load i8* %arrayidx.i, align 1
  %arrayidx8.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 0
  store i8 %21, i8* %arrayidx8.i, align 1
  %arrayidx.1.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 1
  %22 = load i8* %arrayidx.1.i, align 1
  %arrayidx8.1.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 1
  store i8 %22, i8* %arrayidx8.1.i, align 1
  %arrayidx.2.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 2
  %23 = load i8* %arrayidx.2.i, align 1
  %arrayidx8.2.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 2
  store i8 %23, i8* %arrayidx8.2.i, align 1
  %arrayidx.3.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 3
  %24 = load i8* %arrayidx.3.i, align 1
  %arrayidx8.3.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 3
  store i8 %24, i8* %arrayidx8.3.i, align 1
  %arrayidx.4.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 4
  %25 = load i8* %arrayidx.4.i, align 1
  %arrayidx8.4.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 4
  store i8 %25, i8* %arrayidx8.4.i, align 1
  %arrayidx.5.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 5
  %26 = load i8* %arrayidx.5.i, align 1
  %arrayidx8.5.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 5
  store i8 %26, i8* %arrayidx8.5.i, align 1
  %arrayidx.6.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 6
  %27 = load i8* %arrayidx.6.i, align 1
  %arrayidx8.6.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 6
  store i8 %27, i8* %arrayidx8.6.i, align 1
  %arrayidx.7.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 7
  %28 = load i8* %arrayidx.7.i, align 1
  %arrayidx8.7.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 7
  store i8 %28, i8* %arrayidx8.7.i, align 1
  %arrayidx.8.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 8
  %29 = load i8* %arrayidx.8.i, align 1
  %arrayidx8.8.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 8
  store i8 %29, i8* %arrayidx8.8.i, align 1
  %arrayidx.9.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 9
  %30 = load i8* %arrayidx.9.i, align 1
  %arrayidx8.9.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 9
  store i8 %30, i8* %arrayidx8.9.i, align 1
  %arrayidx.10.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 10
  %31 = load i8* %arrayidx.10.i, align 1
  %arrayidx8.10.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 10
  store i8 %31, i8* %arrayidx8.10.i, align 1
  %arrayidx.11.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 11
  %32 = load i8* %arrayidx.11.i, align 1
  %arrayidx8.11.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 11
  store i8 %32, i8* %arrayidx8.11.i, align 1
  %arrayidx.12.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 12
  %33 = load i8* %arrayidx.12.i, align 1
  %arrayidx8.12.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 12
  store i8 %33, i8* %arrayidx8.12.i, align 1
  %arrayidx.13.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 13
  %34 = load i8* %arrayidx.13.i, align 1
  %arrayidx8.13.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 13
  store i8 %34, i8* %arrayidx8.13.i, align 1
  %arrayidx.14.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 14
  %35 = load i8* %arrayidx.14.i, align 1
  %arrayidx8.14.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 14
  store i8 %35, i8* %arrayidx8.14.i, align 1
  %arrayidx.15.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 15
  %36 = load i8* %arrayidx.15.i, align 1
  %arrayidx8.15.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 15
  store i8 %36, i8* %arrayidx8.15.i, align 1
  %arrayidx.16.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 16
  %37 = load i8* %arrayidx.16.i, align 1
  %arrayidx8.16.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 16
  store i8 %37, i8* %arrayidx8.16.i, align 1
  %arrayidx.17.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 17
  %38 = load i8* %arrayidx.17.i, align 1
  %arrayidx8.17.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 17
  store i8 %38, i8* %arrayidx8.17.i, align 1
  %arrayidx.18.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 18
  %39 = load i8* %arrayidx.18.i, align 1
  %arrayidx8.18.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 18
  store i8 %39, i8* %arrayidx8.18.i, align 1
  %arrayidx.19.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 19
  %40 = load i8* %arrayidx.19.i, align 1
  %arrayidx8.19.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 19
  store i8 %40, i8* %arrayidx8.19.i, align 1
  ret void
}

define void @uts_function9(i32, i32 %base) {
uts_function9.entry:
  br label %uts_function9.return1

uts_function9.return1:                            ; preds = %uts_function9.entry
  %1 = phi i32 [ %0, %uts_function9.entry ]
  %2 = alloca i32, align 4, !ConsumedBy !51
  store i32 %1, i32* %2, align 4
  br label %uts_function9.return

uts_function9.return:                             ; preds = %uts_function9.return1
  ret void
}

define void @uts_function11() {
uts_function11.if.else:
  %0 = alloca i32, align 4
  store i32 1, i32* %0, align 4
  %1 = load i32* %0
  %2 = alloca i32, align 4, !ConsumedBy !53
  store i32 %1, i32* %2, align 4
  br label %uts_function11.return

uts_function11.return:                            ; preds = %uts_function11.if.else
  ret void
}

define void @uts_function12(i32) {
uts_function12.entry:
  br label %uts_function12.for.cond

uts_function12.for.cond:                          ; preds = %uts_function12.entry
  %1 = phi i32 [ 0, %uts_function12.entry ]
  %2 = alloca i1, align 4, !Controls !55
  %3 = alloca i1, align 4
  %4 = icmp slt i32 %1, %0
  %5 = alloca i32, align 4, !ConsumedBy !57
  store i32 %1, i32* %5, align 4
  store i1 %4, i1* %2, align 4
  br i1 %4, label %uts_function12.return, label %uts_function12.return

uts_function12.return:                            ; preds = %uts_function12.for.cond, %uts_function12.for.cond
  store i1 true, i1* %3, align 4
  ret void
}

define void @uts_function13(%struct.node_t*) {
uts_function13.entry:
  %1 = alloca i1, align 4, !Controls !59
  %2 = alloca %struct.node_t, !ConsumedBy !62
  %type.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 0
  %3 = load i32* %type.i, align 4
  %type1.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 0
  store i32 %3, i32* %type1.i, align 4
  %height.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 1
  %4 = load i32* %height.i, align 4
  %height2.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 1
  store i32 %4, i32* %height2.i, align 4
  %numChildren.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 2
  %5 = load i32* %numChildren.i, align 4
  %numChildren3.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 2
  store i32 %5, i32* %numChildren3.i, align 4
  %arrayidx.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 0
  %6 = load i8* %arrayidx.i, align 1
  %arrayidx8.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 0
  store i8 %6, i8* %arrayidx8.i, align 1
  %arrayidx.1.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 1
  %7 = load i8* %arrayidx.1.i, align 1
  %arrayidx8.1.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 1
  store i8 %7, i8* %arrayidx8.1.i, align 1
  %arrayidx.2.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 2
  %8 = load i8* %arrayidx.2.i, align 1
  %arrayidx8.2.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 2
  store i8 %8, i8* %arrayidx8.2.i, align 1
  %arrayidx.3.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 3
  %9 = load i8* %arrayidx.3.i, align 1
  %arrayidx8.3.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 3
  store i8 %9, i8* %arrayidx8.3.i, align 1
  %arrayidx.4.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 4
  %10 = load i8* %arrayidx.4.i, align 1
  %arrayidx8.4.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 4
  store i8 %10, i8* %arrayidx8.4.i, align 1
  %arrayidx.5.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 5
  %11 = load i8* %arrayidx.5.i, align 1
  %arrayidx8.5.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 5
  store i8 %11, i8* %arrayidx8.5.i, align 1
  %arrayidx.6.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 6
  %12 = load i8* %arrayidx.6.i, align 1
  %arrayidx8.6.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 6
  store i8 %12, i8* %arrayidx8.6.i, align 1
  %arrayidx.7.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 7
  %13 = load i8* %arrayidx.7.i, align 1
  %arrayidx8.7.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 7
  store i8 %13, i8* %arrayidx8.7.i, align 1
  %arrayidx.8.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 8
  %14 = load i8* %arrayidx.8.i, align 1
  %arrayidx8.8.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 8
  store i8 %14, i8* %arrayidx8.8.i, align 1
  %arrayidx.9.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 9
  %15 = load i8* %arrayidx.9.i, align 1
  %arrayidx8.9.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 9
  store i8 %15, i8* %arrayidx8.9.i, align 1
  %arrayidx.10.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 10
  %16 = load i8* %arrayidx.10.i, align 1
  %arrayidx8.10.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 10
  store i8 %16, i8* %arrayidx8.10.i, align 1
  %arrayidx.11.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 11
  %17 = load i8* %arrayidx.11.i, align 1
  %arrayidx8.11.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 11
  store i8 %17, i8* %arrayidx8.11.i, align 1
  %arrayidx.12.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 12
  %18 = load i8* %arrayidx.12.i, align 1
  %arrayidx8.12.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 12
  store i8 %18, i8* %arrayidx8.12.i, align 1
  %arrayidx.13.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 13
  %19 = load i8* %arrayidx.13.i, align 1
  %arrayidx8.13.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 13
  store i8 %19, i8* %arrayidx8.13.i, align 1
  %arrayidx.14.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 14
  %20 = load i8* %arrayidx.14.i, align 1
  %arrayidx8.14.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 14
  store i8 %20, i8* %arrayidx8.14.i, align 1
  %arrayidx.15.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 15
  %21 = load i8* %arrayidx.15.i, align 1
  %arrayidx8.15.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 15
  store i8 %21, i8* %arrayidx8.15.i, align 1
  %arrayidx.16.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 16
  %22 = load i8* %arrayidx.16.i, align 1
  %arrayidx8.16.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 16
  store i8 %22, i8* %arrayidx8.16.i, align 1
  %arrayidx.17.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 17
  %23 = load i8* %arrayidx.17.i, align 1
  %arrayidx8.17.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 17
  store i8 %23, i8* %arrayidx8.17.i, align 1
  %arrayidx.18.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 18
  %24 = load i8* %arrayidx.18.i, align 1
  %arrayidx8.18.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 18
  store i8 %24, i8* %arrayidx8.18.i, align 1
  %arrayidx.19.i = getelementptr inbounds %struct.node_t* %0, i32 0, i32 3, i32 0, i32 19
  %25 = load i8* %arrayidx.19.i, align 1
  %arrayidx8.19.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 19
  store i8 %25, i8* %arrayidx8.19.i, align 1
  %26 = alloca %struct.node_t, align 4, !ConsumedBy !64
  %27 = alloca i32, align 4
  %call = call i32 @uts_numChildren(%struct.node_t* %0)
  store i32 %call, i32* %27
  %28 = load i32* %27, align 4
  %29 = icmp sgt i32 %28, 0
  %upperBound = alloca i32, !ConsumedBy !66
  store i32 0, i32* %upperBound
  %upperBound1 = alloca i32
  store i32 0, i32* %upperBound1
  %30 = alloca i32, align 4, !ConsumedBy !68
  store i1 %29, i1* %1, align 4
  %31 = alloca i32, align 4, !ConsumedBy !70
  br i1 %29, label %uts_function13.if.then, label %uts_function13.return

uts_function13.if.then:                           ; preds = %uts_function13.entry
  %32 = alloca i32, align 4
  %call2 = call i32 @uts_childType(%struct.node_t* %0)
  store i32 %call2, i32* %32
  %33 = load i32* %32, align 4
  %34 = add nsw i32 %28, 0
  store i32 %34, i32* %31, align 4
  store i32 %34, i32* %upperBound1
  store i32 %34, i32* %upperBound
  %35 = add nsw i32 %33, 0
  store i32 %35, i32* %30, align 4
  br label %uts_function13.return

uts_function13.return:                            ; preds = %uts_function13.if.then, %uts_function13.entry
  ret void
}

; Function Attrs: alwaysinline
define void @uts_function_for.body14(i32* %arr, i32 %size) #0 {
entry:
  %arr.addr = alloca i32*, align 8
  %size.addr = alloca i32, align 4
  %i = alloca i32, align 4
  %sum = alloca i32, align 4, !ConsumedBy !53
  store i32* %arr, i32** %arr.addr, align 8
  store i32 %size, i32* %size.addr, align 4
  store i32 0, i32* %sum, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32* %i, align 4
  %1 = load i32* %size.addr, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %2 = load i32* %i, align 4
  %idxprom = sext i32 %2 to i64
  %3 = load i32** %arr.addr, align 8
  %arrayidx = getelementptr inbounds i32* %3, i64 %idxprom
  %4 = load i32* %arrayidx, align 4
  %5 = load i32* %sum, align 4
  %add = add nsw i32 %5, %4
  store i32 %add, i32* %sum, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %6 = load i32* %i, align 4
  %inc = add nsw i32 %6, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret void
}

define void @uts_function_for.body15(i32, i32, %struct.node_t*, %struct.node_t*) {
uts_function_for.body8.newFuncRoot:
  %4 = alloca %struct.node_t, !ConsumedBy !47
  br label %uts_function_for.body8.for.body

uts_function_for.body8.for.body:                  ; preds = %uts_function_for.body8.newFuncRoot
  %5 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 0
  store i32 %0, i32* %5, align 4
  %6 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 1
  %7 = load i32* %6, align 4
  %8 = add nsw i32 %7, 1
  %9 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 1
  store i32 %8, i32* %9, align 4
  %10 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 2
  store i32 -1, i32* %10, align 4
  %11 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3
  %12 = getelementptr inbounds %struct.state_t* %11, i32 0, i32 0
  %13 = getelementptr inbounds [20 x i8]* %12, i32 0, i32 0
  %14 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3
  %15 = getelementptr inbounds %struct.state_t* %14, i32 0, i32 0
  %16 = getelementptr inbounds [20 x i8]* %15, i32 0, i32 0
  call void @rng_spawn(i8* %13, i8* %16, i32 %1)
  br label %uts_function_for.body8.return

uts_function_for.body8.return:                    ; preds = %uts_function_for.body8.for.body
  %17 = alloca i32, !ConsumedBy !49
  %type.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 0
  %18 = load i32* %type.i, align 4
  %type1.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 0
  store i32 %18, i32* %type1.i, align 4
  %height.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 1
  %19 = load i32* %height.i, align 4
  %height2.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 1
  store i32 %19, i32* %height2.i, align 4
  %numChildren.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 2
  %20 = load i32* %numChildren.i, align 4
  %numChildren3.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 2
  store i32 %20, i32* %numChildren3.i, align 4
  %arrayidx.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 0
  %21 = load i8* %arrayidx.i, align 1
  %arrayidx8.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 0
  store i8 %21, i8* %arrayidx8.i, align 1
  %arrayidx.1.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 1
  %22 = load i8* %arrayidx.1.i, align 1
  %arrayidx8.1.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 1
  store i8 %22, i8* %arrayidx8.1.i, align 1
  %arrayidx.2.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 2
  %23 = load i8* %arrayidx.2.i, align 1
  %arrayidx8.2.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 2
  store i8 %23, i8* %arrayidx8.2.i, align 1
  %arrayidx.3.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 3
  %24 = load i8* %arrayidx.3.i, align 1
  %arrayidx8.3.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 3
  store i8 %24, i8* %arrayidx8.3.i, align 1
  %arrayidx.4.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 4
  %25 = load i8* %arrayidx.4.i, align 1
  %arrayidx8.4.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 4
  store i8 %25, i8* %arrayidx8.4.i, align 1
  %arrayidx.5.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 5
  %26 = load i8* %arrayidx.5.i, align 1
  %arrayidx8.5.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 5
  store i8 %26, i8* %arrayidx8.5.i, align 1
  %arrayidx.6.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 6
  %27 = load i8* %arrayidx.6.i, align 1
  %arrayidx8.6.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 6
  store i8 %27, i8* %arrayidx8.6.i, align 1
  %arrayidx.7.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 7
  %28 = load i8* %arrayidx.7.i, align 1
  %arrayidx8.7.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 7
  store i8 %28, i8* %arrayidx8.7.i, align 1
  %arrayidx.8.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 8
  %29 = load i8* %arrayidx.8.i, align 1
  %arrayidx8.8.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 8
  store i8 %29, i8* %arrayidx8.8.i, align 1
  %arrayidx.9.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 9
  %30 = load i8* %arrayidx.9.i, align 1
  %arrayidx8.9.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 9
  store i8 %30, i8* %arrayidx8.9.i, align 1
  %arrayidx.10.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 10
  %31 = load i8* %arrayidx.10.i, align 1
  %arrayidx8.10.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 10
  store i8 %31, i8* %arrayidx8.10.i, align 1
  %arrayidx.11.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 11
  %32 = load i8* %arrayidx.11.i, align 1
  %arrayidx8.11.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 11
  store i8 %32, i8* %arrayidx8.11.i, align 1
  %arrayidx.12.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 12
  %33 = load i8* %arrayidx.12.i, align 1
  %arrayidx8.12.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 12
  store i8 %33, i8* %arrayidx8.12.i, align 1
  %arrayidx.13.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 13
  %34 = load i8* %arrayidx.13.i, align 1
  %arrayidx8.13.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 13
  store i8 %34, i8* %arrayidx8.13.i, align 1
  %arrayidx.14.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 14
  %35 = load i8* %arrayidx.14.i, align 1
  %arrayidx8.14.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 14
  store i8 %35, i8* %arrayidx8.14.i, align 1
  %arrayidx.15.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 15
  %36 = load i8* %arrayidx.15.i, align 1
  %arrayidx8.15.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 15
  store i8 %36, i8* %arrayidx8.15.i, align 1
  %arrayidx.16.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 16
  %37 = load i8* %arrayidx.16.i, align 1
  %arrayidx8.16.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 16
  store i8 %37, i8* %arrayidx8.16.i, align 1
  %arrayidx.17.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 17
  %38 = load i8* %arrayidx.17.i, align 1
  %arrayidx8.17.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 17
  store i8 %38, i8* %arrayidx8.17.i, align 1
  %arrayidx.18.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 18
  %39 = load i8* %arrayidx.18.i, align 1
  %arrayidx8.18.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 18
  store i8 %39, i8* %arrayidx8.18.i, align 1
  %arrayidx.19.i = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 19
  %40 = load i8* %arrayidx.19.i, align 1
  %arrayidx8.19.i = getelementptr inbounds %struct.node_t* %4, i32 0, i32 3, i32 0, i32 19
  store i8 %40, i8* %arrayidx8.19.i, align 1
  ret void
}

attributes #0 = { alwaysinline "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }

!redefine.annotations = !{!0, !1, !2, !3, !4, !5, !6, !7, !8, !9, !10, !11, !12, !13, !14, !15, !16, !17, !18, !19, !20, !21, !22, !23, !24, !25}

!0 = metadata !{metadata !"HyperOp", void ()* @redefine_start1, metadata !"Static"}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void (i32)* @uts_function2, metadata !"Static"}
!3 = metadata !{metadata !"Exit", metadata !2}
!4 = metadata !{metadata !"HyperOp", void ()* @uts_function4, metadata !"Static"}
!5 = metadata !{metadata !"Intermediate", metadata !4}
!6 = metadata !{metadata !"HyperOp", void (i32)* @uts_function5, metadata !"Static"}
!7 = metadata !{metadata !"Intermediate", metadata !6}
!8 = metadata !{metadata !"HyperOp", void (%struct.node_t*)* @uts_function6, metadata !"Static"}
!9 = metadata !{metadata !"Intermediate", metadata !8}
!10 = metadata !{metadata !"HyperOp", void (i32*, i32)* @uts_function_for.body7, metadata !"Static"}
!11 = metadata !{metadata !"Intermediate", metadata !10}
!12 = metadata !{metadata !"HyperOp", void (i32, i32, %struct.node_t*, %struct.node_t*)* @uts_function_for.body8, metadata !"Dynamic", void (i32, i32, %struct.node_t*, %struct.node_t*)* @uts_function_for.body8, metadata !"<0>", metadata !"0", metadata !"uts_function6:upperBound1", metadata !"add", i32 1}
!13 = metadata !{metadata !"Intermediate", metadata !12}
!14 = metadata !{metadata !"HyperOp", void (i32, i32)* @uts_function9, metadata !"Dynamic", void (i32)* @uts_function2, metadata !"<0,0>"}
!15 = metadata !{metadata !"Intermediate", metadata !14}
!16 = metadata !{metadata !"HyperOp", void ()* @uts_function11, metadata !"Dynamic", void ()* @uts_function4, metadata !"<0,0>"}
!17 = metadata !{metadata !"Intermediate", metadata !16}
!18 = metadata !{metadata !"HyperOp", void (i32)* @uts_function12, metadata !"Dynamic", void (i32)* @uts_function5, metadata !"<0,0>"}
!19 = metadata !{metadata !"Intermediate", metadata !18}
!20 = metadata !{metadata !"HyperOp", void (%struct.node_t*)* @uts_function13, metadata !"Dynamic", void (%struct.node_t*)* @uts_function6, metadata !"<0,0>"}
!21 = metadata !{metadata !"Intermediate", metadata !20}
!22 = metadata !{metadata !"HyperOp", void (i32*, i32)* @uts_function_for.body14, metadata !"Dynamic", void (i32*, i32)* @uts_function_for.body7, metadata !"<0,0>"}
!23 = metadata !{metadata !"Intermediate", metadata !22}
!24 = metadata !{metadata !"HyperOp", void (i32, i32, %struct.node_t*, %struct.node_t*)* @uts_function_for.body15, metadata !"Dynamic", void (i32, i32, %struct.node_t*, %struct.node_t*)* @uts_function_for.body8, metadata !"<0,0,0>", metadata !"0", metadata !"uts_function13:upperBound1", metadata !"add", i32 1}
!25 = metadata !{metadata !"Intermediate", metadata !24}
!26 = metadata !{metadata !27}
!27 = metadata !{metadata !8, i32 0, metadata !"11"}
!28 = metadata !{metadata !29}
!29 = metadata !{metadata !2, i32 0, metadata !"11"}
!30 = metadata !{metadata !31}
!31 = metadata !{metadata !12, metadata !"1", metadata !"1n", metadata !"<id,0>"}
!32 = metadata !{metadata !33}
!33 = metadata !{metadata !12, i32 1, metadata !"1n", metadata !"<id,0>"}
!34 = metadata !{metadata !35, metadata !36}
!35 = metadata !{metadata !6, metadata !"1", metadata !"11"}
!36 = metadata !{metadata !4, metadata !"0", metadata !"11"}
!37 = metadata !{metadata !38}
!38 = metadata !{metadata !12, i32 3, metadata !"1n", metadata !"<id,0>"}
!39 = metadata !{metadata !40}
!40 = metadata !{metadata !12, i32 2, metadata !"1n", metadata !"<id,0>"}
!41 = metadata !{metadata !42}
!42 = metadata !{metadata !10, i32 1, metadata !"11"}
!43 = metadata !{metadata !44}
!44 = metadata !{metadata !12, i32 0, metadata !"1n", metadata !"<id,0>"}
!45 = metadata !{metadata !46}
!46 = metadata !{metadata !6, i32 0, metadata !"11"}
!47 = metadata !{metadata !48}
!48 = metadata !{metadata !20, i32 0, metadata !"11", metadata !"<id,0>"}
!49 = metadata !{metadata !50}
!50 = metadata !{metadata !14, i32 1, metadata !"11", metadata !"<id,0>"}
!51 = metadata !{metadata !52}
!52 = metadata !{metadata !22, i32 0, metadata !"11", metadata !"<prefixId, 2>"}
!53 = metadata !{metadata !54}
!54 = metadata !{metadata !14, i32 0, metadata !"11"}
!55 = metadata !{metadata !56}
!56 = metadata !{metadata !24, metadata !"1", metadata !"1n"}
!57 = metadata !{metadata !58}
!58 = metadata !{metadata !24, i32 1, metadata !"1n"}
!59 = metadata !{metadata !60, metadata !61}
!60 = metadata !{metadata !16, metadata !"0", metadata !"11"}
!61 = metadata !{metadata !18, metadata !"1", metadata !"11"}
!62 = metadata !{metadata !63}
!63 = metadata !{metadata !24, i32 3, metadata !"1n"}
!64 = metadata !{metadata !65}
!65 = metadata !{metadata !24, i32 2, metadata !"1n"}
!66 = metadata !{metadata !67}
!67 = metadata !{metadata !22, i32 1, metadata !"11"}
!68 = metadata !{metadata !69}
!69 = metadata !{metadata !24, i32 0, metadata !"1n"}
!70 = metadata !{metadata !71}
!71 = metadata !{metadata !18, i32 0, metadata !"11"}
