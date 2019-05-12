; ModuleID = '/Users/kavithamadhu/Compiler/REDEFINETests/forkjoin.ll'
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

declare void @uts_initRoot(%struct.node_t*, i32) #0

declare void @rng_spawn(i8*, i8*, i32) #0

declare i32 @uts_numChildren(%struct.node_t* nocapture %parent) #0

declare i32 @uts_childType(%struct.node_t* nocapture %parent) #0

define void @redefine_start1() {
redefine_start1.entry:
  %0 = alloca %struct.node_t, align 4, !ConsumedBy !30
  store i32 1, i32* @type, align 4
  store float 4.000000e+00, float* @b_0, align 4
  store i32 34, i32* @rootId, align 4
  store i32 20, i32* @gen_mx, align 4
  store i32 0, i32* @shape_fn, align 4
  store i32 1, i32* @computeGranularity, align 4
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
  %2 = alloca i32, align 4, !ConsumedBy !32
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
  %3 = alloca i1, align 4, !Controls !36
  %4 = icmp slt i32 %1, %0
  %5 = alloca i32, align 4, !ConsumedBy !38
  store i32 %1, i32* %5, align 4
  store i1 %4, i1* %3, align 4
  br i1 %4, label %uts_function5.return, label %uts_function5.return

uts_function5.return:                             ; preds = %uts_function5.for.cond, %uts_function5.for.cond
  store i1 true, i1* %2, align 4
  ret void
}

define void @uts_function6(%struct.node_t*) {
uts_function6.entry:
  %1 = alloca i1, align 4, !Controls !40
  %2 = alloca %struct.node_t, !ConsumedBy !43
  %3 = alloca %struct.node_t, align 4, !ConsumedBy !45
  %4 = call i32 @uts_numChildren(%struct.node_t* %0)
  %5 = load i32* %4, align 4
  %6 = icmp sgt i32 %5, 0
  %upperBound = alloca i32, !ConsumedBy !76
  store i32 0, i32* %upperBound
  %upperBound1 = alloca i32
  store i32 0, i32* %upperBound1
  %7 = alloca i32, align 4, !ConsumedBy !47
  %8 = alloca i32, align 4, !ConsumedBy !49
  store i1 %6, i1* %1, align 4
  br i1 %6, label %uts_function6.if.then, label %uts_function6.return

uts_function6.if.then:                            ; preds = %uts_function6.entry
  %9 = call i32 @uts_childType(%struct.node_t* %0)
  %10 = load i32* %9, align 4
  %11 = add nsw i32 %5, 0
  store i32 %11, i32* %8, align 4
  store i32 %11, i32* %upperBound1
  store i32 %11, i32* %upperBound
  %12 = add nsw i32 %10, 0
  store i32 %12, i32* %7, align 4
  br label %uts_function6.return

uts_function6.return:                             ; preds = %uts_function6.if.then, %uts_function6.entry
  ret void
}

define void @uts_function_for.body7(i32* %arr, i32 %size) #0 {
entry:
  %arr.addr = alloca i32*, align 8
  %size.addr = alloca i32, align 4
  %i = alloca i32, align 4
  %sum = alloca i32, align 4, !ConsumedBy !32
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
  %4 = alloca i32, !ConsumedBy !53
  store i32 %0, i32* %4
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
  %17 = alloca i32, !ConsumedBy !80
  ret void
}

define void @uts_function9(i32, i32 %base) {
uts_function9.entry:
  br label %uts_function9.return1

uts_function9.return1:                            ; preds = %uts_function9.entry
  %1 = phi i32 [ %0, %uts_function9.entry ]
  %2 = alloca i32, align 4, !ConsumedBy !55
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
  %2 = alloca i32, align 4, !ConsumedBy !57
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
  %2 = alloca i1, align 4, !Controls !59
  %3 = alloca i1, align 4
  %4 = icmp slt i32 %1, %0
  %5 = alloca i32, align 4, !ConsumedBy !63
  store i32 %1, i32* %5, align 4
  store i1 %4, i1* %2, align 4
  br i1 %4, label %uts_function12.return, label %uts_function12.return

uts_function12.return:                            ; preds = %uts_function12.for.cond, %uts_function12.for.cond
  store i1 true, i1* %3, align 4
  ret void
}

define void @uts_function13(%struct.node_t*) {
uts_function13.entry:
  %1 = alloca i1, align 4, !Controls !65
  %2 = alloca %struct.node_t, !ConsumedBy !68
  %3 = alloca %struct.node_t, align 4, !ConsumedBy !70
  %4 = call i32 @uts_numChildren(%struct.node_t* %0)
  %5 = load i32* %4, align 4
  %6 = icmp sgt i32 %5, 0
  %upperBound = alloca i32, !ConsumedBy !78
  store i32 0, i32* %upperBound
  %upperBound1 = alloca i32
  store i32 0, i32* %upperBound1
  %7 = alloca i32, align 4, !ConsumedBy !72
  store i1 %6, i1* %1, align 4
  %8 = alloca i32, align 4, !ConsumedBy !74
  br i1 %6, label %uts_function13.if.then, label %uts_function13.return

uts_function13.if.then:                           ; preds = %uts_function13.entry
  %9 = call i32 @uts_childType(%struct.node_t* %0)
  %10 = load i32* %9, align 4
  %11 = add nsw i32 %5, 0
  store i32 %11, i32* %8, align 4
  store i32 %11, i32* %upperBound1
  store i32 %11, i32* %upperBound
  %12 = add nsw i32 %10, 0
  store i32 %12, i32* %7, align 4
  br label %uts_function13.return

uts_function13.return:                            ; preds = %uts_function13.if.then, %uts_function13.entry
  ret void
}

define void @uts_function_for.body14(i32* %arr, i32 %size) #0 {
entry:
  %arr.addr = alloca i32*, align 8
  %size.addr = alloca i32, align 4
  %i = alloca i32, align 4
  %sum = alloca i32, align 4, !ConsumedBy !57
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
  %4 = alloca i32, !ConsumedBy !53
  store i32 %0, i32* %4
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
  %17 = alloca i32, !ConsumedBy !82
  ret void
}

attributes #0 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }

!redefine.annotations = !{!0, !1, !2, !3, !6, !7, !8, !9, !10, !11, !12, !13, !14, !15, !16, !17, !20, !21, !22, !23, !24, !25, !26, !27, !28, !29}

!0 = metadata !{metadata !"HyperOp", void ()* @redefine_start1, metadata !"Static"}
!1 = metadata !{metadata !"Entry", metadata !0}
!2 = metadata !{metadata !"HyperOp", void (i32)* @uts_function2, metadata !"Static"}
!3 = metadata !{metadata !"Exit", metadata !2}
!6 = metadata !{metadata !"HyperOp", void ()* @uts_function4, metadata !"Static"}
!7 = metadata !{metadata !"Intermediate", metadata !6}
!8 = metadata !{metadata !"HyperOp", void (i32)* @uts_function5, metadata !"Static"}
!9 = metadata !{metadata !"Intermediate", metadata !8}
!10 = metadata !{metadata !"HyperOp", void (%struct.node_t*)* @uts_function6, metadata !"Static"}
!11 = metadata !{metadata !"Intermediate", metadata !10}
!12 = metadata !{metadata !"HyperOp", void (i32*, i32)* @uts_function_for.body7, metadata !"Static"}
!13 = metadata !{metadata !"Intermediate", metadata !12}
!14 = metadata !{metadata !"HyperOp", void (i32, i32, %struct.node_t*, %struct.node_t*)* @uts_function_for.body8, metadata !"Dynamic", void (i32, i32, %struct.node_t*, %struct.node_t*)* @uts_function_for.body8, metadata !"<0>", metadata !"0", metadata !"uts_function6:upperBound1", metadata !"add", i32 1}
!15 = metadata !{metadata !"Intermediate", metadata !14}
!16 = metadata !{metadata !"HyperOp", void (i32, i32)* @uts_function9, metadata !"Dynamic", void (i32)* @uts_function2, metadata !"<0,0>"}
!17 = metadata !{metadata !"Intermediate", metadata !16}
!20 = metadata !{metadata !"HyperOp", void ()* @uts_function11, metadata !"Dynamic", void ()* @uts_function4, metadata !"<0,0>"}
!21 = metadata !{metadata !"Intermediate", metadata !20}
!22 = metadata !{metadata !"HyperOp", void (i32)* @uts_function12, metadata !"Dynamic", void (i32)* @uts_function5, metadata !"<0,0>"}
!23 = metadata !{metadata !"Intermediate", metadata !22}
!24 = metadata !{metadata !"HyperOp", void (%struct.node_t*)* @uts_function13, metadata !"Dynamic", void (%struct.node_t*)* @uts_function6, metadata !"<0,0>"}
!25 = metadata !{metadata !"Intermediate", metadata !24}
!26 = metadata !{metadata !"HyperOp", void (i32*, i32)* @uts_function_for.body14, metadata !"Dynamic", void (i32*, i32)* @uts_function_for.body7, metadata !"<0,0>"}
!27 = metadata !{metadata !"Intermediate", metadata !26}
!28 = metadata !{metadata !"HyperOp", void (i32, i32, %struct.node_t*, %struct.node_t*)* @uts_function_for.body15, metadata !"Dynamic", void (i32, i32, %struct.node_t*, %struct.node_t*)* @uts_function_for.body8, metadata !"<0,0,0>", metadata !"0", metadata !"uts_function13:upperBound1", metadata !"add", i32 1}
!29 = metadata !{metadata !"Intermediate", metadata !28}
!30 = metadata !{metadata !31}
!31 = metadata !{metadata !10, i32 0, metadata !"11"}
!32 = metadata !{metadata !33}
!33 = metadata !{metadata !2, i32 0, metadata !"11"}
!36 = metadata !{metadata !37}
!37 = metadata !{metadata !14, metadata !"1", metadata !"11", metadata !"<id,0>"}
!38 = metadata !{metadata !39}
!39 = metadata !{metadata !14, i32 1, metadata !"11", metadata !"<id,0>"}
!40 = metadata !{metadata !41, metadata !42}
!41 = metadata !{metadata !8, metadata !"1", metadata !"11"}
!42 = metadata !{metadata !6, metadata !"0", metadata !"11"}
!43 = metadata !{metadata !44}
!44 = metadata !{metadata !14, i32 3, metadata !"1n", metadata !"<id,0>"}
!45 = metadata !{metadata !46}
!46 = metadata !{metadata !14, i32 2, metadata !"1n", metadata !"<id,0>"}
!47 = metadata !{metadata !48}
!48 = metadata !{metadata !14, i32 0, metadata !"1n", metadata !"<id,0>"}
!49 = metadata !{metadata !50}
!50 = metadata !{metadata !8, i32 0, metadata !"11"}
!53 = metadata !{metadata !54}
!54 = metadata !{metadata !24, i32 0, metadata !"11", metadata !"<id,0>"}
!55 = metadata !{metadata !56}
!56 = metadata !{metadata !26, i32 0, metadata !"11", metadata !"<prefixId, 2>"}
!57 = metadata !{metadata !58}
!58 = metadata !{metadata !16, i32 0, metadata !"11"}
!59 = metadata !{metadata !60}
!60 = metadata !{metadata !28, metadata !"1", metadata !"11"}
!63 = metadata !{metadata !64}
!64 = metadata !{metadata !28, i32 1, metadata !"11"}
!65 = metadata !{metadata !66, metadata !67}
!66 = metadata !{metadata !20, metadata !"0", metadata !"11"}
!67 = metadata !{metadata !22, metadata !"1", metadata !"11"}
!68 = metadata !{metadata !69}
!69 = metadata !{metadata !28, i32 3, metadata !"1n"}
!70 = metadata !{metadata !71}
!71 = metadata !{metadata !28, i32 2, metadata !"1n"}
!72 = metadata !{metadata !73}
!73 = metadata !{metadata !28, i32 0, metadata !"1n"}
!74 = metadata !{metadata !75}
!75 = metadata !{metadata !22, i32 0, metadata !"11"}
!76 = metadata !{metadata !77}
!77 = metadata !{metadata !12, i32 1, metadata !"11"}
!78 = metadata !{metadata !79}
!79 = metadata !{metadata !26, i32 1, metadata !"11"}
!80 = metadata !{metadata !81}
!81 = metadata !{metadata !16, i32 1, metadata !"11", metadata !"<id,0>"}
!82 = metadata !{metadata !83}
!83 = metadata !{metadata !16, i32 1, metadata !"11", metadata !"<id,0>"}