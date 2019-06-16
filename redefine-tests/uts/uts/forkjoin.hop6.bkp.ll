; ModuleID = '/Users/kavithamadhu/Compiler/REDEFINETests/forkjoin.hop5.ll'
target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-n32"
target triple = "redefine"

%struct.sha1_ctx_s = type { [2 x i32], [5 x i32], [16 x i32] }
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

; Function Attrs: alwaysinline nounwind ssp
define void @redefine_start111(i32 inreg) #0 {
while.body.i.i.i.preheader:
  %ctx.i.i = alloca %struct.sha1_ctx_s, align 4
  %1 = alloca %struct.node_t, align 4
  store i32 1, i32* @type, align 4
  store float 4.000000e+00, float* @b_0, align 4
  store i32 34, i32* @rootId, align 4
  store i32 20, i32* @gen_mx, align 4
  store i32 0, i32* @shape_fn, align 4
  store i32 1, i32* @computeGranularity, align 4
  store i32 4, i32* @nonLeafBF, align 4
  store float 5.000000e-01, float* @shiftDepth, align 4
  store float 2.343750e-01, float* @nonLeafProb, align 4
  %type1.i = getelementptr inbounds %struct.node_t* %1, i32 0, i32 0
  store i32 1, i32* %type1.i, align 4
  %height.i = getelementptr inbounds %struct.node_t* %1, i32 0, i32 1
  store i32 0, i32* %height.i, align 4
  %numChildren.i = getelementptr inbounds %struct.node_t* %1, i32 0, i32 2
  store i32 0, i32* %numChildren.i, align 4
  %arrayidx.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 0, i32 1
  store i32 0, i32* %arrayidx.i.i.i, align 4
  %arrayidx2.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 0, i32 0
  %arrayidx3.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 1, i32 0
  store i32 1732584193, i32* %arrayidx3.i.i.i, align 4
  %arrayidx5.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 1, i32 1
  store i32 -271733879, i32* %arrayidx5.i.i.i, align 4
  %arrayidx7.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 1, i32 2
  store i32 -1732584194, i32* %arrayidx7.i.i.i, align 4
  %arrayidx9.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 1, i32 3
  store i32 271733878, i32* %arrayidx9.i.i.i, align 4
  %arrayidx11.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 1, i32 4
  store i32 -1009589776, i32* %arrayidx11.i.i.i, align 4
  store i32 20, i32* %arrayidx2.i.i.i, align 4
  %arraydecay60.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 0
  %2 = bitcast i32* %arraydecay60.i.i.i to i8*
  store i8 0, i8* %2, align 4
  %arrayidx61.i.i.i.1 = getelementptr inbounds i8* %2, i32 1
  store i8 0, i8* %arrayidx61.i.i.i.1, align 1
  %arrayidx61.i.i.i.2 = getelementptr inbounds i8* %2, i32 2
  store i8 0, i8* %arrayidx61.i.i.i.2, align 2
  %arrayidx61.i.i.i.3 = getelementptr inbounds i8* %2, i32 3
  store i8 0, i8* %arrayidx61.i.i.i.3, align 1
  %arrayidx61.i.i.i.45 = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 1
  %arrayidx61.i.i.i.4 = bitcast i32* %arrayidx61.i.i.i.45 to i8*
  store i8 0, i8* %arrayidx61.i.i.i.4, align 4
  %arrayidx61.i.i.i.5 = getelementptr inbounds i8* %2, i32 5
  store i8 0, i8* %arrayidx61.i.i.i.5, align 1
  %arrayidx61.i.i.i.6 = getelementptr inbounds i8* %2, i32 6
  store i8 0, i8* %arrayidx61.i.i.i.6, align 2
  %arrayidx61.i.i.i.7 = getelementptr inbounds i8* %2, i32 7
  store i8 0, i8* %arrayidx61.i.i.i.7, align 1
  %arrayidx61.i.i.i.86 = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 2
  %arrayidx61.i.i.i.8 = bitcast i32* %arrayidx61.i.i.i.86 to i8*
  store i8 0, i8* %arrayidx61.i.i.i.8, align 4
  %arrayidx61.i.i.i.9 = getelementptr inbounds i8* %2, i32 9
  store i8 0, i8* %arrayidx61.i.i.i.9, align 1
  %arrayidx61.i.i.i.10 = getelementptr inbounds i8* %2, i32 10
  store i8 0, i8* %arrayidx61.i.i.i.10, align 2
  %arrayidx61.i.i.i.11 = getelementptr inbounds i8* %2, i32 11
  store i8 0, i8* %arrayidx61.i.i.i.11, align 1
  %arrayidx61.i.i.i.127 = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 3
  %arrayidx61.i.i.i.12 = bitcast i32* %arrayidx61.i.i.i.127 to i8*
  store i8 0, i8* %arrayidx61.i.i.i.12, align 4
  %arrayidx61.i.i.i.13 = getelementptr inbounds i8* %2, i32 13
  store i8 0, i8* %arrayidx61.i.i.i.13, align 1
  %arrayidx61.i.i.i.14 = getelementptr inbounds i8* %2, i32 14
  store i8 0, i8* %arrayidx61.i.i.i.14, align 2
  %arrayidx61.i.i.i.15 = getelementptr inbounds i8* %2, i32 15
  store i8 0, i8* %arrayidx61.i.i.i.15, align 1
  %arrayidx61.i.i.i.168 = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 4
  %arrayidx61.i.i.i.16 = bitcast i32* %arrayidx61.i.i.i.168 to i8*
  store i8 0, i8* %arrayidx61.i.i.i.16, align 4
  %arrayidx61.i.i.i.17 = getelementptr inbounds i8* %2, i32 17
  store i8 0, i8* %arrayidx61.i.i.i.17, align 1
  %arrayidx61.i.i.i.18 = getelementptr inbounds i8* %2, i32 18
  store i8 0, i8* %arrayidx61.i.i.i.18, align 2
  %arrayidx61.i.i.i.19 = getelementptr inbounds i8* %2, i32 19
  store i8 34, i8* %arrayidx61.i.i.i.19, align 1
  br label %while.body.i.i.i

while.body.i.i.i:                                 ; preds = %while.body.i.i.i, %while.body.i.i.i.preheader
  %dec.i.i.i263.in = phi i32 [ %dec.i.i.i263, %while.body.i.i.i ], [ 5, %while.body.i.i.i.preheader ]
  %dec.i.i.i263 = add nsw i32 %dec.i.i.i263.in, -1
  %arrayidx1.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %dec.i.i.i263
  %3 = load i32* %arrayidx1.i.i.i, align 4
  %shr.i = ashr i32 %3, 24
  %shl.i = shl i32 %3, 8
  %or.i = or i32 %shr.i, %shl.i
  %and.i = and i32 %or.i, 16711935
  %shr1.i = ashr i32 %3, 8
  %shl2.i = shl i32 %3, 24
  %shr1.i.masked = and i32 %shr1.i, -16711936
  %and4.i = or i32 %shr1.i.masked, %shl2.i
  %or5.i = or i32 %and4.i, %and.i
  store i32 %or5.i, i32* %arrayidx1.i.i.i, align 4
  %tobool.i.i.i = icmp eq i32 %dec.i.i.i263, 0
  br i1 %tobool.i.i.i, label %while.body50.i.i.i.preheader, label %while.body.i.i.i

while.body50.i.i.i.preheader:                     ; preds = %while.body.i.i.i
  %arrayidx30.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 5
  store i32 -2147483648, i32* %arrayidx30.i.i.i, align 4
  br label %while.body50.i.i.i

while.body50.i.i.i:                               ; preds = %while.body50.i.i.i, %while.body50.i.i.i.preheader
  %i.i.i.0.i226 = phi i32 [ %inc.i.i.i, %while.body50.i.i.i ], [ 6, %while.body50.i.i.i.preheader ]
  %inc.i.i.i = add i32 %i.i.i.0.i226, 1
  %arrayidx53.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %i.i.i.0.i226
  store i32 0, i32* %arrayidx53.i.i.i, align 4
  %exitcond314 = icmp eq i32 %inc.i.i.i, 14
  br i1 %exitcond314, label %while.end54.i.i.i, label %while.body50.i.i.i

while.end54.i.i.i:                                ; preds = %while.body50.i.i.i
  %4 = load i32* %arrayidx.i.i.i, align 4
  %shl57.i.i.i = shl i32 %4, 3
  %5 = load i32* %arrayidx2.i.i.i, align 4
  %shr60.i.i.i = lshr i32 %5, 29
  %or61.i.i.i = or i32 %shr60.i.i.i, %shl57.i.i.i
  %arrayidx63.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 14
  store i32 %or61.i.i.i, i32* %arrayidx63.i.i.i, align 4
  %shl66.i.i.i = shl i32 %5, 3
  %arrayidx68.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 15
  store i32 %shl66.i.i.i, i32* %arrayidx68.i.i.i, align 4
  %6 = load i32* %arrayidx3.i.i.i, align 4
  %7 = load i32* %arrayidx5.i.i.i, align 4
  %8 = load i32* %arrayidx7.i.i.i, align 4
  %9 = load i32* %arrayidx9.i.i.i, align 4
  %10 = load i32* %arrayidx11.i.i.i, align 4
  %shr.i105.i.i.i209 = lshr i32 %6, 27
  %shl.i106.i.i.i210 = shl i32 %6, 5
  %or.i107.i.i.i211 = or i32 %shr.i105.i.i.i209, %shl.i106.i.i.i210
  %and.i108.i.i.i212 = and i32 %8, %7
  %neg.i109.i.i.i213 = xor i32 %7, -1
  %and9.i110.i.i.i214 = and i32 %9, %neg.i109.i.i.i213
  %xor.i111.i.i.i215 = xor i32 %and9.i110.i.i.i214, %and.i108.i.i.i212
  %add.i112.i.i.i216 = add i32 %or.i107.i.i.i211, 1518500249
  %add10.i113.i.i.i217 = add i32 %add.i112.i.i.i216, %xor.i111.i.i.i215
  br label %for.body.i189.i.i.i

for.body.i189.i.i.i:                              ; preds = %for.body.i189.i.i.i, %while.end54.i.i.i
  %add10.i113.i.i.i224 = phi i32 [ %add10.i113.i.i.i217, %while.end54.i.i.i ], [ %add10.i113.i.i.i, %for.body.i189.i.i.i ]
  %ii.i87.i.i.0.i223 = phi i32 [ 0, %while.end54.i.i.i ], [ %add85.i190.i.i.i, %for.body.i189.i.i.i ]
  %v4.i86.i.i.0.i222 = phi i32 [ %10, %while.end54.i.i.i ], [ %or50.i154.i.i.i, %for.body.i189.i.i.i ]
  %v3.i85.i.i.0.i221 = phi i32 [ %9, %while.end54.i.i.i ], [ %or67.i171.i.i.i, %for.body.i189.i.i.i ]
  %v2.i84.i.i.0.i220 = phi i32 [ %8, %while.end54.i.i.i ], [ %or84.i188.i.i.i, %for.body.i189.i.i.i ]
  %v1.i83.i.i.0.i219 = phi i32 [ %7, %while.end54.i.i.i ], [ %add64.i168.i.i.i, %for.body.i189.i.i.i ]
  %v0.i82.i.i.0.i218 = phi i32 [ %6, %while.end54.i.i.i ], [ %add81.i185.i.i.i, %for.body.i189.i.i.i ]
  %arrayidx11.i115.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %ii.i87.i.i.0.i223
  %11 = load i32* %arrayidx11.i115.i.i.i, align 4
  %add12.i116.i.i.i = add i32 %v4.i86.i.i.0.i222, %add10.i113.i.i.i224
  %add13.i117.i.i.i = add i32 %add12.i116.i.i.i, %11
  %shr14.i118.i.i.i = lshr i32 %v1.i83.i.i.0.i219, 2
  %shl15.i119.i.i.i = shl i32 %v1.i83.i.i.0.i219, 30
  %or16.i120.i.i.i = or i32 %shr14.i118.i.i.i, %shl15.i119.i.i.i
  %shr17.i121.i.i.i = lshr i32 %add13.i117.i.i.i, 27
  %shl18.i122.i.i.i = shl i32 %add13.i117.i.i.i, 5
  %or19.i123.i.i.i = or i32 %shr17.i121.i.i.i, %shl18.i122.i.i.i
  %and20.i124.i.i.i = and i32 %or16.i120.i.i.i, %v0.i82.i.i.0.i218
  %neg21.i125.i.i.i = xor i32 %v0.i82.i.i.0.i218, -1
  %and22.i126.i.i.i = and i32 %v2.i84.i.i.0.i220, %neg21.i125.i.i.i
  %xor23.i127.i.i.i = xor i32 %and20.i124.i.i.i, %and22.i126.i.i.i
  %add26.i130.i.i.i = add nsw i32 %ii.i87.i.i.0.i223, 1
  %arrayidx28.i132.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %add26.i130.i.i.i
  %12 = load i32* %arrayidx28.i132.i.i.i, align 4
  %add24.i128.i.i.i = add i32 %v3.i85.i.i.0.i221, 1518500249
  %add25.i129.i.i.i = add i32 %add24.i128.i.i.i, %12
  %add29.i133.i.i.i = add i32 %add25.i129.i.i.i, %xor23.i127.i.i.i
  %add30.i134.i.i.i = add i32 %add29.i133.i.i.i, %or19.i123.i.i.i
  %shr31.i135.i.i.i = lshr i32 %v0.i82.i.i.0.i218, 2
  %shl32.i136.i.i.i = shl i32 %v0.i82.i.i.0.i218, 30
  %or33.i137.i.i.i = or i32 %shr31.i135.i.i.i, %shl32.i136.i.i.i
  %shr34.i138.i.i.i = lshr i32 %add30.i134.i.i.i, 27
  %shl35.i139.i.i.i = shl i32 %add30.i134.i.i.i, 5
  %or36.i140.i.i.i = or i32 %shr34.i138.i.i.i, %shl35.i139.i.i.i
  %and37.i141.i.i.i = and i32 %add13.i117.i.i.i, %or33.i137.i.i.i
  %neg38.i142.i.i.i = xor i32 %add13.i117.i.i.i, -1
  %and39.i143.i.i.i = and i32 %or16.i120.i.i.i, %neg38.i142.i.i.i
  %xor40.i144.i.i.i = xor i32 %and37.i141.i.i.i, %and39.i143.i.i.i
  %add43.i147.i.i.i = add nsw i32 %ii.i87.i.i.0.i223, 2
  %arrayidx45.i149.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %add43.i147.i.i.i
  %13 = load i32* %arrayidx45.i149.i.i.i, align 4
  %add41.i145.i.i.i = add i32 %v2.i84.i.i.0.i220, 1518500249
  %add42.i146.i.i.i = add i32 %add41.i145.i.i.i, %13
  %add46.i150.i.i.i = add i32 %add42.i146.i.i.i, %xor40.i144.i.i.i
  %add47.i151.i.i.i = add i32 %add46.i150.i.i.i, %or36.i140.i.i.i
  %shr48.i152.i.i.i = lshr i32 %add13.i117.i.i.i, 2
  %shl49.i153.i.i.i = shl i32 %add13.i117.i.i.i, 30
  %or50.i154.i.i.i = or i32 %shr48.i152.i.i.i, %shl49.i153.i.i.i
  %shr51.i155.i.i.i = lshr i32 %add47.i151.i.i.i, 27
  %shl52.i156.i.i.i = shl i32 %add47.i151.i.i.i, 5
  %or53.i157.i.i.i = or i32 %shr51.i155.i.i.i, %shl52.i156.i.i.i
  %and54.i158.i.i.i = and i32 %add30.i134.i.i.i, %or50.i154.i.i.i
  %neg55.i159.i.i.i = xor i32 %add30.i134.i.i.i, -1
  %and56.i160.i.i.i = and i32 %or33.i137.i.i.i, %neg55.i159.i.i.i
  %xor57.i161.i.i.i = xor i32 %and54.i158.i.i.i, %and56.i160.i.i.i
  %add60.i164.i.i.i = add nsw i32 %ii.i87.i.i.0.i223, 3
  %arrayidx62.i166.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %add60.i164.i.i.i
  %14 = load i32* %arrayidx62.i166.i.i.i, align 4
  %add58.i162.i.i.i = add i32 %or16.i120.i.i.i, 1518500249
  %add59.i163.i.i.i = add i32 %add58.i162.i.i.i, %14
  %add63.i167.i.i.i = add i32 %add59.i163.i.i.i, %xor57.i161.i.i.i
  %add64.i168.i.i.i = add i32 %add63.i167.i.i.i, %or53.i157.i.i.i
  %shr65.i169.i.i.i = lshr i32 %add30.i134.i.i.i, 2
  %shl66.i170.i.i.i = shl i32 %add30.i134.i.i.i, 30
  %or67.i171.i.i.i = or i32 %shr65.i169.i.i.i, %shl66.i170.i.i.i
  %shr68.i172.i.i.i = lshr i32 %add64.i168.i.i.i, 27
  %shl69.i173.i.i.i = shl i32 %add64.i168.i.i.i, 5
  %or70.i174.i.i.i = or i32 %shr68.i172.i.i.i, %shl69.i173.i.i.i
  %and71.i175.i.i.i = and i32 %add47.i151.i.i.i, %or67.i171.i.i.i
  %neg72.i176.i.i.i = xor i32 %add47.i151.i.i.i, -1
  %and73.i177.i.i.i = and i32 %or50.i154.i.i.i, %neg72.i176.i.i.i
  %xor74.i178.i.i.i = xor i32 %and71.i175.i.i.i, %and73.i177.i.i.i
  %add77.i181.i.i.i = add nsw i32 %ii.i87.i.i.0.i223, 4
  %arrayidx79.i183.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %add77.i181.i.i.i
  %15 = load i32* %arrayidx79.i183.i.i.i, align 4
  %add75.i179.i.i.i = add i32 %or33.i137.i.i.i, 1518500249
  %add76.i180.i.i.i = add i32 %add75.i179.i.i.i, %15
  %add80.i184.i.i.i = add i32 %add76.i180.i.i.i, %xor74.i178.i.i.i
  %add81.i185.i.i.i = add i32 %add80.i184.i.i.i, %or70.i174.i.i.i
  %shr82.i186.i.i.i = lshr i32 %add47.i151.i.i.i, 2
  %shl83.i187.i.i.i = shl i32 %add47.i151.i.i.i, 30
  %or84.i188.i.i.i = or i32 %shr82.i186.i.i.i, %shl83.i187.i.i.i
  %add85.i190.i.i.i = add nsw i32 %ii.i87.i.i.0.i223, 5
  %cmp.i103.i.i.i = icmp slt i32 %add85.i190.i.i.i, 15
  %shr.i105.i.i.i = lshr i32 %add81.i185.i.i.i, 27
  %shl.i106.i.i.i = shl i32 %add81.i185.i.i.i, 5
  %or.i107.i.i.i = or i32 %shr.i105.i.i.i, %shl.i106.i.i.i
  %and.i108.i.i.i = and i32 %add64.i168.i.i.i, %or84.i188.i.i.i
  %neg.i109.i.i.i = xor i32 %add64.i168.i.i.i, -1
  %and9.i110.i.i.i = and i32 %or67.i171.i.i.i, %neg.i109.i.i.i
  %xor.i111.i.i.i = xor i32 %and9.i110.i.i.i, %and.i108.i.i.i
  %add.i112.i.i.i = add i32 %xor.i111.i.i.i, 1518500249
  %add10.i113.i.i.i = add i32 %add.i112.i.i.i, %or.i107.i.i.i
  br i1 %cmp.i103.i.i.i, label %for.body.i189.i.i.i, label %for.end.i329.i.i.i

for.end.i329.i.i.i:                               ; preds = %for.body.i189.i.i.i
  %arraydecay.i92.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 0
  %add96.i201.i.i.i = add i32 %or50.i154.i.i.i, %shl66.i.i.i
  %add97.i202.i.i.i = add i32 %add96.i201.i.i.i, %add10.i113.i.i.i
  %shr98.i203.i.i.i = lshr i32 %add64.i168.i.i.i, 2
  %shl99.i204.i.i.i = shl i32 %add64.i168.i.i.i, 30
  %or100.i205.i.i.i = or i32 %shr98.i203.i.i.i, %shl99.i204.i.i.i
  %shr101.i206.i.i.i = lshr i32 %add97.i202.i.i.i, 27
  %shl102.i207.i.i.i = shl i32 %add97.i202.i.i.i, 5
  %or103.i208.i.i.i = or i32 %shr101.i206.i.i.i, %shl102.i207.i.i.i
  %and104.i209.i.i.i = and i32 %add81.i185.i.i.i, %or100.i205.i.i.i
  %neg105.i210.i.i.i = xor i32 %add81.i185.i.i.i, -1
  %and106.i211.i.i.i = and i32 %or84.i188.i.i.i, %neg105.i210.i.i.i
  %xor107.i212.i.i.i = xor i32 %and104.i209.i.i.i, %and106.i211.i.i.i
  %arrayidx110.i215.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 13
  %16 = load i32* %arrayidx110.i215.i.i.i, align 4
  %arrayidx111.i216.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 8
  %17 = load i32* %arrayidx111.i216.i.i.i, align 4
  %xor112.i217.i.i.i = xor i32 %17, %16
  %arrayidx113.i218.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 2
  %18 = load i32* %arrayidx113.i218.i.i.i, align 4
  %xor114.i219.i.i.i = xor i32 %xor112.i217.i.i.i, %18
  %19 = load i32* %arraydecay.i92.i.i.i, align 4
  %xor116.i220.i.i.i = xor i32 %xor114.i219.i.i.i, %19
  %shl117.i221.i.i.i = shl i32 %xor116.i220.i.i.i, 1
  %shr125.i228.i.i.i = lshr i32 %xor116.i220.i.i.i, 31
  %or126.i229.i.i.i = or i32 %shl117.i221.i.i.i, %shr125.i228.i.i.i
  store i32 %or126.i229.i.i.i, i32* %arraydecay.i92.i.i.i, align 4
  %add108.i213.i.i.i = add i32 %or67.i171.i.i.i, 1518500249
  %add109.i214.i.i.i = add i32 %add108.i213.i.i.i, %xor107.i212.i.i.i
  %add128.i230.i.i.i = add i32 %add109.i214.i.i.i, %or103.i208.i.i.i
  %add129.i231.i.i.i = add i32 %add128.i230.i.i.i, %or126.i229.i.i.i
  %shr130.i232.i.i.i = lshr i32 %add81.i185.i.i.i, 2
  %shl131.i233.i.i.i = shl i32 %add81.i185.i.i.i, 30
  %or132.i234.i.i.i = or i32 %shr130.i232.i.i.i, %shl131.i233.i.i.i
  %shr133.i235.i.i.i = lshr i32 %add129.i231.i.i.i, 27
  %shl134.i236.i.i.i = shl i32 %add129.i231.i.i.i, 5
  %or135.i237.i.i.i = or i32 %shr133.i235.i.i.i, %shl134.i236.i.i.i
  %and136.i238.i.i.i = and i32 %add97.i202.i.i.i, %or132.i234.i.i.i
  %neg137.i239.i.i.i = xor i32 %add97.i202.i.i.i, -1
  %and138.i240.i.i.i = and i32 %or100.i205.i.i.i, %neg137.i239.i.i.i
  %xor139.i241.i.i.i = xor i32 %and136.i238.i.i.i, %and138.i240.i.i.i
  %arrayidx143.i245.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 9
  %20 = load i32* %arrayidx143.i245.i.i.i, align 4
  %xor144.i246.i.i.i = xor i32 %20, %or61.i.i.i
  %arrayidx145.i247.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 3
  %21 = load i32* %arrayidx145.i247.i.i.i, align 4
  %xor146.i248.i.i.i = xor i32 %xor144.i246.i.i.i, %21
  %arrayidx147.i249.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 1
  %22 = load i32* %arrayidx147.i249.i.i.i, align 4
  %xor148.i250.i.i.i = xor i32 %xor146.i248.i.i.i, %22
  %shl149.i251.i.i.i = shl i32 %xor148.i250.i.i.i, 1
  %shr157.i259.i.i.i = lshr i32 %xor148.i250.i.i.i, 31
  %or158.i260.i.i.i = or i32 %shl149.i251.i.i.i, %shr157.i259.i.i.i
  store i32 %or158.i260.i.i.i, i32* %arrayidx147.i249.i.i.i, align 4
  %add140.i242.i.i.i = add i32 %or84.i188.i.i.i, 1518500249
  %add141.i243.i.i.i = add i32 %add140.i242.i.i.i, %xor139.i241.i.i.i
  %add160.i262.i.i.i = add i32 %add141.i243.i.i.i, %or135.i237.i.i.i
  %add161.i263.i.i.i = add i32 %add160.i262.i.i.i, %or158.i260.i.i.i
  %shr162.i264.i.i.i = lshr i32 %add97.i202.i.i.i, 2
  %shl163.i265.i.i.i = shl i32 %add97.i202.i.i.i, 30
  %or164.i266.i.i.i = or i32 %shr162.i264.i.i.i, %shl163.i265.i.i.i
  %shr165.i267.i.i.i = lshr i32 %add161.i263.i.i.i, 27
  %shl166.i268.i.i.i = shl i32 %add161.i263.i.i.i, 5
  %or167.i269.i.i.i = or i32 %shr165.i267.i.i.i, %shl166.i268.i.i.i
  %and168.i270.i.i.i = and i32 %add129.i231.i.i.i, %or164.i266.i.i.i
  %neg169.i271.i.i.i = xor i32 %add129.i231.i.i.i, -1
  %and170.i272.i.i.i = and i32 %or132.i234.i.i.i, %neg169.i271.i.i.i
  %xor171.i273.i.i.i = xor i32 %and168.i270.i.i.i, %and170.i272.i.i.i
  %arrayidx175.i277.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 10
  %23 = load i32* %arrayidx175.i277.i.i.i, align 4
  %arrayidx177.i279.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 4
  %24 = load i32* %arrayidx177.i279.i.i.i, align 4
  %xor176.i278.i.i.i = xor i32 %18, %shl66.i.i.i
  %xor178.i280.i.i.i = xor i32 %xor176.i278.i.i.i, %23
  %xor180.i282.i.i.i = xor i32 %xor178.i280.i.i.i, %24
  %shl181.i283.i.i.i = shl i32 %xor180.i282.i.i.i, 1
  %shr189.i291.i.i.i = lshr i32 %xor180.i282.i.i.i, 31
  %or190.i292.i.i.i = or i32 %shl181.i283.i.i.i, %shr189.i291.i.i.i
  store i32 %or190.i292.i.i.i, i32* %arrayidx113.i218.i.i.i, align 4
  %add172.i274.i.i.i = add i32 %or100.i205.i.i.i, 1518500249
  %add173.i275.i.i.i = add i32 %add172.i274.i.i.i, %xor171.i273.i.i.i
  %add192.i294.i.i.i = add i32 %add173.i275.i.i.i, %or190.i292.i.i.i
  %add193.i295.i.i.i = add i32 %add192.i294.i.i.i, %or167.i269.i.i.i
  %shr194.i296.i.i.i = lshr i32 %add129.i231.i.i.i, 2
  %shl195.i297.i.i.i = shl i32 %add129.i231.i.i.i, 30
  %or196.i298.i.i.i = or i32 %shr194.i296.i.i.i, %shl195.i297.i.i.i
  %shr197.i299.i.i.i = lshr i32 %add193.i295.i.i.i, 27
  %shl198.i300.i.i.i = shl i32 %add193.i295.i.i.i, 5
  %or199.i301.i.i.i = or i32 %shr197.i299.i.i.i, %shl198.i300.i.i.i
  %and200.i302.i.i.i = and i32 %add161.i263.i.i.i, %or196.i298.i.i.i
  %neg201.i303.i.i.i = xor i32 %add161.i263.i.i.i, -1
  %and202.i304.i.i.i = and i32 %or164.i266.i.i.i, %neg201.i303.i.i.i
  %xor203.i305.i.i.i = xor i32 %and200.i302.i.i.i, %and202.i304.i.i.i
  %arrayidx207.i308.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 11
  %25 = load i32* %arrayidx207.i308.i.i.i, align 4
  %26 = load i32* %arrayidx30.i.i.i, align 4
  %xor208.i309.i.i.i = xor i32 %or126.i229.i.i.i, %21
  %xor210.i311.i.i.i = xor i32 %xor208.i309.i.i.i, %25
  %xor212.i313.i.i.i = xor i32 %xor210.i311.i.i.i, %26
  %shl213.i314.i.i.i = shl i32 %xor212.i313.i.i.i, 1
  %shr221.i321.i.i.i = lshr i32 %xor212.i313.i.i.i, 31
  %or222.i322.i.i.i = or i32 %shl213.i314.i.i.i, %shr221.i321.i.i.i
  store i32 %or222.i322.i.i.i, i32* %arrayidx145.i247.i.i.i, align 4
  %add204.i306.i.i.i = add i32 %or132.i234.i.i.i, 1518500249
  %add205.i307.i.i.i = add i32 %add204.i306.i.i.i, %xor203.i305.i.i.i
  %add224.i324.i.i.i = add i32 %add205.i307.i.i.i, %or222.i322.i.i.i
  %add225.i325.i.i.i = add i32 %add224.i324.i.i.i, %or199.i301.i.i.i
  %shr226.i326.i.i.i = lshr i32 %add161.i263.i.i.i, 2
  %shl227.i327.i.i.i = shl i32 %add161.i263.i.i.i, 30
  %or228.i328.i.i.i = or i32 %shr226.i326.i.i.i, %shl227.i327.i.i.i
  br label %for.body232.i638.i.i.i

for.body232.i638.i.i.i:                           ; preds = %for.body232.i638.i.i.i, %for.end.i329.i.i.i
  %ii229.i88.i.i.0.i208 = phi i32 [ 20, %for.end.i329.i.i.i ], [ %add432.i531.i.i.i, %for.body232.i638.i.i.i ]
  %v4.i86.i.i.1.i207 = phi i32 [ %or164.i266.i.i.i, %for.end.i329.i.i.i ], [ %or412.i511.i.i.i, %for.body232.i638.i.i.i ]
  %v3.i85.i.i.1.i206 = phi i32 [ %or196.i298.i.i.i, %for.end.i329.i.i.i ], [ %or475.i574.i.i.i, %for.body232.i638.i.i.i ]
  %v2.i84.i.i.1.i205 = phi i32 [ %or228.i328.i.i.i, %for.end.i329.i.i.i ], [ %or538.i637.i.i.i, %for.body232.i638.i.i.i ]
  %v1.i83.i.i.1.i204 = phi i32 [ %add193.i295.i.i.i, %for.end.i329.i.i.i ], [ %add472.i571.i.i.i, %for.body232.i638.i.i.i ]
  %v0.i82.i.i.1.i203 = phi i32 [ %add225.i325.i.i.i, %for.end.i329.i.i.i ], [ %add535.i634.i.i.i, %for.body232.i638.i.i.i ]
  %shr233.i332.i.i.i = lshr i32 %v0.i82.i.i.1.i203, 27
  %shl234.i333.i.i.i = shl i32 %v0.i82.i.i.1.i203, 5
  %or235.i334.i.i.i = or i32 %shr233.i332.i.i.i, %shl234.i333.i.i.i
  %xor236.i335.i.i.i = xor i32 %v2.i84.i.i.1.i205, %v3.i85.i.i.1.i206
  %xor237.i336.i.i.i = xor i32 %xor236.i335.i.i.i, %v1.i83.i.i.1.i204
  %add240.i339.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 13
  %and241.i340.i.i.i = and i32 %add240.i339.i.i.i, 15
  %arrayidx243.i342.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and241.i340.i.i.i
  %27 = load i32* %arrayidx243.i342.i.i.i, align 4
  %add244.i343.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 8
  %and245.i344.i.i.i = and i32 %add244.i343.i.i.i, 15
  %arrayidx247.i346.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and245.i344.i.i.i
  %28 = load i32* %arrayidx247.i346.i.i.i, align 4
  %xor248.i347.i.i.i = xor i32 %28, %27
  %add249.i348.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 2
  %and250.i349.i.i.i = and i32 %add249.i348.i.i.i, 15
  %arrayidx252.i351.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and250.i349.i.i.i
  %29 = load i32* %arrayidx252.i351.i.i.i, align 4
  %xor253.i352.i.i.i = xor i32 %xor248.i347.i.i.i, %29
  %and254.i353.i.i.i = and i32 %ii229.i88.i.i.0.i208, 15
  %arrayidx256.i355.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and254.i353.i.i.i
  %30 = load i32* %arrayidx256.i355.i.i.i, align 4
  %xor257.i356.i.i.i = xor i32 %xor253.i352.i.i.i, %30
  %shl258.i357.i.i.i = shl i32 %xor257.i356.i.i.i, 1
  %shr277.i376.i.i.i = lshr i32 %xor257.i356.i.i.i, 31
  %or278.i377.i.i.i = or i32 %shl258.i357.i.i.i, %shr277.i376.i.i.i
  store i32 %or278.i377.i.i.i, i32* %arrayidx256.i355.i.i.i, align 4
  %add238.i337.i.i.i = add i32 %v4.i86.i.i.1.i207, 1859775393
  %add239.i338.i.i.i = add i32 %add238.i337.i.i.i, %xor237.i336.i.i.i
  %add282.i381.i.i.i = add i32 %add239.i338.i.i.i, %or235.i334.i.i.i
  %add283.i382.i.i.i = add i32 %add282.i381.i.i.i, %or278.i377.i.i.i
  %shr284.i383.i.i.i = lshr i32 %v1.i83.i.i.1.i204, 2
  %shl285.i384.i.i.i = shl i32 %v1.i83.i.i.1.i204, 30
  %or286.i385.i.i.i = or i32 %shr284.i383.i.i.i, %shl285.i384.i.i.i
  %shr287.i386.i.i.i = lshr i32 %add283.i382.i.i.i, 27
  %shl288.i387.i.i.i = shl i32 %add283.i382.i.i.i, 5
  %or289.i388.i.i.i = or i32 %shr287.i386.i.i.i, %shl288.i387.i.i.i
  %xor290.i389.i.i.i = xor i32 %v0.i82.i.i.1.i203, %v2.i84.i.i.1.i205
  %xor291.i390.i.i.i = xor i32 %xor290.i389.i.i.i, %or286.i385.i.i.i
  %add294.i393.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 1
  %add295.i394.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 14
  %and296.i395.i.i.i = and i32 %add295.i394.i.i.i, 15
  %arrayidx298.i397.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and296.i395.i.i.i
  %31 = load i32* %arrayidx298.i397.i.i.i, align 4
  %add300.i399.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 9
  %and301.i400.i.i.i = and i32 %add300.i399.i.i.i, 15
  %arrayidx303.i402.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and301.i400.i.i.i
  %32 = load i32* %arrayidx303.i402.i.i.i, align 4
  %xor304.i403.i.i.i = xor i32 %32, %31
  %add306.i405.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 3
  %and307.i406.i.i.i = and i32 %add306.i405.i.i.i, 15
  %arrayidx309.i408.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and307.i406.i.i.i
  %33 = load i32* %arrayidx309.i408.i.i.i, align 4
  %xor310.i409.i.i.i = xor i32 %xor304.i403.i.i.i, %33
  %and312.i411.i.i.i = and i32 %add294.i393.i.i.i, 15
  %arrayidx314.i413.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and312.i411.i.i.i
  %34 = load i32* %arrayidx314.i413.i.i.i, align 4
  %xor315.i414.i.i.i = xor i32 %xor310.i409.i.i.i, %34
  %shl316.i415.i.i.i = shl i32 %xor315.i414.i.i.i, 1
  %shr339.i438.i.i.i = lshr i32 %xor315.i414.i.i.i, 31
  %or340.i439.i.i.i = or i32 %shl316.i415.i.i.i, %shr339.i438.i.i.i
  store i32 %or340.i439.i.i.i, i32* %arrayidx314.i413.i.i.i, align 4
  %add292.i391.i.i.i = add i32 %v3.i85.i.i.1.i206, 1859775393
  %add293.i392.i.i.i = add i32 %add292.i391.i.i.i, %xor291.i390.i.i.i
  %add345.i444.i.i.i = add i32 %add293.i392.i.i.i, %or289.i388.i.i.i
  %add346.i445.i.i.i = add i32 %add345.i444.i.i.i, %or340.i439.i.i.i
  %shr347.i446.i.i.i = lshr i32 %v0.i82.i.i.1.i203, 2
  %shl348.i447.i.i.i = shl i32 %v0.i82.i.i.1.i203, 30
  %or349.i448.i.i.i = or i32 %shr347.i446.i.i.i, %shl348.i447.i.i.i
  %shr350.i449.i.i.i = lshr i32 %add346.i445.i.i.i, 27
  %shl351.i450.i.i.i = shl i32 %add346.i445.i.i.i, 5
  %or352.i451.i.i.i = or i32 %shr350.i449.i.i.i, %shl351.i450.i.i.i
  %xor353.i452.i.i.i = xor i32 %or349.i448.i.i.i, %or286.i385.i.i.i
  %xor354.i453.i.i.i = xor i32 %xor353.i452.i.i.i, %add283.i382.i.i.i
  %add358.i457.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 15
  %and359.i458.i.i.i = and i32 %add358.i457.i.i.i, 15
  %arrayidx361.i460.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and359.i458.i.i.i
  %35 = load i32* %arrayidx361.i460.i.i.i, align 4
  %add363.i462.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 10
  %and364.i463.i.i.i = and i32 %add363.i462.i.i.i, 15
  %arrayidx366.i465.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and364.i463.i.i.i
  %36 = load i32* %arrayidx366.i465.i.i.i, align 4
  %xor367.i466.i.i.i = xor i32 %36, %35
  %add369.i468.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 4
  %and370.i469.i.i.i = and i32 %add369.i468.i.i.i, 15
  %arrayidx372.i471.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and370.i469.i.i.i
  %37 = load i32* %arrayidx372.i471.i.i.i, align 4
  %xor373.i472.i.i.i = xor i32 %xor367.i466.i.i.i, %37
  %38 = load i32* %arrayidx252.i351.i.i.i, align 4
  %xor378.i477.i.i.i = xor i32 %xor373.i472.i.i.i, %38
  %shl379.i478.i.i.i = shl i32 %xor378.i477.i.i.i, 1
  %shr402.i501.i.i.i = lshr i32 %xor378.i477.i.i.i, 31
  %or403.i502.i.i.i = or i32 %shl379.i478.i.i.i, %shr402.i501.i.i.i
  store i32 %or403.i502.i.i.i, i32* %arrayidx252.i351.i.i.i, align 4
  %add355.i454.i.i.i = add i32 %v2.i84.i.i.1.i205, 1859775393
  %add356.i455.i.i.i = add i32 %add355.i454.i.i.i, %xor354.i453.i.i.i
  %add408.i507.i.i.i = add i32 %add356.i455.i.i.i, %or352.i451.i.i.i
  %add409.i508.i.i.i = add i32 %add408.i507.i.i.i, %or403.i502.i.i.i
  %shr410.i509.i.i.i = lshr i32 %add283.i382.i.i.i, 2
  %shl411.i510.i.i.i = shl i32 %add283.i382.i.i.i, 30
  %or412.i511.i.i.i = or i32 %shr410.i509.i.i.i, %shl411.i510.i.i.i
  %shr413.i512.i.i.i = lshr i32 %add409.i508.i.i.i, 27
  %shl414.i513.i.i.i = shl i32 %add409.i508.i.i.i, 5
  %or415.i514.i.i.i = or i32 %shr413.i512.i.i.i, %shl414.i513.i.i.i
  %xor416.i515.i.i.i = xor i32 %or412.i511.i.i.i, %or349.i448.i.i.i
  %xor417.i516.i.i.i = xor i32 %xor416.i515.i.i.i, %add346.i445.i.i.i
  %39 = load i32* %arrayidx256.i355.i.i.i, align 4
  %add426.i525.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 11
  %and427.i526.i.i.i = and i32 %add426.i525.i.i.i, 15
  %arrayidx429.i528.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and427.i526.i.i.i
  %40 = load i32* %arrayidx429.i528.i.i.i, align 4
  %xor430.i529.i.i.i = xor i32 %40, %39
  %add432.i531.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 5
  %and433.i532.i.i.i = and i32 %add432.i531.i.i.i, 15
  %arrayidx435.i534.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and433.i532.i.i.i
  %41 = load i32* %arrayidx435.i534.i.i.i, align 4
  %xor436.i535.i.i.i = xor i32 %xor430.i529.i.i.i, %41
  %42 = load i32* %arrayidx309.i408.i.i.i, align 4
  %xor441.i540.i.i.i = xor i32 %xor436.i535.i.i.i, %42
  %shl442.i541.i.i.i = shl i32 %xor441.i540.i.i.i, 1
  %shr465.i564.i.i.i = lshr i32 %xor441.i540.i.i.i, 31
  %or466.i565.i.i.i = or i32 %shl442.i541.i.i.i, %shr465.i564.i.i.i
  store i32 %or466.i565.i.i.i, i32* %arrayidx309.i408.i.i.i, align 4
  %add418.i517.i.i.i = add i32 %or286.i385.i.i.i, 1859775393
  %add419.i518.i.i.i = add i32 %add418.i517.i.i.i, %xor417.i516.i.i.i
  %add471.i570.i.i.i = add i32 %add419.i518.i.i.i, %or415.i514.i.i.i
  %add472.i571.i.i.i = add i32 %add471.i570.i.i.i, %or466.i565.i.i.i
  %shr473.i572.i.i.i = lshr i32 %add346.i445.i.i.i, 2
  %shl474.i573.i.i.i = shl i32 %add346.i445.i.i.i, 30
  %or475.i574.i.i.i = or i32 %shr473.i572.i.i.i, %shl474.i573.i.i.i
  %shr476.i575.i.i.i = lshr i32 %add472.i571.i.i.i, 27
  %shl477.i576.i.i.i = shl i32 %add472.i571.i.i.i, 5
  %or478.i577.i.i.i = or i32 %shr476.i575.i.i.i, %shl477.i576.i.i.i
  %xor479.i578.i.i.i = xor i32 %or475.i574.i.i.i, %or412.i511.i.i.i
  %xor480.i579.i.i.i = xor i32 %xor479.i578.i.i.i, %add409.i508.i.i.i
  %43 = load i32* %arrayidx314.i413.i.i.i, align 4
  %add489.i588.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 12
  %and490.i589.i.i.i = and i32 %add489.i588.i.i.i, 15
  %arrayidx492.i591.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and490.i589.i.i.i
  %44 = load i32* %arrayidx492.i591.i.i.i, align 4
  %xor493.i592.i.i.i = xor i32 %44, %43
  %add495.i594.i.i.i = add nsw i32 %ii229.i88.i.i.0.i208, 6
  %and496.i595.i.i.i = and i32 %add495.i594.i.i.i, 15
  %arrayidx498.i597.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and496.i595.i.i.i
  %45 = load i32* %arrayidx498.i597.i.i.i, align 4
  %xor499.i598.i.i.i = xor i32 %xor493.i592.i.i.i, %45
  %46 = load i32* %arrayidx372.i471.i.i.i, align 4
  %xor504.i603.i.i.i = xor i32 %xor499.i598.i.i.i, %46
  %shl505.i604.i.i.i = shl i32 %xor504.i603.i.i.i, 1
  %shr528.i627.i.i.i = lshr i32 %xor504.i603.i.i.i, 31
  %or529.i628.i.i.i = or i32 %shl505.i604.i.i.i, %shr528.i627.i.i.i
  store i32 %or529.i628.i.i.i, i32* %arrayidx372.i471.i.i.i, align 4
  %add481.i580.i.i.i = add i32 %or349.i448.i.i.i, 1859775393
  %add482.i581.i.i.i = add i32 %add481.i580.i.i.i, %xor480.i579.i.i.i
  %add534.i633.i.i.i = add i32 %add482.i581.i.i.i, %or478.i577.i.i.i
  %add535.i634.i.i.i = add i32 %add534.i633.i.i.i, %or529.i628.i.i.i
  %shr536.i635.i.i.i = lshr i32 %add409.i508.i.i.i, 2
  %shl537.i636.i.i.i = shl i32 %add409.i508.i.i.i, 30
  %or538.i637.i.i.i = or i32 %shr536.i635.i.i.i, %shl537.i636.i.i.i
  %cmp231.i330.i.i.i = icmp slt i32 %add432.i531.i.i.i, 40
  br i1 %cmp231.i330.i.i.i, label %for.body232.i638.i.i.i, label %for.body545.i964.i.i.i

for.body545.i964.i.i.i:                           ; preds = %for.body545.i964.i.i.i, %for.body232.i638.i.i.i
  %ii542.i89.i.i.0.i202 = phi i32 [ %add757.i854.i.i.i, %for.body545.i964.i.i.i ], [ 40, %for.body232.i638.i.i.i ]
  %v4.i86.i.i.2.i201 = phi i32 [ %or734.i831.i.i.i, %for.body545.i964.i.i.i ], [ %or412.i511.i.i.i, %for.body232.i638.i.i.i ]
  %v3.i85.i.i.2.i200 = phi i32 [ %or800.i897.i.i.i, %for.body545.i964.i.i.i ], [ %or475.i574.i.i.i, %for.body232.i638.i.i.i ]
  %v2.i84.i.i.2.i199 = phi i32 [ %or866.i963.i.i.i, %for.body545.i964.i.i.i ], [ %or538.i637.i.i.i, %for.body232.i638.i.i.i ]
  %v1.i83.i.i.2.i198 = phi i32 [ %add797.i894.i.i.i, %for.body545.i964.i.i.i ], [ %add472.i571.i.i.i, %for.body232.i638.i.i.i ]
  %v0.i82.i.i.2.i197 = phi i32 [ %add863.i960.i.i.i, %for.body545.i964.i.i.i ], [ %add535.i634.i.i.i, %for.body232.i638.i.i.i ]
  %shr546.i643.i.i.i = lshr i32 %v0.i82.i.i.2.i197, 27
  %shl547.i644.i.i.i = shl i32 %v0.i82.i.i.2.i197, 5
  %or548.i645.i.i.i = or i32 %shr546.i643.i.i.i, %shl547.i644.i.i.i
  %and550.i647.i.i.i23 = xor i32 %v2.i84.i.i.2.i199, %v3.i85.i.i.2.i200
  %xor551.i648.i.i.i = and i32 %and550.i647.i.i.i23, %v1.i83.i.i.2.i198
  %and552.i649.i.i.i = and i32 %v2.i84.i.i.2.i199, %v3.i85.i.i.2.i200
  %xor553.i650.i.i.i = xor i32 %xor551.i648.i.i.i, %and552.i649.i.i.i
  %add556.i653.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 13
  %and557.i654.i.i.i = and i32 %add556.i653.i.i.i, 15
  %arrayidx559.i656.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and557.i654.i.i.i
  %47 = load i32* %arrayidx559.i656.i.i.i, align 4
  %add560.i657.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 8
  %and561.i658.i.i.i = and i32 %add560.i657.i.i.i, 15
  %arrayidx563.i660.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and561.i658.i.i.i
  %48 = load i32* %arrayidx563.i660.i.i.i, align 4
  %xor564.i661.i.i.i = xor i32 %48, %47
  %add565.i662.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 2
  %and566.i663.i.i.i = and i32 %add565.i662.i.i.i, 15
  %arrayidx568.i665.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and566.i663.i.i.i
  %49 = load i32* %arrayidx568.i665.i.i.i, align 4
  %xor569.i666.i.i.i = xor i32 %xor564.i661.i.i.i, %49
  %and570.i667.i.i.i = and i32 %ii542.i89.i.i.0.i202, 15
  %arrayidx572.i669.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and570.i667.i.i.i
  %50 = load i32* %arrayidx572.i669.i.i.i, align 4
  %xor573.i670.i.i.i = xor i32 %xor569.i666.i.i.i, %50
  %shl574.i671.i.i.i = shl i32 %xor573.i670.i.i.i, 1
  %shr593.i690.i.i.i = lshr i32 %xor573.i670.i.i.i, 31
  %or594.i691.i.i.i = or i32 %shl574.i671.i.i.i, %shr593.i690.i.i.i
  store i32 %or594.i691.i.i.i, i32* %arrayidx572.i669.i.i.i, align 4
  %add554.i651.i.i.i = add i32 %v4.i86.i.i.2.i201, -1894007588
  %add555.i652.i.i.i = add i32 %add554.i651.i.i.i, %xor553.i650.i.i.i
  %add598.i695.i.i.i = add i32 %add555.i652.i.i.i, %or548.i645.i.i.i
  %add599.i696.i.i.i = add i32 %add598.i695.i.i.i, %or594.i691.i.i.i
  %shr600.i697.i.i.i = lshr i32 %v1.i83.i.i.2.i198, 2
  %shl601.i698.i.i.i = shl i32 %v1.i83.i.i.2.i198, 30
  %or602.i699.i.i.i = or i32 %shr600.i697.i.i.i, %shl601.i698.i.i.i
  %shr603.i700.i.i.i = lshr i32 %add599.i696.i.i.i, 27
  %shl604.i701.i.i.i = shl i32 %add599.i696.i.i.i, 5
  %or605.i702.i.i.i = or i32 %shr603.i700.i.i.i, %shl604.i701.i.i.i
  %and607.i704.i.i.i28 = xor i32 %or602.i699.i.i.i, %v2.i84.i.i.2.i199
  %xor608.i705.i.i.i = and i32 %and607.i704.i.i.i28, %v0.i82.i.i.2.i197
  %and609.i706.i.i.i = and i32 %or602.i699.i.i.i, %v2.i84.i.i.2.i199
  %xor610.i707.i.i.i = xor i32 %xor608.i705.i.i.i, %and609.i706.i.i.i
  %add613.i710.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 1
  %add614.i711.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 14
  %and615.i712.i.i.i = and i32 %add614.i711.i.i.i, 15
  %arrayidx617.i714.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and615.i712.i.i.i
  %51 = load i32* %arrayidx617.i714.i.i.i, align 4
  %add619.i716.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 9
  %and620.i717.i.i.i = and i32 %add619.i716.i.i.i, 15
  %arrayidx622.i719.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and620.i717.i.i.i
  %52 = load i32* %arrayidx622.i719.i.i.i, align 4
  %xor623.i720.i.i.i = xor i32 %52, %51
  %add625.i722.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 3
  %and626.i723.i.i.i = and i32 %add625.i722.i.i.i, 15
  %arrayidx628.i725.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and626.i723.i.i.i
  %53 = load i32* %arrayidx628.i725.i.i.i, align 4
  %xor629.i726.i.i.i = xor i32 %xor623.i720.i.i.i, %53
  %and631.i728.i.i.i = and i32 %add613.i710.i.i.i, 15
  %arrayidx633.i730.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and631.i728.i.i.i
  %54 = load i32* %arrayidx633.i730.i.i.i, align 4
  %xor634.i731.i.i.i = xor i32 %xor629.i726.i.i.i, %54
  %shl635.i732.i.i.i = shl i32 %xor634.i731.i.i.i, 1
  %shr658.i755.i.i.i = lshr i32 %xor634.i731.i.i.i, 31
  %or659.i756.i.i.i = or i32 %shl635.i732.i.i.i, %shr658.i755.i.i.i
  store i32 %or659.i756.i.i.i, i32* %arrayidx633.i730.i.i.i, align 4
  %add611.i708.i.i.i = add i32 %v3.i85.i.i.2.i200, -1894007588
  %add612.i709.i.i.i = add i32 %add611.i708.i.i.i, %xor610.i707.i.i.i
  %add664.i761.i.i.i = add i32 %add612.i709.i.i.i, %or605.i702.i.i.i
  %add665.i762.i.i.i = add i32 %add664.i761.i.i.i, %or659.i756.i.i.i
  %shr666.i763.i.i.i = lshr i32 %v0.i82.i.i.2.i197, 2
  %shl667.i764.i.i.i = shl i32 %v0.i82.i.i.2.i197, 30
  %or668.i765.i.i.i = or i32 %shr666.i763.i.i.i, %shl667.i764.i.i.i
  %shr669.i766.i.i.i = lshr i32 %add665.i762.i.i.i, 27
  %shl670.i767.i.i.i = shl i32 %add665.i762.i.i.i, 5
  %or671.i768.i.i.i = or i32 %shr669.i766.i.i.i, %shl670.i767.i.i.i
  %and673.i770.i.i.i33 = xor i32 %or668.i765.i.i.i, %or602.i699.i.i.i
  %xor674.i771.i.i.i = and i32 %add599.i696.i.i.i, %and673.i770.i.i.i33
  %and675.i772.i.i.i = and i32 %or668.i765.i.i.i, %or602.i699.i.i.i
  %xor676.i773.i.i.i = xor i32 %xor674.i771.i.i.i, %and675.i772.i.i.i
  %add680.i777.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 15
  %and681.i778.i.i.i = and i32 %add680.i777.i.i.i, 15
  %arrayidx683.i780.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and681.i778.i.i.i
  %55 = load i32* %arrayidx683.i780.i.i.i, align 4
  %add685.i782.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 10
  %and686.i783.i.i.i = and i32 %add685.i782.i.i.i, 15
  %arrayidx688.i785.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and686.i783.i.i.i
  %56 = load i32* %arrayidx688.i785.i.i.i, align 4
  %xor689.i786.i.i.i = xor i32 %56, %55
  %add691.i788.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 4
  %and692.i789.i.i.i = and i32 %add691.i788.i.i.i, 15
  %arrayidx694.i791.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and692.i789.i.i.i
  %57 = load i32* %arrayidx694.i791.i.i.i, align 4
  %xor695.i792.i.i.i = xor i32 %xor689.i786.i.i.i, %57
  %58 = load i32* %arrayidx568.i665.i.i.i, align 4
  %xor700.i797.i.i.i = xor i32 %xor695.i792.i.i.i, %58
  %shl701.i798.i.i.i = shl i32 %xor700.i797.i.i.i, 1
  %shr724.i821.i.i.i = lshr i32 %xor700.i797.i.i.i, 31
  %or725.i822.i.i.i = or i32 %shl701.i798.i.i.i, %shr724.i821.i.i.i
  store i32 %or725.i822.i.i.i, i32* %arrayidx568.i665.i.i.i, align 4
  %add677.i774.i.i.i = add i32 %v2.i84.i.i.2.i199, -1894007588
  %add678.i775.i.i.i = add i32 %add677.i774.i.i.i, %xor676.i773.i.i.i
  %add730.i827.i.i.i = add i32 %add678.i775.i.i.i, %or671.i768.i.i.i
  %add731.i828.i.i.i = add i32 %add730.i827.i.i.i, %or725.i822.i.i.i
  %shr732.i829.i.i.i = lshr i32 %add599.i696.i.i.i, 2
  %shl733.i830.i.i.i = shl i32 %add599.i696.i.i.i, 30
  %or734.i831.i.i.i = or i32 %shr732.i829.i.i.i, %shl733.i830.i.i.i
  %shr735.i832.i.i.i = lshr i32 %add731.i828.i.i.i, 27
  %shl736.i833.i.i.i = shl i32 %add731.i828.i.i.i, 5
  %or737.i834.i.i.i = or i32 %shr735.i832.i.i.i, %shl736.i833.i.i.i
  %and739.i836.i.i.i37 = xor i32 %or734.i831.i.i.i, %or668.i765.i.i.i
  %xor740.i837.i.i.i = and i32 %add665.i762.i.i.i, %and739.i836.i.i.i37
  %and741.i838.i.i.i = and i32 %or734.i831.i.i.i, %or668.i765.i.i.i
  %xor742.i839.i.i.i = xor i32 %xor740.i837.i.i.i, %and741.i838.i.i.i
  %59 = load i32* %arrayidx572.i669.i.i.i, align 4
  %add751.i848.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 11
  %and752.i849.i.i.i = and i32 %add751.i848.i.i.i, 15
  %arrayidx754.i851.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and752.i849.i.i.i
  %60 = load i32* %arrayidx754.i851.i.i.i, align 4
  %xor755.i852.i.i.i = xor i32 %60, %59
  %add757.i854.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 5
  %and758.i855.i.i.i = and i32 %add757.i854.i.i.i, 15
  %arrayidx760.i857.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and758.i855.i.i.i
  %61 = load i32* %arrayidx760.i857.i.i.i, align 4
  %xor761.i858.i.i.i = xor i32 %xor755.i852.i.i.i, %61
  %62 = load i32* %arrayidx628.i725.i.i.i, align 4
  %xor766.i863.i.i.i = xor i32 %xor761.i858.i.i.i, %62
  %shl767.i864.i.i.i = shl i32 %xor766.i863.i.i.i, 1
  %shr790.i887.i.i.i = lshr i32 %xor766.i863.i.i.i, 31
  %or791.i888.i.i.i = or i32 %shl767.i864.i.i.i, %shr790.i887.i.i.i
  store i32 %or791.i888.i.i.i, i32* %arrayidx628.i725.i.i.i, align 4
  %add743.i840.i.i.i = add i32 %or602.i699.i.i.i, -1894007588
  %add744.i841.i.i.i = add i32 %add743.i840.i.i.i, %xor742.i839.i.i.i
  %add796.i893.i.i.i = add i32 %add744.i841.i.i.i, %or737.i834.i.i.i
  %add797.i894.i.i.i = add i32 %add796.i893.i.i.i, %or791.i888.i.i.i
  %shr798.i895.i.i.i = lshr i32 %add665.i762.i.i.i, 2
  %shl799.i896.i.i.i = shl i32 %add665.i762.i.i.i, 30
  %or800.i897.i.i.i = or i32 %shr798.i895.i.i.i, %shl799.i896.i.i.i
  %shr801.i898.i.i.i = lshr i32 %add797.i894.i.i.i, 27
  %shl802.i899.i.i.i = shl i32 %add797.i894.i.i.i, 5
  %or803.i900.i.i.i = or i32 %shr801.i898.i.i.i, %shl802.i899.i.i.i
  %and805.i902.i.i.i42 = xor i32 %or800.i897.i.i.i, %or734.i831.i.i.i
  %xor806.i903.i.i.i = and i32 %add731.i828.i.i.i, %and805.i902.i.i.i42
  %and807.i904.i.i.i = and i32 %or800.i897.i.i.i, %or734.i831.i.i.i
  %xor808.i905.i.i.i = xor i32 %xor806.i903.i.i.i, %and807.i904.i.i.i
  %63 = load i32* %arrayidx633.i730.i.i.i, align 4
  %add817.i914.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 12
  %and818.i915.i.i.i = and i32 %add817.i914.i.i.i, 15
  %arrayidx820.i917.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and818.i915.i.i.i
  %64 = load i32* %arrayidx820.i917.i.i.i, align 4
  %xor821.i918.i.i.i = xor i32 %64, %63
  %add823.i920.i.i.i = add nsw i32 %ii542.i89.i.i.0.i202, 6
  %and824.i921.i.i.i = and i32 %add823.i920.i.i.i, 15
  %arrayidx826.i923.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and824.i921.i.i.i
  %65 = load i32* %arrayidx826.i923.i.i.i, align 4
  %xor827.i924.i.i.i = xor i32 %xor821.i918.i.i.i, %65
  %66 = load i32* %arrayidx694.i791.i.i.i, align 4
  %xor832.i929.i.i.i = xor i32 %xor827.i924.i.i.i, %66
  %shl833.i930.i.i.i = shl i32 %xor832.i929.i.i.i, 1
  %shr856.i953.i.i.i = lshr i32 %xor832.i929.i.i.i, 31
  %or857.i954.i.i.i = or i32 %shl833.i930.i.i.i, %shr856.i953.i.i.i
  store i32 %or857.i954.i.i.i, i32* %arrayidx694.i791.i.i.i, align 4
  %add809.i906.i.i.i = add i32 %or668.i765.i.i.i, -1894007588
  %add810.i907.i.i.i = add i32 %add809.i906.i.i.i, %xor808.i905.i.i.i
  %add862.i959.i.i.i = add i32 %add810.i907.i.i.i, %or803.i900.i.i.i
  %add863.i960.i.i.i = add i32 %add862.i959.i.i.i, %or857.i954.i.i.i
  %shr864.i961.i.i.i = lshr i32 %add731.i828.i.i.i, 2
  %shl865.i962.i.i.i = shl i32 %add731.i828.i.i.i, 30
  %or866.i963.i.i.i = or i32 %shr864.i961.i.i.i, %shl865.i962.i.i.i
  %cmp544.i641.i.i.i = icmp slt i32 %add757.i854.i.i.i, 60
  br i1 %cmp544.i641.i.i.i, label %for.body545.i964.i.i.i, label %for.body873.i1275.i.i.i

for.body873.i1275.i.i.i:                          ; preds = %for.body873.i1275.i.i.i, %for.body545.i964.i.i.i
  %ii870.i90.i.i.0.i196 = phi i32 [ %add1073.i1168.i.i.i, %for.body873.i1275.i.i.i ], [ 60, %for.body545.i964.i.i.i ]
  %v4.i86.i.i.3.i195 = phi i32 [ %or1053.i1148.i.i.i, %for.body873.i1275.i.i.i ], [ %or734.i831.i.i.i, %for.body545.i964.i.i.i ]
  %v3.i85.i.i.3.i194 = phi i32 [ %or1116.i1211.i.i.i, %for.body873.i1275.i.i.i ], [ %or800.i897.i.i.i, %for.body545.i964.i.i.i ]
  %v2.i84.i.i.3.i193 = phi i32 [ %or1179.i1274.i.i.i, %for.body873.i1275.i.i.i ], [ %or866.i963.i.i.i, %for.body545.i964.i.i.i ]
  %v1.i83.i.i.3.i192 = phi i32 [ %add1113.i1208.i.i.i, %for.body873.i1275.i.i.i ], [ %add797.i894.i.i.i, %for.body545.i964.i.i.i ]
  %v0.i82.i.i.3.i191 = phi i32 [ %add1176.i1271.i.i.i, %for.body873.i1275.i.i.i ], [ %add863.i960.i.i.i, %for.body545.i964.i.i.i ]
  %shr874.i969.i.i.i = lshr i32 %v0.i82.i.i.3.i191, 27
  %shl875.i970.i.i.i = shl i32 %v0.i82.i.i.3.i191, 5
  %or876.i971.i.i.i = or i32 %shr874.i969.i.i.i, %shl875.i970.i.i.i
  %xor877.i972.i.i.i = xor i32 %v2.i84.i.i.3.i193, %v3.i85.i.i.3.i194
  %xor878.i973.i.i.i = xor i32 %xor877.i972.i.i.i, %v1.i83.i.i.3.i192
  %add881.i976.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 13
  %and882.i977.i.i.i = and i32 %add881.i976.i.i.i, 15
  %arrayidx884.i979.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and882.i977.i.i.i
  %67 = load i32* %arrayidx884.i979.i.i.i, align 4
  %add885.i980.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 8
  %and886.i981.i.i.i = and i32 %add885.i980.i.i.i, 15
  %arrayidx888.i983.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and886.i981.i.i.i
  %68 = load i32* %arrayidx888.i983.i.i.i, align 4
  %xor889.i984.i.i.i = xor i32 %68, %67
  %add890.i985.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 2
  %and891.i986.i.i.i = and i32 %add890.i985.i.i.i, 15
  %arrayidx893.i988.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and891.i986.i.i.i
  %69 = load i32* %arrayidx893.i988.i.i.i, align 4
  %xor894.i989.i.i.i = xor i32 %xor889.i984.i.i.i, %69
  %and895.i990.i.i.i = and i32 %ii870.i90.i.i.0.i196, 15
  %arrayidx897.i992.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and895.i990.i.i.i
  %70 = load i32* %arrayidx897.i992.i.i.i, align 4
  %xor898.i993.i.i.i = xor i32 %xor894.i989.i.i.i, %70
  %shl899.i994.i.i.i = shl i32 %xor898.i993.i.i.i, 1
  %shr918.i1013.i.i.i = lshr i32 %xor898.i993.i.i.i, 31
  %or919.i1014.i.i.i = or i32 %shl899.i994.i.i.i, %shr918.i1013.i.i.i
  store i32 %or919.i1014.i.i.i, i32* %arrayidx897.i992.i.i.i, align 4
  %add879.i974.i.i.i = add i32 %v4.i86.i.i.3.i195, -899497514
  %add880.i975.i.i.i = add i32 %add879.i974.i.i.i, %xor878.i973.i.i.i
  %add923.i1018.i.i.i = add i32 %add880.i975.i.i.i, %or876.i971.i.i.i
  %add924.i1019.i.i.i = add i32 %add923.i1018.i.i.i, %or919.i1014.i.i.i
  %shr925.i1020.i.i.i = lshr i32 %v1.i83.i.i.3.i192, 2
  %shl926.i1021.i.i.i = shl i32 %v1.i83.i.i.3.i192, 30
  %or927.i1022.i.i.i = or i32 %shr925.i1020.i.i.i, %shl926.i1021.i.i.i
  %shr928.i1023.i.i.i = lshr i32 %add924.i1019.i.i.i, 27
  %shl929.i1024.i.i.i = shl i32 %add924.i1019.i.i.i, 5
  %or930.i1025.i.i.i = or i32 %shr928.i1023.i.i.i, %shl929.i1024.i.i.i
  %xor931.i1026.i.i.i = xor i32 %v0.i82.i.i.3.i191, %v2.i84.i.i.3.i193
  %xor932.i1027.i.i.i = xor i32 %xor931.i1026.i.i.i, %or927.i1022.i.i.i
  %add935.i1030.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 1
  %add936.i1031.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 14
  %and937.i1032.i.i.i = and i32 %add936.i1031.i.i.i, 15
  %arrayidx939.i1034.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and937.i1032.i.i.i
  %71 = load i32* %arrayidx939.i1034.i.i.i, align 4
  %add941.i1036.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 9
  %and942.i1037.i.i.i = and i32 %add941.i1036.i.i.i, 15
  %arrayidx944.i1039.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and942.i1037.i.i.i
  %72 = load i32* %arrayidx944.i1039.i.i.i, align 4
  %xor945.i1040.i.i.i = xor i32 %72, %71
  %add947.i1042.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 3
  %and948.i1043.i.i.i = and i32 %add947.i1042.i.i.i, 15
  %arrayidx950.i1045.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and948.i1043.i.i.i
  %73 = load i32* %arrayidx950.i1045.i.i.i, align 4
  %xor951.i1046.i.i.i = xor i32 %xor945.i1040.i.i.i, %73
  %and953.i1048.i.i.i = and i32 %add935.i1030.i.i.i, 15
  %arrayidx955.i1050.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and953.i1048.i.i.i
  %74 = load i32* %arrayidx955.i1050.i.i.i, align 4
  %xor956.i1051.i.i.i = xor i32 %xor951.i1046.i.i.i, %74
  %shl957.i1052.i.i.i = shl i32 %xor956.i1051.i.i.i, 1
  %shr980.i1075.i.i.i = lshr i32 %xor956.i1051.i.i.i, 31
  %or981.i1076.i.i.i = or i32 %shl957.i1052.i.i.i, %shr980.i1075.i.i.i
  store i32 %or981.i1076.i.i.i, i32* %arrayidx955.i1050.i.i.i, align 4
  %add933.i1028.i.i.i = add i32 %v3.i85.i.i.3.i194, -899497514
  %add934.i1029.i.i.i = add i32 %add933.i1028.i.i.i, %xor932.i1027.i.i.i
  %add986.i1081.i.i.i = add i32 %add934.i1029.i.i.i, %or930.i1025.i.i.i
  %add987.i1082.i.i.i = add i32 %add986.i1081.i.i.i, %or981.i1076.i.i.i
  %shr988.i1083.i.i.i = lshr i32 %v0.i82.i.i.3.i191, 2
  %shl989.i1084.i.i.i = shl i32 %v0.i82.i.i.3.i191, 30
  %or990.i1085.i.i.i = or i32 %shr988.i1083.i.i.i, %shl989.i1084.i.i.i
  %shr991.i1086.i.i.i = lshr i32 %add987.i1082.i.i.i, 27
  %shl992.i1087.i.i.i = shl i32 %add987.i1082.i.i.i, 5
  %or993.i1088.i.i.i = or i32 %shr991.i1086.i.i.i, %shl992.i1087.i.i.i
  %xor994.i1089.i.i.i = xor i32 %or990.i1085.i.i.i, %or927.i1022.i.i.i
  %xor995.i1090.i.i.i = xor i32 %xor994.i1089.i.i.i, %add924.i1019.i.i.i
  %add999.i1094.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 15
  %and1000.i1095.i.i.i = and i32 %add999.i1094.i.i.i, 15
  %arrayidx1002.i1097.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and1000.i1095.i.i.i
  %75 = load i32* %arrayidx1002.i1097.i.i.i, align 4
  %add1004.i1099.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 10
  %and1005.i1100.i.i.i = and i32 %add1004.i1099.i.i.i, 15
  %arrayidx1007.i1102.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and1005.i1100.i.i.i
  %76 = load i32* %arrayidx1007.i1102.i.i.i, align 4
  %xor1008.i1103.i.i.i = xor i32 %76, %75
  %add1010.i1105.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 4
  %and1011.i1106.i.i.i = and i32 %add1010.i1105.i.i.i, 15
  %arrayidx1013.i1108.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and1011.i1106.i.i.i
  %77 = load i32* %arrayidx1013.i1108.i.i.i, align 4
  %xor1014.i1109.i.i.i = xor i32 %xor1008.i1103.i.i.i, %77
  %78 = load i32* %arrayidx893.i988.i.i.i, align 4
  %xor1019.i1114.i.i.i = xor i32 %xor1014.i1109.i.i.i, %78
  %shl1020.i1115.i.i.i = shl i32 %xor1019.i1114.i.i.i, 1
  %shr1043.i1138.i.i.i = lshr i32 %xor1019.i1114.i.i.i, 31
  %or1044.i1139.i.i.i = or i32 %shl1020.i1115.i.i.i, %shr1043.i1138.i.i.i
  store i32 %or1044.i1139.i.i.i, i32* %arrayidx893.i988.i.i.i, align 4
  %add996.i1091.i.i.i = add i32 %v2.i84.i.i.3.i193, -899497514
  %add997.i1092.i.i.i = add i32 %add996.i1091.i.i.i, %xor995.i1090.i.i.i
  %add1049.i1144.i.i.i = add i32 %add997.i1092.i.i.i, %or993.i1088.i.i.i
  %add1050.i1145.i.i.i = add i32 %add1049.i1144.i.i.i, %or1044.i1139.i.i.i
  %shr1051.i1146.i.i.i = lshr i32 %add924.i1019.i.i.i, 2
  %shl1052.i1147.i.i.i = shl i32 %add924.i1019.i.i.i, 30
  %or1053.i1148.i.i.i = or i32 %shr1051.i1146.i.i.i, %shl1052.i1147.i.i.i
  %shr1054.i1149.i.i.i = lshr i32 %add1050.i1145.i.i.i, 27
  %shl1055.i1150.i.i.i = shl i32 %add1050.i1145.i.i.i, 5
  %or1056.i1151.i.i.i = or i32 %shr1054.i1149.i.i.i, %shl1055.i1150.i.i.i
  %xor1057.i1152.i.i.i = xor i32 %or1053.i1148.i.i.i, %or990.i1085.i.i.i
  %xor1058.i1153.i.i.i = xor i32 %xor1057.i1152.i.i.i, %add987.i1082.i.i.i
  %79 = load i32* %arrayidx897.i992.i.i.i, align 4
  %add1067.i1162.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 11
  %and1068.i1163.i.i.i = and i32 %add1067.i1162.i.i.i, 15
  %arrayidx1070.i1165.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and1068.i1163.i.i.i
  %80 = load i32* %arrayidx1070.i1165.i.i.i, align 4
  %xor1071.i1166.i.i.i = xor i32 %80, %79
  %add1073.i1168.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 5
  %and1074.i1169.i.i.i = and i32 %add1073.i1168.i.i.i, 15
  %arrayidx1076.i1171.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and1074.i1169.i.i.i
  %81 = load i32* %arrayidx1076.i1171.i.i.i, align 4
  %xor1077.i1172.i.i.i = xor i32 %xor1071.i1166.i.i.i, %81
  %82 = load i32* %arrayidx950.i1045.i.i.i, align 4
  %xor1082.i1177.i.i.i = xor i32 %xor1077.i1172.i.i.i, %82
  %shl1083.i1178.i.i.i = shl i32 %xor1082.i1177.i.i.i, 1
  %shr1106.i1201.i.i.i = lshr i32 %xor1082.i1177.i.i.i, 31
  %or1107.i1202.i.i.i = or i32 %shl1083.i1178.i.i.i, %shr1106.i1201.i.i.i
  store i32 %or1107.i1202.i.i.i, i32* %arrayidx950.i1045.i.i.i, align 4
  %add1059.i1154.i.i.i = add i32 %or927.i1022.i.i.i, -899497514
  %add1060.i1155.i.i.i = add i32 %add1059.i1154.i.i.i, %xor1058.i1153.i.i.i
  %add1112.i1207.i.i.i = add i32 %add1060.i1155.i.i.i, %or1056.i1151.i.i.i
  %add1113.i1208.i.i.i = add i32 %add1112.i1207.i.i.i, %or1107.i1202.i.i.i
  %shr1114.i1209.i.i.i = lshr i32 %add987.i1082.i.i.i, 2
  %shl1115.i1210.i.i.i = shl i32 %add987.i1082.i.i.i, 30
  %or1116.i1211.i.i.i = or i32 %shr1114.i1209.i.i.i, %shl1115.i1210.i.i.i
  %shr1117.i1212.i.i.i = lshr i32 %add1113.i1208.i.i.i, 27
  %shl1118.i1213.i.i.i = shl i32 %add1113.i1208.i.i.i, 5
  %or1119.i1214.i.i.i = or i32 %shr1117.i1212.i.i.i, %shl1118.i1213.i.i.i
  %xor1120.i1215.i.i.i = xor i32 %or1116.i1211.i.i.i, %or1053.i1148.i.i.i
  %xor1121.i1216.i.i.i = xor i32 %xor1120.i1215.i.i.i, %add1050.i1145.i.i.i
  %83 = load i32* %arrayidx955.i1050.i.i.i, align 4
  %add1130.i1225.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 12
  %and1131.i1226.i.i.i = and i32 %add1130.i1225.i.i.i, 15
  %arrayidx1133.i1228.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and1131.i1226.i.i.i
  %84 = load i32* %arrayidx1133.i1228.i.i.i, align 4
  %xor1134.i1229.i.i.i = xor i32 %84, %83
  %add1136.i1231.i.i.i = add nsw i32 %ii870.i90.i.i.0.i196, 6
  %and1137.i1232.i.i.i = and i32 %add1136.i1231.i.i.i, 15
  %arrayidx1139.i1234.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 2, i32 %and1137.i1232.i.i.i
  %85 = load i32* %arrayidx1139.i1234.i.i.i, align 4
  %xor1140.i1235.i.i.i = xor i32 %xor1134.i1229.i.i.i, %85
  %86 = load i32* %arrayidx1013.i1108.i.i.i, align 4
  %xor1145.i1240.i.i.i = xor i32 %xor1140.i1235.i.i.i, %86
  %shl1146.i1241.i.i.i = shl i32 %xor1145.i1240.i.i.i, 1
  %shr1169.i1264.i.i.i = lshr i32 %xor1145.i1240.i.i.i, 31
  %or1170.i1265.i.i.i = or i32 %shl1146.i1241.i.i.i, %shr1169.i1264.i.i.i
  store i32 %or1170.i1265.i.i.i, i32* %arrayidx1013.i1108.i.i.i, align 4
  %add1122.i1217.i.i.i = add i32 %or990.i1085.i.i.i, -899497514
  %add1123.i1218.i.i.i = add i32 %add1122.i1217.i.i.i, %xor1121.i1216.i.i.i
  %add1175.i1270.i.i.i = add i32 %add1123.i1218.i.i.i, %or1119.i1214.i.i.i
  %add1176.i1271.i.i.i = add i32 %add1175.i1270.i.i.i, %or1170.i1265.i.i.i
  %shr1177.i1272.i.i.i = lshr i32 %add1050.i1145.i.i.i, 2
  %shl1178.i1273.i.i.i = shl i32 %add1050.i1145.i.i.i, 30
  %or1179.i1274.i.i.i = or i32 %shr1177.i1272.i.i.i, %shl1178.i1273.i.i.i
  %cmp872.i967.i.i.i = icmp slt i32 %add1073.i1168.i.i.i, 80
  br i1 %cmp872.i967.i.i.i, label %for.body873.i1275.i.i.i, label %sha1_compile.exit1292.i.i.i

sha1_compile.exit1292.i.i.i:                      ; preds = %for.body873.i1275.i.i.i
  %87 = load i32* %arrayidx3.i.i.i, align 4
  %add1185.i1279.i.i.i = add i32 %87, %add1176.i1271.i.i.i
  store i32 %add1185.i1279.i.i.i, i32* %arrayidx3.i.i.i, align 4
  %88 = load i32* %arrayidx5.i.i.i, align 4
  %add1188.i1282.i.i.i = add i32 %88, %add1113.i1208.i.i.i
  store i32 %add1188.i1282.i.i.i, i32* %arrayidx5.i.i.i, align 4
  %89 = load i32* %arrayidx7.i.i.i, align 4
  %add1191.i1285.i.i.i = add i32 %89, %or1179.i1274.i.i.i
  store i32 %add1191.i1285.i.i.i, i32* %arrayidx7.i.i.i, align 4
  %90 = load i32* %arrayidx9.i.i.i, align 4
  %add1194.i1288.i.i.i = add i32 %90, %or1116.i1211.i.i.i
  store i32 %add1194.i1288.i.i.i, i32* %arrayidx9.i.i.i, align 4
  %91 = load i32* %arrayidx11.i.i.i, align 4
  %add1197.i1291.i.i.i = add i32 %91, %or1053.i1148.i.i.i
  store i32 %add1197.i1291.i.i.i, i32* %arrayidx11.i.i.i, align 4
  br label %for.body.i.i.i

for.body.i.i.i:                                   ; preds = %for.body.i.i.i, %sha1_compile.exit1292.i.i.i
  %i.i.i.1.i190 = phi i32 [ 0, %sha1_compile.exit1292.i.i.i ], [ %inc79.i.i.i, %for.body.i.i.i ]
  %shr70.i.i.i = lshr i32 %i.i.i.1.i190, 2
  %arrayidx72.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i.i, i32 0, i32 1, i32 %shr70.i.i.i
  %92 = load i32* %arrayidx72.i.i.i, align 4
  %neg73.i.i.i = shl i32 %i.i.i.1.i190, 3
  %and74.i.i.i = and i32 %neg73.i.i.i, 24
  %mul75.i.i.i = xor i32 %and74.i.i.i, 24
  %shr76.i.i.i = lshr i32 %92, %mul75.i.i.i
  %conv.i.i.i = trunc i32 %shr76.i.i.i to i8
  %arrayidx78.i.i.i = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 %i.i.i.1.i190
  store i8 %conv.i.i.i, i8* %arrayidx78.i.i.i, align 1
  %inc79.i.i.i = add i32 %i.i.i.1.i190, 1
  %exitcond = icmp eq i32 %inc79.i.i.i, 20
  br i1 %exitcond, label %uts_initRoot.exit, label %for.body.i.i.i

uts_initRoot.exit:                                ; preds = %for.body.i.i.i
  call void @llvm.writecm(i32 256, i32 60, i32 1)
  %memframe_reg = call i8* @llvm.getmemframe(i32 256)
  %93 = bitcast %struct.node_t* %1 to i8*
  br label %for.body.i

for.body.i:                                       ; preds = %for.body.i, %uts_initRoot.exit
  %indvars.iv.i = phi i32 [ %indvars.iv.next.i, %for.body.i ], [ 0, %uts_initRoot.exit ]
  %arrayidx.i = getelementptr inbounds i8* %93, i32 %indvars.iv.i
  %94 = load i8* %arrayidx.i, align 1
  %arrayidx2.i = getelementptr inbounds i8* %memframe_reg, i32 %indvars.iv.i
  store i8 %94, i8* %arrayidx2.i, align 1
  %indvars.iv.next.i = add i32 %indvars.iv.i, 1
  %exitcond.i = icmp eq i32 %indvars.iv.next.i, 32
  br i1 %exitcond.i, label %redefine_memcpy.exit, label %for.body.i

redefine_memcpy.exit:                             ; preds = %for.body.i
  call void @llvm.sync(i32 256, i32 60, i32 -1)
  call void @llvm.fdelete(i32 %0)
  ret void
}

; Function Attrs: nounwind
define void @uts_function2111(i32 inreg, i32 inreg, i32 inreg, i32 inreg, i32 inreg, i32 inreg) #1 {
uts_function2.entry:
  tail call void @llvm.fdelete(i32 128)
  tail call void @llvm.fdelete(i32 192)
  %6 = shl i32 %1, 6
  %7 = add i32 %6, -64
  %8 = add nuw i32 %7, %2
  br label %uts_function_for.body811111_create_body

uts_function_for.body811111_create_body:          ; preds = %uts_function_for.body811111_create_body, %uts_function2.entry
  %9 = phi i32 [ %2, %uts_function2.entry ], [ %10, %uts_function_for.body811111_create_body ]
  tail call void @llvm.fdelete(i32 %9)
  %10 = add nuw i32 %9, 64
  %cmpinst = icmp ugt i32 %10, %8
  br i1 %cmpinst, label %uts_function_for.body811111_create_end, label %uts_function_for.body811111_create_body

uts_function_for.body811111_create_end:           ; preds = %uts_function_for.body811111_create_body
  tail call void @llvm.fdelete(i32 %0)
  ret void
}

; Function Attrs: nounwind ssp
define void @uts_function9111(i32 inreg, i32 inreg, i32 inreg, i32 inreg, i32 inreg, i32 inreg, i32 inreg, i32 inreg) #2 {
uts_function9.entry:
  %8 = alloca i32, align 4
  store i32 %7, i32* %8, align 4
  %memframe_reg = call i8* @llvm.getmemframe(i32 %5)
  %9 = bitcast i32* %8 to i8*
  br label %for.body.i

for.body.i:                                       ; preds = %for.body.i, %uts_function9.entry
  %indvars.iv.i = phi i32 [ %indvars.iv.next.i, %for.body.i ], [ 0, %uts_function9.entry ]
  %arrayidx.i = getelementptr inbounds i8* %9, i32 %indvars.iv.i
  %10 = load i8* %arrayidx.i, align 1
  %arrayidx2.i = getelementptr inbounds i8* %memframe_reg, i32 %indvars.iv.i
  store i8 %10, i8* %arrayidx2.i, align 1
  %indvars.iv.next.i = add i32 %indvars.iv.i, 1
  %exitcond.i = icmp eq i32 %indvars.iv.next.i, 4
  br i1 %exitcond.i, label %redefine_memcpy.exit, label %for.body.i

redefine_memcpy.exit:                             ; preds = %for.body.i
  call void @llvm.sync(i32 %5, i32 60, i32 -1)
  call void @llvm.fdelete(i32 %4)
  call void @llvm.fdelete(i32 %3)
  %11 = shl i32 %1, 6
  %12 = add i32 %11, -64
  %13 = add nuw i32 %12, %2
  br label %uts_function_for.body1511111_create_body

uts_function_for.body1511111_create_body:         ; preds = %uts_function_for.body1511111_create_body, %redefine_memcpy.exit
  %14 = phi i32 [ %2, %redefine_memcpy.exit ], [ %15, %uts_function_for.body1511111_create_body ]
  call void @llvm.fdelete(i32 %14)
  %15 = add nuw i32 %14, 64
  %cmpinst = icmp ugt i32 %15, %13
  br i1 %cmpinst, label %uts_function_for.body1511111_create_end, label %uts_function_for.body1511111_create_body

uts_function_for.body1511111_create_end:          ; preds = %uts_function_for.body1511111_create_body
  call void @llvm.fdelete(i32 %0)
  ret void
}

; Function Attrs: nounwind
define void @uts_function41111(i32 inreg, i32 inreg) #1 {
uts_function4.if.else:
  tail call void @llvm.writecm(i32 %1, i32 16, i32 1)
  ret void
}

; Function Attrs: nounwind
define void @uts_function51111(i32 inreg, i32 inreg, i32 inreg, i32 inreg) #1 {
uts_function5.entry:
  %4 = icmp sgt i32 %3, 0
  %5 = shl i32 %1, 6
  %6 = add i32 %5, -64
  %7 = add nuw i32 %6, %2
  %8 = zext i1 %4 to i32
  br label %uts_function_for.body811111_create_body

uts_function_for.body811111_create_body:          ; preds = %uts_function_for.body811111_create_body, %uts_function5.entry
  %9 = phi i32 [ %2, %uts_function5.entry ], [ %10, %uts_function_for.body811111_create_body ]
  tail call void @llvm.writecm(i32 %9, i32 12, i32 0)
  tail call void @llvm.writecmp(i32 %9, i32 %8)
  %10 = add nuw i32 %9, 64
  %cmpinst = icmp ugt i32 %10, %7
  br i1 %cmpinst, label %uts_function_for.body811111_create_end, label %uts_function_for.body811111_create_body

uts_function_for.body811111_create_end:           ; preds = %uts_function_for.body811111_create_body
  ret void
}

; Function Attrs: alwaysinline nounwind ssp
define void @uts_function6111(i32 inreg, %struct.node_t*) #0 {
uts_function6.entry:
  %2 = alloca %struct.node_t, align 8
  %3 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 0
  %4 = load i32* %3, align 4
  %5 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 0
  store i32 %4, i32* %5, align 8
  %6 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 1
  %7 = load i32* %6, align 4
  %8 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 1
  store i32 %7, i32* %8, align 4
  %9 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 2
  %10 = load i32* %9, align 4
  %11 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 2
  store i32 %10, i32* %11, align 8
  %12 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 0
  %13 = load i8* %12, align 1
  %14 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 0
  store i8 %13, i8* %14, align 4
  %15 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 1
  %16 = load i8* %15, align 1
  %17 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 1
  store i8 %16, i8* %17, align 1
  %18 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 2
  %19 = load i8* %18, align 1
  %20 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 2
  store i8 %19, i8* %20, align 2
  %21 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 3
  %22 = load i8* %21, align 1
  %23 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 3
  store i8 %22, i8* %23, align 1
  %24 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 4
  %25 = load i8* %24, align 1
  %26 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 4
  store i8 %25, i8* %26, align 4
  %27 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 5
  %28 = load i8* %27, align 1
  %29 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 5
  store i8 %28, i8* %29, align 1
  %30 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 6
  %31 = load i8* %30, align 1
  %32 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 6
  store i8 %31, i8* %32, align 2
  %33 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 7
  %34 = load i8* %33, align 1
  %35 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 7
  store i8 %34, i8* %35, align 1
  %36 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 8
  %37 = load i8* %36, align 1
  %38 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 8
  store i8 %37, i8* %38, align 4
  %39 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 9
  %40 = load i8* %39, align 1
  %41 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 9
  store i8 %40, i8* %41, align 1
  %42 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 10
  %43 = load i8* %42, align 1
  %44 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 10
  store i8 %43, i8* %44, align 2
  %45 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 11
  %46 = load i8* %45, align 1
  %47 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 11
  store i8 %46, i8* %47, align 1
  %48 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 12
  %49 = load i8* %48, align 1
  %50 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 12
  store i8 %49, i8* %50, align 4
  %51 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 13
  %52 = load i8* %51, align 1
  %53 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 13
  store i8 %52, i8* %53, align 1
  %54 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 14
  %55 = load i8* %54, align 1
  %56 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 14
  store i8 %55, i8* %56, align 2
  %57 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 15
  %58 = load i8* %57, align 1
  %59 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 15
  store i8 %58, i8* %59, align 1
  %60 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 16
  %61 = load i8* %60, align 1
  %62 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 16
  store i8 %61, i8* %62, align 4
  %63 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 17
  %64 = load i8* %63, align 1
  %65 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 17
  store i8 %64, i8* %65, align 1
  %66 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 18
  %67 = load i8* %66, align 1
  %68 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 18
  store i8 %67, i8* %68, align 2
  %69 = getelementptr inbounds %struct.node_t* %1, i32 0, i32 3, i32 0, i32 19
  %70 = load i8* %69, align 1
  %71 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 19
  store i8 %70, i8* %71, align 1
  %72 = alloca %struct.node_t, align 4
  %73 = load float* @b_0, align 4
  %cmp.i.i = icmp sgt i32 %7, 0
  br i1 %cmp.i.i, label %if.then.i.i, label %if.end.i.i

if.then.i.i:                                      ; preds = %uts_function6.entry
  %conv.i.i = sitofp i32 %7 to float
  %74 = load i32* @gen_mx, align 4
  %conv1.i.i = sitofp i32 %74 to float
  %mul.i.i.i = fmul float %conv1.i.i, %conv1.i.i
  %75 = bitcast float %mul.i.i.i to i32
  %mul.i.i.i.i = fmul float %mul.i.i.i, 5.000000e-01
  %shr.i.i.i.i = ashr i32 %75, 1
  %sub.i.i.i.i = sub nsw i32 1597463007, %shr.i.i.i.i
  %76 = bitcast i32 %sub.i.i.i.i to float
  %mul1.i.i.i.i = fmul float %mul.i.i.i.i, %76
  %mul2.i.i.i.i = fmul float %76, %mul1.i.i.i.i
  %sub3.i.i.i.i = fsub float 1.500000e+00, %mul2.i.i.i.i
  %mul4.i.i.i.i = fmul float %76, %sub3.i.i.i.i
  %cmp.i.i.i = fcmp olt float %conv1.i.i, 0.000000e+00
  br i1 %cmp.i.i.i, label %if.then.i.i.i, label %redefine_div.exit.i.i

if.then.i.i.i:                                    ; preds = %if.then.i.i
  %sub.i.i.i = fsub float 0.000000e+00, %mul4.i.i.i.i
  br label %redefine_div.exit.i.i

redefine_div.exit.i.i:                            ; preds = %if.then.i.i.i, %if.then.i.i
  %inv.i.i.0.i = phi float [ %sub.i.i.i, %if.then.i.i.i ], [ %mul4.i.i.i.i, %if.then.i.i ]
  %mul1.i.i.i = fmul float %conv.i.i, %inv.i.i.0.i
  %sub.i.i = fsub float 1.000000e+00, %mul1.i.i.i
  %mul.i.i = fmul float %73, %sub.i.i
  br label %if.end.i.i

if.end.i.i:                                       ; preds = %redefine_div.exit.i.i, %uts_function6.entry
  %b_i.i.0.i = phi float [ %mul.i.i, %redefine_div.exit.i.i ], [ %73, %uts_function6.entry ]
  %add.i.i = fadd float %b_i.i.0.i, 1.000000e+00
  %mul.i263.i.i = fmul float %add.i.i, %add.i.i
  %77 = bitcast float %mul.i263.i.i to i32
  %mul.i.i264.i.i = fmul float %mul.i263.i.i, 5.000000e-01
  %shr.i.i265.i.i = ashr i32 %77, 1
  %sub.i.i266.i.i = sub nsw i32 1597463007, %shr.i.i265.i.i
  %78 = bitcast i32 %sub.i.i266.i.i to float
  %mul1.i.i267.i.i = fmul float %mul.i.i264.i.i, %78
  %mul2.i.i268.i.i = fmul float %78, %mul1.i.i267.i.i
  %sub3.i.i269.i.i = fsub float 1.500000e+00, %mul2.i.i268.i.i
  %mul4.i.i270.i.i = fmul float %78, %sub3.i.i269.i.i
  %cmp.i271.i.i = fcmp olt float %add.i.i, 0.000000e+00
  br i1 %cmp.i271.i.i, label %if.then.i273.i.i, label %redefine_div.exit276.i.i

if.then.i273.i.i:                                 ; preds = %if.end.i.i
  %sub.i272.i.i = fsub float 0.000000e+00, %mul4.i.i270.i.i
  br label %redefine_div.exit276.i.i

redefine_div.exit276.i.i:                         ; preds = %if.then.i273.i.i, %if.end.i.i
  %inv.i262.i.0.i = phi float [ %sub.i272.i.i, %if.then.i273.i.i ], [ %mul4.i.i270.i.i, %if.end.i.i ]
  %conv.i254.i.i = zext i8 %61 to i32
  %shl.i.i.i = shl nuw i32 %conv.i254.i.i, 24
  %conv2.i.i.i = zext i8 %64 to i32
  %shl3.i.i.i = shl nuw nsw i32 %conv2.i.i.i, 16
  %conv5.i.i.i = zext i8 %67 to i32
  %shl6.i.i.i = shl nuw nsw i32 %conv5.i.i.i, 8
  %conv9.i.i.i = zext i8 %70 to i32
  %shl.i.i.i.masked = and i32 %shl.i.i.i, 2130706432
  %or.i255.i.i.masked = or i32 %shl.i.i.i.masked, %shl3.i.i.i
  %or7.i.i.i.masked = or i32 %or.i255.i.i.masked, %shl6.i.i.i
  %and.i256.i.i = or i32 %or7.i.i.i.masked, %conv9.i.i.i
  %conv.i250.i.i = sitofp i32 %and.i256.i.i to float
  %mul1.i.i253.i.i = fmul float %conv.i250.i.i, 0x3DFFF221E0000000
  %sub6.i.i = fsub float 1.000000e+00, %mul1.i.i253.i.i
  %79 = bitcast float %sub6.i.i to i32
  %cmp.i105.i.i = icmp slt i32 %79, 8388608
  br i1 %cmp.i105.i.i, label %if.then.i108.i.i, label %if.end17.i144.i.i

if.then.i108.i.i:                                 ; preds = %redefine_div.exit276.i.i
  %and.i106.i.i = and i32 %79, 2147483647
  %cmp1.i107.i.i = icmp eq i32 %and.i106.i.i, 0
  br i1 %cmp1.i107.i.i, label %redefine_logf.exit247.i.i, label %if.end.i119.i.i

if.end.i119.i.i:                                  ; preds = %if.then.i108.i.i
  %cmp3.i118.i.i = icmp slt i32 %79, 0
  br i1 %cmp3.i118.i.i, label %if.then4.i137.i.i, label %if.end9.i140.i.i

if.then4.i137.i.i:                                ; preds = %if.end.i119.i.i
  %sub.i120.i.i = fsub float %sub6.i.i, %sub6.i.i
  %mul6.i128.i.i = fmul float %sub.i120.i.i, 0x43F1306CE0000000
  %mul8.i136.i.i = fmul float %mul6.i128.i.i, 0x43F1306CE0000000
  br label %redefine_logf.exit247.i.i

if.end9.i140.i.i:                                 ; preds = %if.end.i119.i.i
  %mul11.i139.i.i = fmul float %sub6.i.i, 0x4180000000000000
  %80 = bitcast float %mul11.i139.i.i to i32
  br label %if.end17.i144.i.i

if.end17.i144.i.i:                                ; preds = %if.end9.i140.i.i, %redefine_div.exit276.i.i
  %ix.i97.i.0.i = phi i32 [ %80, %if.end9.i140.i.i ], [ %79, %redefine_div.exit276.i.i ]
  %k.i96.i.0.i = phi i32 [ -25, %if.end9.i140.i.i ], [ 0, %redefine_div.exit276.i.i ]
  %x.addr.i86.i.0.i = phi float [ %mul11.i139.i.i, %if.end9.i140.i.i ], [ %sub6.i.i, %redefine_div.exit276.i.i ]
  %cmp18.i143.i.i = icmp sgt i32 %ix.i97.i.0.i, 2139095039
  br i1 %cmp18.i143.i.i, label %if.then19.i146.i.i, label %if.end20.i153.i.i

if.then19.i146.i.i:                               ; preds = %if.end17.i144.i.i
  %add.i145.i.i = fadd float %x.addr.i86.i.0.i, %x.addr.i86.i.0.i
  br label %redefine_logf.exit247.i.i

if.end20.i153.i.i:                                ; preds = %if.end17.i144.i.i
  %shr.i147.i.i = ashr i32 %ix.i97.i.0.i, 23
  %sub21.i148.i.i = add nsw i32 %shr.i147.i.i, -127
  %add22.i149.i.i = add nsw i32 %sub21.i148.i.i, %k.i96.i.0.i
  %81 = and i32 %ix.i97.i.0.i, 8388607
  %add24.i151.i.i = add nsw i32 %81, 4913952
  %and25.i152.i.i = and i32 %add24.i151.i.i, 8388608
  %and23.i150.i.i = or i32 %and25.i152.i.i, %81
  %or.i155.i.i = xor i32 %and23.i150.i.i, 1065353216
  %82 = bitcast i32 %or.i155.i.i to float
  %shr30.i158.i.i14 = lshr exact i32 %and25.i152.i.i, 23
  %add31.i159.i.i = add nsw i32 %add22.i149.i.i, %shr30.i158.i.i14
  %sub32.i160.i.i = fadd float %82, -1.000000e+00
  %add33.i161.i.i = add nsw i32 %ix.i97.i.0.i, 32768
  %and34.i162.i.i = and i32 %add33.i161.i.i, 8372224
  %cmp35.i163.i.i = icmp ult i32 %and34.i162.i.i, 49152
  br i1 %cmp35.i163.i.i, label %if.then36.i165.i.i, label %if.end60.i214.i.i

if.then36.i165.i.i:                               ; preds = %if.end20.i153.i.i
  %cmp37.i164.i.i = fcmp oeq float %sub32.i160.i.i, 0.000000e+00
  br i1 %cmp37.i164.i.i, label %if.then38.i167.i.i, label %if.end44.i179.i.i

if.then38.i167.i.i:                               ; preds = %if.then36.i165.i.i
  %cmp39.i166.i.i = icmp eq i32 %add31.i159.i.i, 0
  br i1 %cmp39.i166.i.i, label %redefine_logf.exit247.i.i, label %if.else.i173.i.i

if.else.i173.i.i:                                 ; preds = %if.then38.i167.i.i
  %conv.i169.i.i = sitofp i32 %add31.i159.i.i to float
  %mul41.i170.i.i = fmul float %conv.i169.i.i, 0x3FE62E3000000000
  %mul42.i171.i.i = fmul float %conv.i169.i.i, 0x3EE2FEFA20000000
  %add43.i172.i.i = fadd float %mul41.i170.i.i, %mul42.i171.i.i
  br label %redefine_logf.exit247.i.i

if.end44.i179.i.i:                                ; preds = %if.then36.i165.i.i
  %mul45.i174.i.i = fmul float %sub32.i160.i.i, %sub32.i160.i.i
  %mul46.i175.i.i = fmul float %sub32.i160.i.i, 0x3FD5555560000000
  %sub47.i176.i.i = fsub float 5.000000e-01, %mul46.i175.i.i
  %mul48.i177.i.i = fmul float %mul45.i174.i.i, %sub47.i176.i.i
  %cmp49.i178.i.i = icmp eq i32 %add31.i159.i.i, 0
  br i1 %cmp49.i178.i.i, label %if.then51.i181.i.i, label %if.else53.i188.i.i

if.then51.i181.i.i:                               ; preds = %if.end44.i179.i.i
  %sub52.i180.i.i = fsub float %sub32.i160.i.i, %mul48.i177.i.i
  br label %redefine_logf.exit247.i.i

if.else53.i188.i.i:                               ; preds = %if.end44.i179.i.i
  %conv54.i182.i.i = sitofp i32 %add31.i159.i.i to float
  %mul55.i183.i.i = fmul float %conv54.i182.i.i, 0x3FE62E3000000000
  %mul56.i184.i.i = fmul float %conv54.i182.i.i, 0x3EE2FEFA20000000
  %sub57.i185.i.i = fsub float %mul48.i177.i.i, %mul56.i184.i.i
  %sub58.i186.i.i = fsub float %sub57.i185.i.i, %sub32.i160.i.i
  %sub59.i187.i.i = fsub float %mul55.i183.i.i, %sub58.i186.i.i
  br label %redefine_logf.exit247.i.i

if.end60.i214.i.i:                                ; preds = %if.end20.i153.i.i
  %add61.i189.i.i = fadd float %sub32.i160.i.i, 2.000000e+00
  %mul63.i191.i.i = fmul float %add61.i189.i.i, %add61.i189.i.i
  %83 = bitcast float %mul63.i191.i.i to i32
  %mul.i118.i192.i.i = fmul float %mul63.i191.i.i, 5.000000e-01
  %shr.i119.i193.i.i = ashr i32 %83, 1
  %sub.i120.i194.i.i = sub nsw i32 1597463007, %shr.i119.i193.i.i
  %84 = bitcast i32 %sub.i120.i194.i.i to float
  %mul1.i121.i195.i.i = fmul float %mul.i118.i192.i.i, %84
  %mul2.i122.i196.i.i = fmul float %84, %mul1.i121.i195.i.i
  %sub3.i123.i197.i.i = fsub float 1.500000e+00, %mul2.i122.i196.i.i
  %mul4.i124.i198.i.i = fmul float %84, %sub3.i123.i197.i.i
  %mul65.i199.i.i = fmul float %sub32.i160.i.i, %mul4.i124.i198.i.i
  %conv66.i200.i.i = sitofp i32 %add31.i159.i.i to float
  %mul67.i201.i.i = fmul float %mul65.i199.i.i, %mul65.i199.i.i
  %sub68.i202.i.i = add nsw i32 %81, -3187664
  %mul69.i203.i.i = fmul float %mul67.i201.i.i, %mul67.i201.i.i
  %sub70.i204.i.i = sub nsw i32 3523208, %81
  %mul71.i205.i.i = fmul float %mul69.i203.i.i, 0x3FCF13C4C0000000
  %add72.i206.i.i = fadd float %mul71.i205.i.i, 0x3FD999C260000000
  %mul73.i207.i.i = fmul float %mul69.i203.i.i, %add72.i206.i.i
  %mul74.i208.i.i = fmul float %mul69.i203.i.i, 0x3FD23D3DC0000000
  %add75.i209.i.i = fadd float %mul74.i208.i.i, 0x3FE5555540000000
  %mul76.i210.i.i = fmul float %mul67.i201.i.i, %add75.i209.i.i
  %or77.i211.i.i = or i32 %sub68.i202.i.i, %sub70.i204.i.i
  %add78.i212.i.i = fadd float %mul76.i210.i.i, %mul73.i207.i.i
  %cmp79.i213.i.i = icmp sgt i32 %or77.i211.i.i, 0
  br i1 %cmp79.i213.i.i, label %if.then81.i218.i.i, label %if.else100.i234.i.i

if.then81.i218.i.i:                               ; preds = %if.end60.i214.i.i
  %mul82.i215.i.i = fmul float %sub32.i160.i.i, 5.000000e-01
  %mul83.i216.i.i = fmul float %sub32.i160.i.i, %mul82.i215.i.i
  %cmp84.i217.i.i = icmp eq i32 %add31.i159.i.i, 0
  br i1 %cmp84.i217.i.i, label %if.then86.i223.i.i, label %if.else91.i232.i.i

if.then86.i223.i.i:                               ; preds = %if.then81.i218.i.i
  %add87.i219.i.i = fadd float %mul83.i216.i.i, %add78.i212.i.i
  %mul88.i220.i.i = fmul float %mul65.i199.i.i, %add87.i219.i.i
  %sub89.i221.i.i = fsub float %mul83.i216.i.i, %mul88.i220.i.i
  %sub90.i222.i.i = fsub float %sub32.i160.i.i, %sub89.i221.i.i
  br label %redefine_logf.exit247.i.i

if.else91.i232.i.i:                               ; preds = %if.then81.i218.i.i
  %mul92.i224.i.i = fmul float %conv66.i200.i.i, 0x3FE62E3000000000
  %add93.i225.i.i = fadd float %mul83.i216.i.i, %add78.i212.i.i
  %mul94.i226.i.i = fmul float %mul65.i199.i.i, %add93.i225.i.i
  %mul95.i227.i.i = fmul float %conv66.i200.i.i, 0x3EE2FEFA20000000
  %add96.i228.i.i = fadd float %mul95.i227.i.i, %mul94.i226.i.i
  %sub97.i229.i.i = fsub float %mul83.i216.i.i, %add96.i228.i.i
  %sub98.i230.i.i = fsub float %sub97.i229.i.i, %sub32.i160.i.i
  %sub99.i231.i.i = fsub float %mul92.i224.i.i, %sub98.i230.i.i
  br label %redefine_logf.exit247.i.i

if.else100.i234.i.i:                              ; preds = %if.end60.i214.i.i
  %cmp101.i233.i.i = icmp eq i32 %add31.i159.i.i, 0
  br i1 %cmp101.i233.i.i, label %if.then103.i238.i.i, label %if.else107.i246.i.i

if.then103.i238.i.i:                              ; preds = %if.else100.i234.i.i
  %sub104.i235.i.i = fsub float %sub32.i160.i.i, %add78.i212.i.i
  %mul105.i236.i.i = fmul float %mul65.i199.i.i, %sub104.i235.i.i
  %sub106.i237.i.i = fsub float %sub32.i160.i.i, %mul105.i236.i.i
  br label %redefine_logf.exit247.i.i

if.else107.i246.i.i:                              ; preds = %if.else100.i234.i.i
  %mul108.i239.i.i = fmul float %conv66.i200.i.i, 0x3FE62E3000000000
  %sub109.i240.i.i = fsub float %sub32.i160.i.i, %add78.i212.i.i
  %mul110.i241.i.i = fmul float %mul65.i199.i.i, %sub109.i240.i.i
  %mul111.i242.i.i = fmul float %conv66.i200.i.i, 0x3EE2FEFA20000000
  %sub112.i243.i.i = fsub float %mul110.i241.i.i, %mul111.i242.i.i
  %sub113.i244.i.i = fsub float %sub112.i243.i.i, %sub32.i160.i.i
  %sub114.i245.i.i = fsub float %mul108.i239.i.i, %sub113.i244.i.i
  br label %redefine_logf.exit247.i.i

redefine_logf.exit247.i.i:                        ; preds = %if.else107.i246.i.i, %if.then103.i238.i.i, %if.else91.i232.i.i, %if.then86.i223.i.i, %if.else53.i188.i.i, %if.then51.i181.i.i, %if.else.i173.i.i, %if.then38.i167.i.i, %if.then19.i146.i.i, %if.then4.i137.i.i, %if.then.i108.i.i
  %retval.i85.i.0.i = phi float [ %mul8.i136.i.i, %if.then4.i137.i.i ], [ %add.i145.i.i, %if.then19.i146.i.i ], [ %add43.i172.i.i, %if.else.i173.i.i ], [ %sub52.i180.i.i, %if.then51.i181.i.i ], [ %sub59.i187.i.i, %if.else53.i188.i.i ], [ %sub90.i222.i.i, %if.then86.i223.i.i ], [ %sub99.i231.i.i, %if.else91.i232.i.i ], [ %sub106.i237.i.i, %if.then103.i238.i.i ], [ %sub114.i245.i.i, %if.else107.i246.i.i ], [ 0xC581306CE0000000, %if.then.i108.i.i ], [ 0.000000e+00, %if.then38.i167.i.i ]
  %sub8.i.i = fsub float 1.000000e+00, %inv.i262.i.0.i
  %85 = bitcast float %sub8.i.i to i32
  %cmp.i51.i.i = icmp slt i32 %85, 8388608
  br i1 %cmp.i51.i.i, label %if.then.i54.i.i, label %if.end17.i68.i.i

if.then.i54.i.i:                                  ; preds = %redefine_logf.exit247.i.i
  %and.i52.i.i = and i32 %85, 2147483647
  %cmp1.i53.i.i = icmp eq i32 %and.i52.i.i, 0
  br i1 %cmp1.i53.i.i, label %redefine_logf.exit.i.i, label %if.end.i65.i.i

if.end.i65.i.i:                                   ; preds = %if.then.i54.i.i
  %cmp3.i64.i.i = icmp slt i32 %85, 0
  br i1 %cmp3.i64.i.i, label %if.then4.i67.i.i, label %if.end9.i.i.i

if.then4.i67.i.i:                                 ; preds = %if.end.i65.i.i
  %sub.i66.i.i = fsub float %sub8.i.i, %sub8.i.i
  %mul6.i.i.i = fmul float %sub.i66.i.i, 0x43F1306CE0000000
  %mul8.i.i.i = fmul float %mul6.i.i.i, 0x43F1306CE0000000
  br label %redefine_logf.exit.i.i

if.end9.i.i.i:                                    ; preds = %if.end.i65.i.i
  %mul11.i.i.i = fmul float %sub8.i.i, 0x4180000000000000
  %86 = bitcast float %mul11.i.i.i to i32
  br label %if.end17.i68.i.i

if.end17.i68.i.i:                                 ; preds = %if.end9.i.i.i, %redefine_logf.exit247.i.i
  %ix.i.i.0.i = phi i32 [ %86, %if.end9.i.i.i ], [ %85, %redefine_logf.exit247.i.i ]
  %k.i.i.0.i = phi i32 [ -25, %if.end9.i.i.i ], [ 0, %redefine_logf.exit247.i.i ]
  %x.addr.i45.i.0.i = phi float [ %mul11.i.i.i, %if.end9.i.i.i ], [ %sub8.i.i, %redefine_logf.exit247.i.i ]
  %cmp18.i.i.i = icmp sgt i32 %ix.i.i.0.i, 2139095039
  br i1 %cmp18.i.i.i, label %if.then19.i.i.i, label %if.end20.i.i.i

if.then19.i.i.i:                                  ; preds = %if.end17.i68.i.i
  %add.i69.i.i = fadd float %x.addr.i45.i.0.i, %x.addr.i45.i.0.i
  br label %redefine_logf.exit.i.i

if.end20.i.i.i:                                   ; preds = %if.end17.i68.i.i
  %shr.i70.i.i = ashr i32 %ix.i.i.0.i, 23
  %sub21.i.i.i = add nsw i32 %shr.i70.i.i, -127
  %add22.i.i.i = add nsw i32 %sub21.i.i.i, %k.i.i.0.i
  %87 = and i32 %ix.i.i.0.i, 8388607
  %add24.i71.i.i = add nsw i32 %87, 4913952
  %and25.i.i.i = and i32 %add24.i71.i.i, 8388608
  %and23.i.i.i = or i32 %and25.i.i.i, %87
  %or.i.i.i = xor i32 %and23.i.i.i, 1065353216
  %88 = bitcast i32 %or.i.i.i to float
  %shr30.i.i.i15 = lshr exact i32 %and25.i.i.i, 23
  %add31.i.i.i = add nsw i32 %add22.i.i.i, %shr30.i.i.i15
  %sub32.i.i.i = fadd float %88, -1.000000e+00
  %add33.i.i.i = add nsw i32 %ix.i.i.0.i, 32768
  %and34.i.i.i = and i32 %add33.i.i.i, 8372224
  %cmp35.i.i.i = icmp ult i32 %and34.i.i.i, 49152
  br i1 %cmp35.i.i.i, label %if.then36.i.i.i, label %if.end60.i.i.i

if.then36.i.i.i:                                  ; preds = %if.end20.i.i.i
  %cmp37.i.i.i = fcmp oeq float %sub32.i.i.i, 0.000000e+00
  br i1 %cmp37.i.i.i, label %if.then38.i.i.i, label %if.end44.i.i.i

if.then38.i.i.i:                                  ; preds = %if.then36.i.i.i
  %cmp39.i.i.i = icmp eq i32 %add31.i.i.i, 0
  br i1 %cmp39.i.i.i, label %redefine_logf.exit.i.i, label %if.else.i72.i.i

if.else.i72.i.i:                                  ; preds = %if.then38.i.i.i
  %conv.i.i.i = sitofp i32 %add31.i.i.i to float
  %mul41.i.i.i = fmul float %conv.i.i.i, 0x3FE62E3000000000
  %mul42.i.i.i = fmul float %conv.i.i.i, 0x3EE2FEFA20000000
  %add43.i.i.i = fadd float %mul41.i.i.i, %mul42.i.i.i
  br label %redefine_logf.exit.i.i

if.end44.i.i.i:                                   ; preds = %if.then36.i.i.i
  %mul45.i.i.i = fmul float %sub32.i.i.i, %sub32.i.i.i
  %mul46.i.i.i = fmul float %sub32.i.i.i, 0x3FD5555560000000
  %sub47.i.i.i = fsub float 5.000000e-01, %mul46.i.i.i
  %mul48.i.i.i = fmul float %mul45.i.i.i, %sub47.i.i.i
  %cmp49.i.i.i = icmp eq i32 %add31.i.i.i, 0
  br i1 %cmp49.i.i.i, label %if.then51.i.i.i, label %if.else53.i.i.i

if.then51.i.i.i:                                  ; preds = %if.end44.i.i.i
  %sub52.i.i.i = fsub float %sub32.i.i.i, %mul48.i.i.i
  br label %redefine_logf.exit.i.i

if.else53.i.i.i:                                  ; preds = %if.end44.i.i.i
  %conv54.i.i.i = sitofp i32 %add31.i.i.i to float
  %mul55.i.i.i = fmul float %conv54.i.i.i, 0x3FE62E3000000000
  %mul56.i.i.i = fmul float %conv54.i.i.i, 0x3EE2FEFA20000000
  %sub57.i.i.i = fsub float %mul48.i.i.i, %mul56.i.i.i
  %sub58.i.i.i = fsub float %sub57.i.i.i, %sub32.i.i.i
  %sub59.i.i.i = fsub float %mul55.i.i.i, %sub58.i.i.i
  br label %redefine_logf.exit.i.i

if.end60.i.i.i:                                   ; preds = %if.end20.i.i.i
  %add61.i.i.i = fadd float %sub32.i.i.i, 2.000000e+00
  %mul63.i.i.i = fmul float %add61.i.i.i, %add61.i.i.i
  %89 = bitcast float %mul63.i.i.i to i32
  %mul.i118.i.i.i = fmul float %mul63.i.i.i, 5.000000e-01
  %shr.i119.i.i.i = ashr i32 %89, 1
  %sub.i120.i.i.i = sub nsw i32 1597463007, %shr.i119.i.i.i
  %90 = bitcast i32 %sub.i120.i.i.i to float
  %mul1.i121.i.i.i = fmul float %mul.i118.i.i.i, %90
  %mul2.i122.i.i.i = fmul float %90, %mul1.i121.i.i.i
  %sub3.i123.i.i.i = fsub float 1.500000e+00, %mul2.i122.i.i.i
  %mul4.i124.i.i.i = fmul float %90, %sub3.i123.i.i.i
  %mul65.i.i.i = fmul float %sub32.i.i.i, %mul4.i124.i.i.i
  %conv66.i.i.i = sitofp i32 %add31.i.i.i to float
  %mul67.i.i.i = fmul float %mul65.i.i.i, %mul65.i.i.i
  %sub68.i.i.i = add nsw i32 %87, -3187664
  %mul69.i.i.i = fmul float %mul67.i.i.i, %mul67.i.i.i
  %sub70.i.i.i = sub nsw i32 3523208, %87
  %mul71.i.i.i = fmul float %mul69.i.i.i, 0x3FCF13C4C0000000
  %add72.i.i.i = fadd float %mul71.i.i.i, 0x3FD999C260000000
  %mul73.i.i.i = fmul float %mul69.i.i.i, %add72.i.i.i
  %mul74.i.i.i = fmul float %mul69.i.i.i, 0x3FD23D3DC0000000
  %add75.i.i.i = fadd float %mul74.i.i.i, 0x3FE5555540000000
  %mul76.i.i.i = fmul float %mul67.i.i.i, %add75.i.i.i
  %or77.i.i.i = or i32 %sub68.i.i.i, %sub70.i.i.i
  %add78.i.i.i = fadd float %mul76.i.i.i, %mul73.i.i.i
  %cmp79.i.i.i = icmp sgt i32 %or77.i.i.i, 0
  br i1 %cmp79.i.i.i, label %if.then81.i.i.i, label %if.else100.i.i.i

if.then81.i.i.i:                                  ; preds = %if.end60.i.i.i
  %mul82.i.i.i = fmul float %sub32.i.i.i, 5.000000e-01
  %mul83.i.i.i = fmul float %sub32.i.i.i, %mul82.i.i.i
  %cmp84.i.i.i = icmp eq i32 %add31.i.i.i, 0
  br i1 %cmp84.i.i.i, label %if.then86.i.i.i, label %if.else91.i.i.i

if.then86.i.i.i:                                  ; preds = %if.then81.i.i.i
  %add87.i.i.i = fadd float %mul83.i.i.i, %add78.i.i.i
  %mul88.i.i.i = fmul float %mul65.i.i.i, %add87.i.i.i
  %sub89.i.i.i = fsub float %mul83.i.i.i, %mul88.i.i.i
  %sub90.i.i.i = fsub float %sub32.i.i.i, %sub89.i.i.i
  br label %redefine_logf.exit.i.i

if.else91.i.i.i:                                  ; preds = %if.then81.i.i.i
  %mul92.i.i.i = fmul float %conv66.i.i.i, 0x3FE62E3000000000
  %add93.i.i.i = fadd float %mul83.i.i.i, %add78.i.i.i
  %mul94.i.i.i = fmul float %mul65.i.i.i, %add93.i.i.i
  %mul95.i.i.i = fmul float %conv66.i.i.i, 0x3EE2FEFA20000000
  %add96.i.i.i = fadd float %mul95.i.i.i, %mul94.i.i.i
  %sub97.i.i.i = fsub float %mul83.i.i.i, %add96.i.i.i
  %sub98.i.i.i = fsub float %sub97.i.i.i, %sub32.i.i.i
  %sub99.i.i.i = fsub float %mul92.i.i.i, %sub98.i.i.i
  br label %redefine_logf.exit.i.i

if.else100.i.i.i:                                 ; preds = %if.end60.i.i.i
  %cmp101.i.i.i = icmp eq i32 %add31.i.i.i, 0
  br i1 %cmp101.i.i.i, label %if.then103.i.i.i, label %if.else107.i.i.i

if.then103.i.i.i:                                 ; preds = %if.else100.i.i.i
  %sub104.i.i.i = fsub float %sub32.i.i.i, %add78.i.i.i
  %mul105.i.i.i = fmul float %mul65.i.i.i, %sub104.i.i.i
  %sub106.i.i.i = fsub float %sub32.i.i.i, %mul105.i.i.i
  br label %redefine_logf.exit.i.i

if.else107.i.i.i:                                 ; preds = %if.else100.i.i.i
  %mul108.i.i.i = fmul float %conv66.i.i.i, 0x3FE62E3000000000
  %sub109.i.i.i = fsub float %sub32.i.i.i, %add78.i.i.i
  %mul110.i.i.i = fmul float %mul65.i.i.i, %sub109.i.i.i
  %mul111.i.i.i = fmul float %conv66.i.i.i, 0x3EE2FEFA20000000
  %sub112.i.i.i = fsub float %mul110.i.i.i, %mul111.i.i.i
  %sub113.i.i.i = fsub float %sub112.i.i.i, %sub32.i.i.i
  %sub114.i.i.i = fsub float %mul108.i.i.i, %sub113.i.i.i
  br label %redefine_logf.exit.i.i

redefine_logf.exit.i.i:                           ; preds = %if.else107.i.i.i, %if.then103.i.i.i, %if.else91.i.i.i, %if.then86.i.i.i, %if.else53.i.i.i, %if.then51.i.i.i, %if.else.i72.i.i, %if.then38.i.i.i, %if.then19.i.i.i, %if.then4.i67.i.i, %if.then.i54.i.i
  %retval.i44.i.0.i = phi float [ %mul8.i.i.i, %if.then4.i67.i.i ], [ %add.i69.i.i, %if.then19.i.i.i ], [ %add43.i.i.i, %if.else.i72.i.i ], [ %sub52.i.i.i, %if.then51.i.i.i ], [ %sub59.i.i.i, %if.else53.i.i.i ], [ %sub90.i.i.i, %if.then86.i.i.i ], [ %sub99.i.i.i, %if.else91.i.i.i ], [ %sub106.i.i.i, %if.then103.i.i.i ], [ %sub114.i.i.i, %if.else107.i.i.i ], [ 0xC581306CE0000000, %if.then.i54.i.i ], [ 0.000000e+00, %if.then38.i.i.i ]
  %mul.i27.i.i = fmul float %retval.i44.i.0.i, %retval.i44.i.0.i
  %91 = bitcast float %mul.i27.i.i to i32
  %mul.i.i28.i.i = fmul float %mul.i27.i.i, 5.000000e-01
  %shr.i.i29.i.i = ashr i32 %91, 1
  %sub.i.i30.i.i = sub nsw i32 1597463007, %shr.i.i29.i.i
  %92 = bitcast i32 %sub.i.i30.i.i to float
  %mul1.i.i31.i.i = fmul float %mul.i.i28.i.i, %92
  %mul2.i.i32.i.i = fmul float %92, %mul1.i.i31.i.i
  %sub3.i.i33.i.i = fsub float 1.500000e+00, %mul2.i.i32.i.i
  %mul4.i.i34.i.i = fmul float %92, %sub3.i.i33.i.i
  %cmp.i35.i.i = fcmp olt float %retval.i44.i.0.i, 0.000000e+00
  br i1 %cmp.i35.i.i, label %if.then.i37.i.i, label %redefine_div.exit40.i.i

if.then.i37.i.i:                                  ; preds = %redefine_logf.exit.i.i
  %sub.i36.i.i = fsub float 0.000000e+00, %mul4.i.i34.i.i
  br label %redefine_div.exit40.i.i

redefine_div.exit40.i.i:                          ; preds = %if.then.i37.i.i, %redefine_logf.exit.i.i
  %inv.i26.i.0.i = phi float [ %sub.i36.i.i, %if.then.i37.i.i ], [ %mul4.i.i34.i.i, %redefine_logf.exit.i.i ]
  %mul1.i38.i.i = fmul float %retval.i85.i.0.i, %inv.i26.i.0.i
  %93 = bitcast float %mul1.i38.i.i to i32
  %shr.i.i.i16 = lshr i32 %93, 23
  %and.i.i.i = and i32 %shr.i.i.i16, 255
  %sub.i18.i.i = add nsw i32 %and.i.i.i, -127
  %cmp.i19.i.i = icmp slt i32 %sub.i18.i.i, 23
  br i1 %cmp.i19.i.i, label %if.then.i20.i.i, label %if.else29.i.i.i

if.then.i20.i.i:                                  ; preds = %redefine_div.exit40.i.i
  %cmp1.i.i.i = icmp slt i32 %sub.i18.i.i, 0
  br i1 %cmp1.i.i.i, label %if.then2.i.i.i, label %if.else12.i.i.i

if.then2.i.i.i:                                   ; preds = %if.then.i20.i.i
  %add.i.i.i = fadd float %mul1.i38.i.i, 0x46293E5940000000
  %cmp3.i.i.i = fcmp ogt float %add.i.i.i, 0.000000e+00
  br i1 %cmp3.i.i.i, label %if.then4.i.i.i, label %if.end28.i.i.i

if.then4.i.i.i:                                   ; preds = %if.then2.i.i.i
  %cmp5.i.i.i = icmp sgt i32 %93, -1
  br i1 %cmp5.i.i.i, label %if.end28.i.i.i, label %if.else.i.i.i

if.else.i.i.i:                                    ; preds = %if.then4.i.i.i
  %and7.i.i.i = and i32 %93, 2147483647
  %cmp8.i.i.i = icmp eq i32 %and7.i.i.i, 0
  %. = select i1 %cmp8.i.i.i, i32 %93, i32 -1082130432
  br label %if.end28.i.i.i

if.else12.i.i.i:                                  ; preds = %if.then.i20.i.i
  %shr13.i.i.i = lshr i32 8388607, %sub.i18.i.i
  %and14.i.i.i = and i32 %shr13.i.i.i, %93
  %cmp15.i.i.i = icmp eq i32 %and14.i.i.i, 0
  br i1 %cmp15.i.i.i, label %redefine_floorf.exit.i.i, label %if.end17.i.i.i

if.end17.i.i.i:                                   ; preds = %if.else12.i.i.i
  %add18.i.i.i = fadd float %mul1.i38.i.i, 0x46293E5940000000
  %cmp19.i.i.i = fcmp ogt float %add18.i.i.i, 0.000000e+00
  br i1 %cmp19.i.i.i, label %if.then20.i.i.i, label %if.end28.i.i.i

if.then20.i.i.i:                                  ; preds = %if.end17.i.i.i
  %cmp21.i.i.i = icmp slt i32 %93, 0
  br i1 %cmp21.i.i.i, label %if.then22.i.i.i, label %if.end25.i.i.i

if.then22.i.i.i:                                  ; preds = %if.then20.i.i.i
  %shr23.i.i.i = lshr i32 8388608, %sub.i18.i.i
  %add24.i.i.i = add nsw i32 %shr23.i.i.i, %93
  br label %if.end25.i.i.i

if.end25.i.i.i:                                   ; preds = %if.then22.i.i.i, %if.then20.i.i.i
  %i0.i.i.0.i = phi i32 [ %add24.i.i.i, %if.then22.i.i.i ], [ %93, %if.then20.i.i.i ]
  %neg.i.i.i = xor i32 %shr13.i.i.i, -1
  %and26.i.i.i = and i32 %i0.i.i.0.i, %neg.i.i.i
  br label %if.end28.i.i.i

if.end28.i.i.i:                                   ; preds = %if.end25.i.i.i, %if.end17.i.i.i, %if.else.i.i.i, %if.then4.i.i.i, %if.then2.i.i.i
  %i0.i.i.1.i = phi i32 [ %93, %if.then2.i.i.i ], [ %and26.i.i.i, %if.end25.i.i.i ], [ %93, %if.end17.i.i.i ], [ 0, %if.then4.i.i.i ], [ %., %if.else.i.i.i ]
  %94 = bitcast i32 %i0.i.i.1.i to float
  br label %redefine_floorf.exit.i.i

if.else29.i.i.i:                                  ; preds = %redefine_div.exit40.i.i
  %cmp30.i.i.i = icmp eq i32 %sub.i18.i.i, 128
  br i1 %cmp30.i.i.i, label %if.then31.i.i.i, label %redefine_floorf.exit.i.i

if.then31.i.i.i:                                  ; preds = %if.else29.i.i.i
  %add32.i.i.i = fadd float %mul1.i38.i.i, %mul1.i38.i.i
  br label %redefine_floorf.exit.i.i

redefine_floorf.exit.i.i:                         ; preds = %if.then31.i.i.i, %if.else29.i.i.i, %if.end28.i.i.i, %if.else12.i.i.i
  %retval.i.i.0.i = phi float [ %94, %if.end28.i.i.i ], [ %add32.i.i.i, %if.then31.i.i.i ], [ %mul1.i38.i.i, %if.else12.i.i.i ], [ %mul1.i38.i.i, %if.else29.i.i.i ]
  %cmp13.i.i = icmp slt i32 %7, 5
  br i1 %cmp13.i.i, label %if.then15.i.i, label %uts_numChildren.exit

if.then15.i.i:                                    ; preds = %redefine_floorf.exit.i.i
  %conv12.i.i = fptosi float %retval.i.i.0.i to i32
  br label %uts_numChildren.exit

uts_numChildren.exit:                             ; preds = %if.then15.i.i, %redefine_floorf.exit.i.i
  %retval.i.0.i = phi i32 [ %conv12.i.i, %if.then15.i.i ], [ 0, %redefine_floorf.exit.i.i ]
  %95 = icmp sgt i32 %retval.i.0.i, 0
  %retval.i.0.i. = select i1 %95, i32 %retval.i.0.i, i32 0
  call void @llvm.writecm(i32 320, i32 60, i32 1)
  %96 = add i32 %retval.i.0.i., -1
  %falloc_reg = call i32 @llvm.falloc(i32 %96)
  %97 = shl nuw i32 %96, 6
  %98 = add nuw i32 %97, %falloc_reg
  br label %uts_function_for.body811111_create_body

uts_function_for.body811111_create_body:          ; preds = %uts_function_for.body811111_create_body, %uts_numChildren.exit
  %99 = phi i32 [ %falloc_reg, %uts_numChildren.exit ], [ %100, %uts_function_for.body811111_create_body ]
  call void @llvm.fbind(i32 11, i32 %99)
  %100 = add nuw i32 %99, 64
  %cmpinst = icmp ugt i32 %100, %98
  br i1 %cmpinst, label %uts_function_for.body811111_create_end, label %uts_function_for.body811111_create_body

uts_function_for.body811111_create_end:           ; preds = %uts_function_for.body811111_create_body
  call void @llvm.writecm(i32 192, i32 8, i32 %retval.i.0.i)
  call void @llvm.writecm(i32 192, i32 4, i32 %falloc_reg)
  call void @llvm.writecm(i32 192, i32 0, i32 %retval.i.0.i.)
  %101 = zext i1 %95 to i32
  call void @llvm.writecmp(i32 192, i32 %101)
  call void @llvm.writecm(i32 128, i32 0, i32 64)
  %invertedPred = xor i32 %101, 1
  call void @llvm.writecmp(i32 128, i32 %invertedPred)
  %102 = bitcast %struct.node_t* %2 to i8*
  %103 = bitcast %struct.node_t* %72 to i8*
  br label %uts_function_for.body811111_create_body2

uts_function_for.body811111_create_body2:         ; preds = %redefine_memcpy.exit7, %uts_function_for.body811111_create_end
  %104 = phi i32 [ %falloc_reg, %uts_function_for.body811111_create_end ], [ %107, %redefine_memcpy.exit7 ]
  %memframe_reg = call i8* @llvm.getmemframe(i32 %104)
  %ptr_val = getelementptr i8* %memframe_reg, i32 32
  br label %for.body.i

for.body.i:                                       ; preds = %for.body.i, %uts_function_for.body811111_create_body2
  %indvars.iv.i = phi i32 [ %indvars.iv.next.i, %for.body.i ], [ 0, %uts_function_for.body811111_create_body2 ]
  %arrayidx.i = getelementptr inbounds i8* %102, i32 %indvars.iv.i
  %105 = load i8* %arrayidx.i, align 1
  %arrayidx2.i = getelementptr inbounds i8* %ptr_val, i32 %indvars.iv.i
  store i8 %105, i8* %arrayidx2.i, align 1
  %indvars.iv.next.i = add i32 %indvars.iv.i, 1
  %exitcond.i = icmp eq i32 %indvars.iv.next.i, 32
  br i1 %exitcond.i, label %redefine_memcpy.exit, label %for.body.i

redefine_memcpy.exit:                             ; preds = %for.body.i
  br label %for.body.i6

for.body.i6:                                      ; preds = %for.body.i6, %redefine_memcpy.exit
  %indvars.iv.i1 = phi i32 [ %indvars.iv.next.i4, %for.body.i6 ], [ 0, %redefine_memcpy.exit ]
  %arrayidx.i2 = getelementptr inbounds i8* %103, i32 %indvars.iv.i1
  %106 = load i8* %arrayidx.i2, align 1
  %arrayidx2.i3 = getelementptr inbounds i8* %memframe_reg, i32 %indvars.iv.i1
  store i8 %106, i8* %arrayidx2.i3, align 1
  %indvars.iv.next.i4 = add i32 %indvars.iv.i1, 1
  %exitcond.i5 = icmp eq i32 %indvars.iv.next.i4, 32
  br i1 %exitcond.i5, label %redefine_memcpy.exit7, label %for.body.i6

redefine_memcpy.exit7:                            ; preds = %for.body.i6
  call void @llvm.writecm(i32 %104, i32 16, i32 1)
  call void @llvm.writecm(i32 %104, i32 8, i32 %retval.i.0.i.)
  call void @llvm.writecm(i32 %104, i32 0, i32 %falloc_reg)
  call void @llvm.writecm(i32 %104, i32 4, i32 320)
  %107 = add nuw i32 %104, 64
  %cmpinst4 = icmp ugt i32 %107, %98
  br i1 %cmpinst4, label %uts_function_for.body811111_create_end3, label %uts_function_for.body811111_create_body2

uts_function_for.body811111_create_end3:          ; preds = %redefine_memcpy.exit7
  call void @llvm.writecm(i32 320, i32 4, i32 %retval.i.0.i.)
  call void @llvm.writecm(i32 320, i32 0, i32 64)
  call void @llvm.writecm(i32 64, i32 12, i32 128)
  call void @llvm.writecm(i32 64, i32 8, i32 192)
  call void @llvm.writecm(i32 64, i32 0, i32 %retval.i.0.i.)
  call void @llvm.writecm(i32 64, i32 4, i32 %falloc_reg)
  call void @llvm.fdelete(i32 %0)
  ret void
}

; Function Attrs: nounwind
define void @uts_function_for.body71111(i32 inreg, i32 inreg, i32 inreg, i32* nocapture) #1 {
entry:
  %4 = icmp sgt i32 %2, 0
  br i1 %4, label %for.body, label %for.end

for.body:                                         ; preds = %for.body, %entry
  %.09 = phi i32 [ %7, %for.body ], [ 0, %entry ]
  %.078 = phi i32 [ %8, %for.body ], [ 0, %entry ]
  %5 = getelementptr inbounds i32* %3, i32 %.078
  %6 = load i32* %5, align 4
  %7 = add nsw i32 %6, %.09
  %8 = add nsw i32 %.078, 1
  %exitcond = icmp eq i32 %8, %2
  br i1 %exitcond, label %for.end, label %for.body

for.end:                                          ; preds = %for.body, %entry
  %.0.lcssa = phi i32 [ 0, %entry ], [ %7, %for.body ]
  tail call void @llvm.writecm(i32 %1, i32 16, i32 %.0.lcssa)
  tail call void @llvm.fdelete(i32 %0)
  ret void
}

; Function Attrs: alwaysinline nounwind ssp
define void @uts_function_for.body811111(i32 inreg, i32 inreg, i32 inreg, i32 inreg, i32 inreg, i32 inreg, %struct.node_t*, %struct.node_t* nocapture) #0 {
for.body54.i.i.lr.ph:
  %ctx.i = alloca %struct.sha1_ctx_s, align 4
  %bytes.i = alloca [4 x i8], align 1
  %8 = alloca %struct.node_t, align 8
  %9 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 0
  store i32 %5, i32* %9, align 4
  %10 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 1
  %11 = load i32* %10, align 4
  %12 = add nsw i32 %11, 1
  %13 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 1
  store i32 %12, i32* %13, align 4
  %14 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 2
  store i32 -1, i32* %14, align 4
  %15 = getelementptr inbounds [4 x i8]* %bytes.i, i32 0, i32 0
  %shr.i1 = lshr i32 %4, 24
  %conv.i = trunc i32 %shr.i1 to i8
  store i8 %conv.i, i8* %15, align 1
  %shr1.i2 = lshr i32 %4, 16
  %conv3.i = trunc i32 %shr1.i2 to i8
  %arrayidx4.i = getelementptr inbounds [4 x i8]* %bytes.i, i32 0, i32 1
  store i8 %conv3.i, i8* %arrayidx4.i, align 1
  %shr5.i3 = lshr i32 %4, 8
  %conv7.i = trunc i32 %shr5.i3 to i8
  %arrayidx8.i = getelementptr inbounds [4 x i8]* %bytes.i, i32 0, i32 2
  store i8 %conv7.i, i8* %arrayidx8.i, align 1
  %conv10.i = trunc i32 %4 to i8
  %arrayidx11.i = getelementptr inbounds [4 x i8]* %bytes.i, i32 0, i32 3
  store i8 %conv10.i, i8* %arrayidx11.i, align 1
  %arrayidx.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 0, i32 1
  store i32 0, i32* %arrayidx.i.i, align 4
  %arrayidx2.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 0, i32 0
  %arrayidx3.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 1, i32 0
  store i32 1732584193, i32* %arrayidx3.i.i, align 4
  %arrayidx5.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 1, i32 1
  store i32 -271733879, i32* %arrayidx5.i.i, align 4
  %arrayidx7.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 1, i32 2
  store i32 -1732584194, i32* %arrayidx7.i.i, align 4
  %arrayidx9.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 1, i32 3
  store i32 271733878, i32* %arrayidx9.i.i, align 4
  %arrayidx11.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 1, i32 4
  store i32 -1009589776, i32* %arrayidx11.i.i, align 4
  %arraydecay60.i2562.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 0
  %16 = bitcast i32* %arraydecay60.i2562.i to i8*
  %arrayidx56.i2558.i = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 0
  %17 = load i8* %arrayidx56.i2558.i, align 1
  store i8 %17, i8* %16, align 4
  %arrayidx56.i2558.i.1 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 1
  %18 = load i8* %arrayidx56.i2558.i.1, align 1
  %arrayidx61.i2563.i.1 = getelementptr inbounds i8* %16, i32 1
  store i8 %18, i8* %arrayidx61.i2563.i.1, align 1
  %arrayidx56.i2558.i.2 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 2
  %19 = load i8* %arrayidx56.i2558.i.2, align 1
  %arrayidx61.i2563.i.2 = getelementptr inbounds i8* %16, i32 2
  store i8 %19, i8* %arrayidx61.i2563.i.2, align 2
  %arrayidx56.i2558.i.3 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 3
  %20 = load i8* %arrayidx56.i2558.i.3, align 1
  %arrayidx61.i2563.i.3 = getelementptr inbounds i8* %16, i32 3
  store i8 %20, i8* %arrayidx61.i2563.i.3, align 1
  %arrayidx56.i2558.i.4 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 4
  %21 = load i8* %arrayidx56.i2558.i.4, align 1
  %arrayidx61.i2563.i.453 = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 1
  %arrayidx61.i2563.i.4 = bitcast i32* %arrayidx61.i2563.i.453 to i8*
  store i8 %21, i8* %arrayidx61.i2563.i.4, align 4
  %arrayidx56.i2558.i.5 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 5
  %22 = load i8* %arrayidx56.i2558.i.5, align 1
  %arrayidx61.i2563.i.5 = getelementptr inbounds i8* %16, i32 5
  store i8 %22, i8* %arrayidx61.i2563.i.5, align 1
  %arrayidx56.i2558.i.6 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 6
  %23 = load i8* %arrayidx56.i2558.i.6, align 1
  %arrayidx61.i2563.i.6 = getelementptr inbounds i8* %16, i32 6
  store i8 %23, i8* %arrayidx61.i2563.i.6, align 2
  %arrayidx56.i2558.i.7 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 7
  %24 = load i8* %arrayidx56.i2558.i.7, align 1
  %arrayidx61.i2563.i.7 = getelementptr inbounds i8* %16, i32 7
  store i8 %24, i8* %arrayidx61.i2563.i.7, align 1
  %arrayidx56.i2558.i.8 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 8
  %25 = load i8* %arrayidx56.i2558.i.8, align 1
  %arrayidx61.i2563.i.854 = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 2
  %arrayidx61.i2563.i.8 = bitcast i32* %arrayidx61.i2563.i.854 to i8*
  store i8 %25, i8* %arrayidx61.i2563.i.8, align 4
  %arrayidx56.i2558.i.9 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 9
  %26 = load i8* %arrayidx56.i2558.i.9, align 1
  %arrayidx61.i2563.i.9 = getelementptr inbounds i8* %16, i32 9
  store i8 %26, i8* %arrayidx61.i2563.i.9, align 1
  %arrayidx56.i2558.i.10 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 10
  %27 = load i8* %arrayidx56.i2558.i.10, align 1
  %arrayidx61.i2563.i.10 = getelementptr inbounds i8* %16, i32 10
  store i8 %27, i8* %arrayidx61.i2563.i.10, align 2
  %arrayidx56.i2558.i.11 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 11
  %28 = load i8* %arrayidx56.i2558.i.11, align 1
  %arrayidx61.i2563.i.11 = getelementptr inbounds i8* %16, i32 11
  store i8 %28, i8* %arrayidx61.i2563.i.11, align 1
  %arrayidx56.i2558.i.12 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 12
  %29 = load i8* %arrayidx56.i2558.i.12, align 1
  %arrayidx61.i2563.i.1255 = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 3
  %arrayidx61.i2563.i.12 = bitcast i32* %arrayidx61.i2563.i.1255 to i8*
  store i8 %29, i8* %arrayidx61.i2563.i.12, align 4
  %arrayidx56.i2558.i.13 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 13
  %30 = load i8* %arrayidx56.i2558.i.13, align 1
  %arrayidx61.i2563.i.13 = getelementptr inbounds i8* %16, i32 13
  store i8 %30, i8* %arrayidx61.i2563.i.13, align 1
  %arrayidx56.i2558.i.14 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 14
  %31 = load i8* %arrayidx56.i2558.i.14, align 1
  %arrayidx61.i2563.i.14 = getelementptr inbounds i8* %16, i32 14
  store i8 %31, i8* %arrayidx61.i2563.i.14, align 2
  %arrayidx56.i2558.i.15 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 15
  %32 = load i8* %arrayidx56.i2558.i.15, align 1
  %arrayidx61.i2563.i.15 = getelementptr inbounds i8* %16, i32 15
  store i8 %32, i8* %arrayidx61.i2563.i.15, align 1
  %arrayidx56.i2558.i.16 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 16
  %33 = load i8* %arrayidx56.i2558.i.16, align 1
  %arrayidx61.i2563.i.1656 = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 4
  %arrayidx61.i2563.i.16 = bitcast i32* %arrayidx61.i2563.i.1656 to i8*
  store i8 %33, i8* %arrayidx61.i2563.i.16, align 4
  %arrayidx56.i2558.i.17 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 17
  %34 = load i8* %arrayidx56.i2558.i.17, align 1
  %arrayidx61.i2563.i.17 = getelementptr inbounds i8* %16, i32 17
  store i8 %34, i8* %arrayidx61.i2563.i.17, align 1
  %arrayidx56.i2558.i.18 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 18
  %35 = load i8* %arrayidx56.i2558.i.18, align 1
  %arrayidx61.i2563.i.18 = getelementptr inbounds i8* %16, i32 18
  store i8 %35, i8* %arrayidx61.i2563.i.18, align 2
  %arrayidx56.i2558.i.19 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 19
  %36 = load i8* %arrayidx56.i2558.i.19, align 1
  %arrayidx61.i2563.i.19 = getelementptr inbounds i8* %16, i32 19
  store i8 %36, i8* %arrayidx61.i2563.i.19, align 1
  store i32 24, i32* %arrayidx2.i.i, align 4
  br label %for.body54.i.i

for.body54.i.i:                                   ; preds = %for.body54.i.i, %for.body54.i.i.lr.ph
  %i49.i.0.i327 = phi i32 [ 0, %for.body54.i.i.lr.ph ], [ %inc63.i.i, %for.body54.i.i ]
  %arrayidx56.i1260.i = getelementptr inbounds [4 x i8]* %bytes.i, i32 0, i32 %i49.i.0.i327
  %37 = load i8* %arrayidx56.i1260.i, align 1
  %add57.i.i = add i32 %i49.i.0.i327, 20
  %arrayidx61.i.i = getelementptr inbounds i8* %16, i32 %add57.i.i
  store i8 %37, i8* %arrayidx61.i.i, align 1
  %inc63.i.i = add nsw i32 %i49.i.0.i327, 1
  %cmp52.i.i = icmp ult i32 %inc63.i.i, 4
  br i1 %cmp52.i.i, label %for.body54.i.i, label %sha1_hash.exit.i

sha1_hash.exit.i:                                 ; preds = %for.body54.i.i
  %38 = load i32* %arrayidx2.i.i, align 4
  %and.i.i = and i32 %38, 63
  %add.i.i = add i32 %and.i.i, 3
  %shr.i.i = lshr i32 %add.i.i, 2
  %tobool.i.i324 = icmp eq i32 %shr.i.i, 0
  br i1 %tobool.i.i324, label %while.end.i.i, label %while.body.i.i

while.body.i.i:                                   ; preds = %while.body.i.i, %sha1_hash.exit.i
  %dec.i.i325.in = phi i32 [ %dec.i.i325, %while.body.i.i ], [ %shr.i.i, %sha1_hash.exit.i ]
  %dec.i.i325 = add nsw i32 %dec.i.i325.in, -1
  %arrayidx1.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %dec.i.i325
  %39 = load i32* %arrayidx1.i.i, align 4
  %shr.i2 = ashr i32 %39, 24
  %shl.i3 = shl i32 %39, 8
  %or.i4 = or i32 %shr.i2, %shl.i3
  %and.i5 = and i32 %or.i4, 16711935
  %shr1.i6 = ashr i32 %39, 8
  %shl2.i7 = shl i32 %39, 24
  %shr1.i6.masked = and i32 %shr1.i6, -16711936
  %and4.i9 = or i32 %shr1.i6.masked, %shl2.i7
  %or5.i10 = or i32 %and4.i9, %and.i5
  store i32 %or5.i10, i32* %arrayidx1.i.i, align 4
  %tobool.i.i = icmp eq i32 %dec.i.i325, 0
  br i1 %tobool.i.i, label %while.end.i.i, label %while.body.i.i

while.end.i.i:                                    ; preds = %while.body.i.i, %sha1_hash.exit.i
  %neg.i.i = shl i32 %38, 3
  %and25.i.i = and i32 %neg.i.i, 24
  %mul.i.i = xor i32 %and25.i.i, 24
  %shl26.i.i = shl i32 -128, %mul.i.i
  %shr27.i.i = lshr i32 %and.i.i, 2
  %wbuf29.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2
  %arrayidx30.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %shr27.i.i
  %40 = load i32* %arrayidx30.i.i, align 4
  %and31.i.i = and i32 %40, %shl26.i.i
  %shl35.i.i = shl i32 128, %mul.i.i
  %or40.i.i = or i32 %and31.i.i, %shl35.i.i
  store i32 %or40.i.i, i32* %arrayidx30.i.i, align 4
  %cmp.i.i = icmp ugt i32 %and.i.i, 55
  br i1 %cmp.i.i, label %if.then.i.i, label %while.cond48.i.i.preheader

if.then.i.i:                                      ; preds = %while.end.i.i
  %cmp41.i.i = icmp ult i32 %and.i.i, 60
  br i1 %cmp41.i.i, label %if.then42.i.i, label %if.end.i.i

if.then42.i.i:                                    ; preds = %if.then.i.i
  %arrayidx44.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 15
  store i32 0, i32* %arrayidx44.i.i, align 4
  br label %if.end.i.i

if.end.i.i:                                       ; preds = %if.then42.i.i, %if.then.i.i
  %41 = load i32* %arrayidx3.i.i, align 4
  %42 = load i32* %arrayidx5.i.i, align 4
  %43 = load i32* %arrayidx7.i.i, align 4
  %44 = load i32* %arrayidx9.i.i, align 4
  %45 = load i32* %arrayidx11.i.i, align 4
  %shr.i.i.i307 = lshr i32 %41, 27
  %shl.i.i.i308 = shl i32 %41, 5
  %or.i.i.i309 = or i32 %shr.i.i.i307, %shl.i.i.i308
  %and.i.i.i310 = and i32 %43, %42
  %neg.i.i.i311 = xor i32 %42, -1
  %and9.i.i.i312 = and i32 %44, %neg.i.i.i311
  %xor.i.i.i313 = xor i32 %and9.i.i.i312, %and.i.i.i310
  %add.i.i.i314 = add i32 %or.i.i.i309, 1518500249
  %add10.i.i.i315 = add i32 %add.i.i.i314, %xor.i.i.i313
  br label %for.body.i.i.i

for.body.i.i.i:                                   ; preds = %for.body.i.i.i, %if.end.i.i
  %add10.i.i.i322 = phi i32 [ %add10.i.i.i315, %if.end.i.i ], [ %add10.i.i.i, %for.body.i.i.i ]
  %ii.i.i.0.i321 = phi i32 [ 0, %if.end.i.i ], [ %add85.i.i.i, %for.body.i.i.i ]
  %v4.i.i.0.i320 = phi i32 [ %45, %if.end.i.i ], [ %or50.i.i.i, %for.body.i.i.i ]
  %v3.i.i.0.i319 = phi i32 [ %44, %if.end.i.i ], [ %or67.i.i.i, %for.body.i.i.i ]
  %v2.i.i.0.i318 = phi i32 [ %43, %if.end.i.i ], [ %or84.i.i.i, %for.body.i.i.i ]
  %v1.i.i.0.i317 = phi i32 [ %42, %if.end.i.i ], [ %add64.i.i.i, %for.body.i.i.i ]
  %v0.i.i.0.i316 = phi i32 [ %41, %if.end.i.i ], [ %add81.i.i.i, %for.body.i.i.i ]
  %arrayidx11.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %ii.i.i.0.i321
  %46 = load i32* %arrayidx11.i.i.i, align 4
  %add12.i.i.i = add i32 %v4.i.i.0.i320, %add10.i.i.i322
  %add13.i.i.i = add i32 %add12.i.i.i, %46
  %shr14.i.i.i = lshr i32 %v1.i.i.0.i317, 2
  %shl15.i.i.i = shl i32 %v1.i.i.0.i317, 30
  %or16.i.i.i = or i32 %shr14.i.i.i, %shl15.i.i.i
  %shr17.i.i.i = lshr i32 %add13.i.i.i, 27
  %shl18.i.i.i = shl i32 %add13.i.i.i, 5
  %or19.i.i.i = or i32 %shr17.i.i.i, %shl18.i.i.i
  %and20.i.i.i = and i32 %or16.i.i.i, %v0.i.i.0.i316
  %neg21.i.i.i = xor i32 %v0.i.i.0.i316, -1
  %and22.i.i.i = and i32 %v2.i.i.0.i318, %neg21.i.i.i
  %xor23.i.i.i = xor i32 %and20.i.i.i, %and22.i.i.i
  %add26.i.i.i = add nsw i32 %ii.i.i.0.i321, 1
  %arrayidx28.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add26.i.i.i
  %47 = load i32* %arrayidx28.i.i.i, align 4
  %add24.i.i.i = add i32 %v3.i.i.0.i319, 1518500249
  %add25.i.i.i = add i32 %add24.i.i.i, %47
  %add29.i.i.i = add i32 %add25.i.i.i, %xor23.i.i.i
  %add30.i.i.i = add i32 %add29.i.i.i, %or19.i.i.i
  %shr31.i.i.i = lshr i32 %v0.i.i.0.i316, 2
  %shl32.i.i.i = shl i32 %v0.i.i.0.i316, 30
  %or33.i.i.i = or i32 %shr31.i.i.i, %shl32.i.i.i
  %shr34.i.i.i = lshr i32 %add30.i.i.i, 27
  %shl35.i.i.i = shl i32 %add30.i.i.i, 5
  %or36.i.i.i = or i32 %shr34.i.i.i, %shl35.i.i.i
  %and37.i.i.i = and i32 %add13.i.i.i, %or33.i.i.i
  %neg38.i.i.i = xor i32 %add13.i.i.i, -1
  %and39.i.i.i = and i32 %or16.i.i.i, %neg38.i.i.i
  %xor40.i.i.i = xor i32 %and37.i.i.i, %and39.i.i.i
  %add43.i.i.i = add nsw i32 %ii.i.i.0.i321, 2
  %arrayidx45.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add43.i.i.i
  %48 = load i32* %arrayidx45.i.i.i, align 4
  %add41.i.i.i = add i32 %v2.i.i.0.i318, 1518500249
  %add42.i.i.i = add i32 %add41.i.i.i, %48
  %add46.i.i.i = add i32 %add42.i.i.i, %xor40.i.i.i
  %add47.i.i.i = add i32 %add46.i.i.i, %or36.i.i.i
  %shr48.i.i.i = lshr i32 %add13.i.i.i, 2
  %shl49.i.i.i = shl i32 %add13.i.i.i, 30
  %or50.i.i.i = or i32 %shr48.i.i.i, %shl49.i.i.i
  %shr51.i.i.i = lshr i32 %add47.i.i.i, 27
  %shl52.i.i.i = shl i32 %add47.i.i.i, 5
  %or53.i.i.i = or i32 %shr51.i.i.i, %shl52.i.i.i
  %and54.i.i.i = and i32 %add30.i.i.i, %or50.i.i.i
  %neg55.i.i.i = xor i32 %add30.i.i.i, -1
  %and56.i.i.i = and i32 %or33.i.i.i, %neg55.i.i.i
  %xor57.i.i.i = xor i32 %and54.i.i.i, %and56.i.i.i
  %add60.i.i.i = add nsw i32 %ii.i.i.0.i321, 3
  %arrayidx62.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add60.i.i.i
  %49 = load i32* %arrayidx62.i.i.i, align 4
  %add58.i.i.i = add i32 %or16.i.i.i, 1518500249
  %add59.i.i.i = add i32 %add58.i.i.i, %49
  %add63.i.i.i = add i32 %add59.i.i.i, %xor57.i.i.i
  %add64.i.i.i = add i32 %add63.i.i.i, %or53.i.i.i
  %shr65.i.i.i = lshr i32 %add30.i.i.i, 2
  %shl66.i.i.i = shl i32 %add30.i.i.i, 30
  %or67.i.i.i = or i32 %shr65.i.i.i, %shl66.i.i.i
  %shr68.i.i.i = lshr i32 %add64.i.i.i, 27
  %shl69.i.i.i = shl i32 %add64.i.i.i, 5
  %or70.i.i.i = or i32 %shr68.i.i.i, %shl69.i.i.i
  %and71.i.i.i = and i32 %add47.i.i.i, %or67.i.i.i
  %neg72.i.i.i = xor i32 %add47.i.i.i, -1
  %and73.i.i.i = and i32 %or50.i.i.i, %neg72.i.i.i
  %xor74.i.i.i = xor i32 %and71.i.i.i, %and73.i.i.i
  %add77.i.i.i = add nsw i32 %ii.i.i.0.i321, 4
  %arrayidx79.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add77.i.i.i
  %50 = load i32* %arrayidx79.i.i.i, align 4
  %add75.i.i.i = add i32 %or33.i.i.i, 1518500249
  %add76.i.i.i = add i32 %add75.i.i.i, %50
  %add80.i.i.i = add i32 %add76.i.i.i, %xor74.i.i.i
  %add81.i.i.i = add i32 %add80.i.i.i, %or70.i.i.i
  %shr82.i.i.i = lshr i32 %add47.i.i.i, 2
  %shl83.i.i.i = shl i32 %add47.i.i.i, 30
  %or84.i.i.i = or i32 %shr82.i.i.i, %shl83.i.i.i
  %add85.i.i.i = add nsw i32 %ii.i.i.0.i321, 5
  %cmp.i.i.i = icmp slt i32 %add85.i.i.i, 15
  %shr.i.i.i = lshr i32 %add81.i.i.i, 27
  %shl.i.i.i = shl i32 %add81.i.i.i, 5
  %or.i.i.i = or i32 %shr.i.i.i, %shl.i.i.i
  %and.i.i.i = and i32 %add64.i.i.i, %or84.i.i.i
  %neg.i.i.i = xor i32 %add64.i.i.i, -1
  %and9.i.i.i = and i32 %or67.i.i.i, %neg.i.i.i
  %xor.i.i.i = xor i32 %and9.i.i.i, %and.i.i.i
  %add.i.i.i = add i32 %xor.i.i.i, 1518500249
  %add10.i.i.i = add i32 %add.i.i.i, %or.i.i.i
  br i1 %cmp.i.i.i, label %for.body.i.i.i, label %for.end.i.i.i

for.end.i.i.i:                                    ; preds = %for.body.i.i.i
  %arraydecay.i.i.i = getelementptr inbounds [16 x i32]* %wbuf29.i.i, i32 0, i32 0
  %arrayidx95.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 15
  %51 = load i32* %arrayidx95.i.i.i, align 4
  %add96.i.i.i = add i32 %add10.i.i.i, %or50.i.i.i
  %add97.i.i.i = add i32 %add96.i.i.i, %51
  %shr98.i.i.i = lshr i32 %add64.i.i.i, 2
  %shl99.i.i.i = shl i32 %add64.i.i.i, 30
  %or100.i.i.i = or i32 %shr98.i.i.i, %shl99.i.i.i
  %shr101.i.i.i = lshr i32 %add97.i.i.i, 27
  %shl102.i.i.i = shl i32 %add97.i.i.i, 5
  %or103.i.i.i = or i32 %shr101.i.i.i, %shl102.i.i.i
  %and104.i.i.i = and i32 %add81.i.i.i, %or100.i.i.i
  %neg105.i.i.i = xor i32 %add81.i.i.i, -1
  %and106.i.i.i = and i32 %or84.i.i.i, %neg105.i.i.i
  %xor107.i.i.i = xor i32 %and104.i.i.i, %and106.i.i.i
  %arrayidx110.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 13
  %52 = load i32* %arrayidx110.i.i.i, align 4
  %arrayidx111.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 8
  %53 = load i32* %arrayidx111.i.i.i, align 4
  %xor112.i.i.i = xor i32 %53, %52
  %arrayidx113.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 2
  %54 = load i32* %arrayidx113.i.i.i, align 4
  %xor114.i.i.i = xor i32 %xor112.i.i.i, %54
  %55 = load i32* %arraydecay.i.i.i, align 4
  %xor116.i.i.i = xor i32 %xor114.i.i.i, %55
  %shl117.i.i.i = shl i32 %xor116.i.i.i, 1
  %shr125.i.i.i = lshr i32 %xor116.i.i.i, 31
  %or126.i.i.i = or i32 %shl117.i.i.i, %shr125.i.i.i
  store i32 %or126.i.i.i, i32* %arraydecay.i.i.i, align 4
  %add108.i.i.i = add i32 %or67.i.i.i, 1518500249
  %add109.i.i.i = add i32 %add108.i.i.i, %xor107.i.i.i
  %add128.i.i.i = add i32 %add109.i.i.i, %or103.i.i.i
  %add129.i.i.i = add i32 %add128.i.i.i, %or126.i.i.i
  %shr130.i.i.i = lshr i32 %add81.i.i.i, 2
  %shl131.i.i.i = shl i32 %add81.i.i.i, 30
  %or132.i.i.i = or i32 %shr130.i.i.i, %shl131.i.i.i
  %shr133.i.i.i = lshr i32 %add129.i.i.i, 27
  %shl134.i.i.i = shl i32 %add129.i.i.i, 5
  %or135.i.i.i = or i32 %shr133.i.i.i, %shl134.i.i.i
  %and136.i.i.i = and i32 %add97.i.i.i, %or132.i.i.i
  %neg137.i.i.i = xor i32 %add97.i.i.i, -1
  %and138.i.i.i = and i32 %or100.i.i.i, %neg137.i.i.i
  %xor139.i.i.i = xor i32 %and136.i.i.i, %and138.i.i.i
  %arrayidx142.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 14
  %56 = load i32* %arrayidx142.i.i.i, align 4
  %arrayidx143.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 9
  %57 = load i32* %arrayidx143.i.i.i, align 4
  %xor144.i.i.i = xor i32 %57, %56
  %arrayidx145.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 3
  %58 = load i32* %arrayidx145.i.i.i, align 4
  %xor146.i.i.i = xor i32 %xor144.i.i.i, %58
  %arrayidx147.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 1
  %59 = load i32* %arrayidx147.i.i.i, align 4
  %xor148.i.i.i = xor i32 %xor146.i.i.i, %59
  %shl149.i.i.i = shl i32 %xor148.i.i.i, 1
  %shr157.i.i.i = lshr i32 %xor148.i.i.i, 31
  %or158.i.i.i = or i32 %shl149.i.i.i, %shr157.i.i.i
  store i32 %or158.i.i.i, i32* %arrayidx147.i.i.i, align 4
  %add140.i.i.i = add i32 %or84.i.i.i, 1518500249
  %add141.i.i.i = add i32 %add140.i.i.i, %xor139.i.i.i
  %add160.i.i.i = add i32 %add141.i.i.i, %or135.i.i.i
  %add161.i.i.i = add i32 %add160.i.i.i, %or158.i.i.i
  %shr162.i.i.i = lshr i32 %add97.i.i.i, 2
  %shl163.i.i.i = shl i32 %add97.i.i.i, 30
  %or164.i.i.i = or i32 %shr162.i.i.i, %shl163.i.i.i
  %shr165.i.i.i = lshr i32 %add161.i.i.i, 27
  %shl166.i.i.i = shl i32 %add161.i.i.i, 5
  %or167.i.i.i = or i32 %shr165.i.i.i, %shl166.i.i.i
  %and168.i.i.i = and i32 %add129.i.i.i, %or164.i.i.i
  %neg169.i.i.i = xor i32 %add129.i.i.i, -1
  %and170.i.i.i = and i32 %or132.i.i.i, %neg169.i.i.i
  %xor171.i.i.i = xor i32 %and168.i.i.i, %and170.i.i.i
  %arrayidx175.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 10
  %60 = load i32* %arrayidx175.i.i.i, align 4
  %arrayidx177.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 4
  %61 = load i32* %arrayidx177.i.i.i, align 4
  %xor176.i.i.i = xor i32 %54, %51
  %xor178.i.i.i = xor i32 %xor176.i.i.i, %60
  %xor180.i.i.i = xor i32 %xor178.i.i.i, %61
  %shl181.i.i.i = shl i32 %xor180.i.i.i, 1
  %shr189.i.i.i = lshr i32 %xor180.i.i.i, 31
  %or190.i.i.i = or i32 %shl181.i.i.i, %shr189.i.i.i
  store i32 %or190.i.i.i, i32* %arrayidx113.i.i.i, align 4
  %add172.i.i.i = add i32 %or100.i.i.i, 1518500249
  %add173.i.i.i = add i32 %add172.i.i.i, %xor171.i.i.i
  %add192.i.i.i = add i32 %add173.i.i.i, %or190.i.i.i
  %add193.i.i.i = add i32 %add192.i.i.i, %or167.i.i.i
  %shr194.i.i.i = lshr i32 %add129.i.i.i, 2
  %shl195.i.i.i = shl i32 %add129.i.i.i, 30
  %or196.i.i.i = or i32 %shr194.i.i.i, %shl195.i.i.i
  %shr197.i.i.i = lshr i32 %add193.i.i.i, 27
  %shl198.i.i.i = shl i32 %add193.i.i.i, 5
  %or199.i.i.i = or i32 %shr197.i.i.i, %shl198.i.i.i
  %and200.i.i.i = and i32 %add161.i.i.i, %or196.i.i.i
  %neg201.i.i.i = xor i32 %add161.i.i.i, -1
  %and202.i.i.i = and i32 %or164.i.i.i, %neg201.i.i.i
  %xor203.i.i.i = xor i32 %and200.i.i.i, %and202.i.i.i
  %arrayidx207.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 11
  %62 = load i32* %arrayidx207.i.i.i, align 4
  %arrayidx209.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 5
  %63 = load i32* %arrayidx209.i.i.i, align 4
  %xor208.i.i.i = xor i32 %58, %or126.i.i.i
  %xor210.i.i.i = xor i32 %xor208.i.i.i, %62
  %xor212.i.i.i = xor i32 %xor210.i.i.i, %63
  %shl213.i.i.i = shl i32 %xor212.i.i.i, 1
  %shr221.i.i.i = lshr i32 %xor212.i.i.i, 31
  %or222.i.i.i = or i32 %shl213.i.i.i, %shr221.i.i.i
  store i32 %or222.i.i.i, i32* %arrayidx145.i.i.i, align 4
  %add204.i.i.i = add i32 %or132.i.i.i, 1518500249
  %add205.i.i.i = add i32 %add204.i.i.i, %xor203.i.i.i
  %add224.i.i.i = add i32 %add205.i.i.i, %or222.i.i.i
  %add225.i.i.i = add i32 %add224.i.i.i, %or199.i.i.i
  %shr226.i.i.i = lshr i32 %add161.i.i.i, 2
  %shl227.i.i.i = shl i32 %add161.i.i.i, 30
  %or228.i.i.i = or i32 %shr226.i.i.i, %shl227.i.i.i
  br label %for.body232.i.i.i

for.body232.i.i.i:                                ; preds = %for.body232.i.i.i, %for.end.i.i.i
  %ii229.i.i.0.i306 = phi i32 [ 20, %for.end.i.i.i ], [ %add432.i.i.i, %for.body232.i.i.i ]
  %v4.i.i.1.i305 = phi i32 [ %or164.i.i.i, %for.end.i.i.i ], [ %or412.i.i.i, %for.body232.i.i.i ]
  %v3.i.i.1.i304 = phi i32 [ %or196.i.i.i, %for.end.i.i.i ], [ %or475.i.i.i, %for.body232.i.i.i ]
  %v2.i.i.1.i303 = phi i32 [ %or228.i.i.i, %for.end.i.i.i ], [ %or538.i.i.i, %for.body232.i.i.i ]
  %v1.i.i.1.i302 = phi i32 [ %add193.i.i.i, %for.end.i.i.i ], [ %add472.i.i.i, %for.body232.i.i.i ]
  %v0.i.i.1.i301 = phi i32 [ %add225.i.i.i, %for.end.i.i.i ], [ %add535.i.i.i, %for.body232.i.i.i ]
  %shr233.i.i.i = lshr i32 %v0.i.i.1.i301, 27
  %shl234.i.i.i = shl i32 %v0.i.i.1.i301, 5
  %or235.i.i.i = or i32 %shr233.i.i.i, %shl234.i.i.i
  %xor236.i.i.i = xor i32 %v2.i.i.1.i303, %v3.i.i.1.i304
  %xor237.i.i.i = xor i32 %xor236.i.i.i, %v1.i.i.1.i302
  %add240.i.i.i = add nsw i32 %ii229.i.i.0.i306, 13
  %and241.i.i.i = and i32 %add240.i.i.i, 15
  %arrayidx243.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and241.i.i.i
  %64 = load i32* %arrayidx243.i.i.i, align 4
  %add244.i.i.i = add nsw i32 %ii229.i.i.0.i306, 8
  %and245.i.i.i = and i32 %add244.i.i.i, 15
  %arrayidx247.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and245.i.i.i
  %65 = load i32* %arrayidx247.i.i.i, align 4
  %xor248.i.i.i = xor i32 %65, %64
  %add249.i.i.i = add nsw i32 %ii229.i.i.0.i306, 2
  %and250.i.i.i = and i32 %add249.i.i.i, 15
  %arrayidx252.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and250.i.i.i
  %66 = load i32* %arrayidx252.i.i.i, align 4
  %xor253.i.i.i = xor i32 %xor248.i.i.i, %66
  %and254.i.i.i = and i32 %ii229.i.i.0.i306, 15
  %arrayidx256.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and254.i.i.i
  %67 = load i32* %arrayidx256.i.i.i, align 4
  %xor257.i.i.i = xor i32 %xor253.i.i.i, %67
  %shl258.i.i.i = shl i32 %xor257.i.i.i, 1
  %shr277.i.i.i = lshr i32 %xor257.i.i.i, 31
  %or278.i.i.i = or i32 %shl258.i.i.i, %shr277.i.i.i
  store i32 %or278.i.i.i, i32* %arrayidx256.i.i.i, align 4
  %add238.i.i.i = add i32 %v4.i.i.1.i305, 1859775393
  %add239.i.i.i = add i32 %add238.i.i.i, %xor237.i.i.i
  %add282.i.i.i = add i32 %add239.i.i.i, %or235.i.i.i
  %add283.i.i.i = add i32 %add282.i.i.i, %or278.i.i.i
  %shr284.i.i.i = lshr i32 %v1.i.i.1.i302, 2
  %shl285.i.i.i = shl i32 %v1.i.i.1.i302, 30
  %or286.i.i.i = or i32 %shr284.i.i.i, %shl285.i.i.i
  %shr287.i.i.i = lshr i32 %add283.i.i.i, 27
  %shl288.i.i.i = shl i32 %add283.i.i.i, 5
  %or289.i.i.i = or i32 %shr287.i.i.i, %shl288.i.i.i
  %xor290.i.i.i = xor i32 %v0.i.i.1.i301, %v2.i.i.1.i303
  %xor291.i.i.i = xor i32 %xor290.i.i.i, %or286.i.i.i
  %add294.i.i.i = add nsw i32 %ii229.i.i.0.i306, 1
  %add295.i.i.i = add nsw i32 %ii229.i.i.0.i306, 14
  %and296.i.i.i = and i32 %add295.i.i.i, 15
  %arrayidx298.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and296.i.i.i
  %68 = load i32* %arrayidx298.i.i.i, align 4
  %add300.i.i.i = add nsw i32 %ii229.i.i.0.i306, 9
  %and301.i.i.i = and i32 %add300.i.i.i, 15
  %arrayidx303.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and301.i.i.i
  %69 = load i32* %arrayidx303.i.i.i, align 4
  %xor304.i.i.i = xor i32 %69, %68
  %add306.i.i.i = add nsw i32 %ii229.i.i.0.i306, 3
  %and307.i.i.i = and i32 %add306.i.i.i, 15
  %arrayidx309.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and307.i.i.i
  %70 = load i32* %arrayidx309.i.i.i, align 4
  %xor310.i.i.i = xor i32 %xor304.i.i.i, %70
  %and312.i.i.i = and i32 %add294.i.i.i, 15
  %arrayidx314.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and312.i.i.i
  %71 = load i32* %arrayidx314.i.i.i, align 4
  %xor315.i.i.i = xor i32 %xor310.i.i.i, %71
  %shl316.i.i.i = shl i32 %xor315.i.i.i, 1
  %shr339.i.i.i = lshr i32 %xor315.i.i.i, 31
  %or340.i.i.i = or i32 %shl316.i.i.i, %shr339.i.i.i
  store i32 %or340.i.i.i, i32* %arrayidx314.i.i.i, align 4
  %add292.i.i.i = add i32 %v3.i.i.1.i304, 1859775393
  %add293.i.i.i = add i32 %add292.i.i.i, %xor291.i.i.i
  %add345.i.i.i = add i32 %add293.i.i.i, %or289.i.i.i
  %add346.i.i.i = add i32 %add345.i.i.i, %or340.i.i.i
  %shr347.i.i.i = lshr i32 %v0.i.i.1.i301, 2
  %shl348.i.i.i = shl i32 %v0.i.i.1.i301, 30
  %or349.i.i.i = or i32 %shr347.i.i.i, %shl348.i.i.i
  %shr350.i.i.i = lshr i32 %add346.i.i.i, 27
  %shl351.i.i.i = shl i32 %add346.i.i.i, 5
  %or352.i.i.i = or i32 %shr350.i.i.i, %shl351.i.i.i
  %xor353.i.i.i = xor i32 %or349.i.i.i, %or286.i.i.i
  %xor354.i.i.i = xor i32 %xor353.i.i.i, %add283.i.i.i
  %add358.i.i.i = add nsw i32 %ii229.i.i.0.i306, 15
  %and359.i.i.i = and i32 %add358.i.i.i, 15
  %arrayidx361.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and359.i.i.i
  %72 = load i32* %arrayidx361.i.i.i, align 4
  %add363.i.i.i = add nsw i32 %ii229.i.i.0.i306, 10
  %and364.i.i.i = and i32 %add363.i.i.i, 15
  %arrayidx366.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and364.i.i.i
  %73 = load i32* %arrayidx366.i.i.i, align 4
  %xor367.i.i.i = xor i32 %73, %72
  %add369.i.i.i = add nsw i32 %ii229.i.i.0.i306, 4
  %and370.i.i.i = and i32 %add369.i.i.i, 15
  %arrayidx372.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and370.i.i.i
  %74 = load i32* %arrayidx372.i.i.i, align 4
  %xor373.i.i.i = xor i32 %xor367.i.i.i, %74
  %75 = load i32* %arrayidx252.i.i.i, align 4
  %xor378.i.i.i = xor i32 %xor373.i.i.i, %75
  %shl379.i.i.i = shl i32 %xor378.i.i.i, 1
  %shr402.i.i.i = lshr i32 %xor378.i.i.i, 31
  %or403.i.i.i = or i32 %shl379.i.i.i, %shr402.i.i.i
  store i32 %or403.i.i.i, i32* %arrayidx252.i.i.i, align 4
  %add355.i.i.i = add i32 %v2.i.i.1.i303, 1859775393
  %add356.i.i.i = add i32 %add355.i.i.i, %xor354.i.i.i
  %add408.i.i.i = add i32 %add356.i.i.i, %or352.i.i.i
  %add409.i.i.i = add i32 %add408.i.i.i, %or403.i.i.i
  %shr410.i.i.i = lshr i32 %add283.i.i.i, 2
  %shl411.i.i.i = shl i32 %add283.i.i.i, 30
  %or412.i.i.i = or i32 %shr410.i.i.i, %shl411.i.i.i
  %shr413.i.i.i = lshr i32 %add409.i.i.i, 27
  %shl414.i.i.i = shl i32 %add409.i.i.i, 5
  %or415.i.i.i = or i32 %shr413.i.i.i, %shl414.i.i.i
  %xor416.i.i.i = xor i32 %or412.i.i.i, %or349.i.i.i
  %xor417.i.i.i = xor i32 %xor416.i.i.i, %add346.i.i.i
  %76 = load i32* %arrayidx256.i.i.i, align 4
  %add426.i.i.i = add nsw i32 %ii229.i.i.0.i306, 11
  %and427.i.i.i = and i32 %add426.i.i.i, 15
  %arrayidx429.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and427.i.i.i
  %77 = load i32* %arrayidx429.i.i.i, align 4
  %xor430.i.i.i = xor i32 %77, %76
  %add432.i.i.i = add nsw i32 %ii229.i.i.0.i306, 5
  %and433.i.i.i = and i32 %add432.i.i.i, 15
  %arrayidx435.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and433.i.i.i
  %78 = load i32* %arrayidx435.i.i.i, align 4
  %xor436.i.i.i = xor i32 %xor430.i.i.i, %78
  %79 = load i32* %arrayidx309.i.i.i, align 4
  %xor441.i.i.i = xor i32 %xor436.i.i.i, %79
  %shl442.i.i.i = shl i32 %xor441.i.i.i, 1
  %shr465.i.i.i = lshr i32 %xor441.i.i.i, 31
  %or466.i.i.i = or i32 %shl442.i.i.i, %shr465.i.i.i
  store i32 %or466.i.i.i, i32* %arrayidx309.i.i.i, align 4
  %add418.i.i.i = add i32 %or286.i.i.i, 1859775393
  %add419.i.i.i = add i32 %add418.i.i.i, %xor417.i.i.i
  %add471.i.i.i = add i32 %add419.i.i.i, %or415.i.i.i
  %add472.i.i.i = add i32 %add471.i.i.i, %or466.i.i.i
  %shr473.i.i.i = lshr i32 %add346.i.i.i, 2
  %shl474.i.i.i = shl i32 %add346.i.i.i, 30
  %or475.i.i.i = or i32 %shr473.i.i.i, %shl474.i.i.i
  %shr476.i.i.i = lshr i32 %add472.i.i.i, 27
  %shl477.i.i.i = shl i32 %add472.i.i.i, 5
  %or478.i.i.i = or i32 %shr476.i.i.i, %shl477.i.i.i
  %xor479.i.i.i = xor i32 %or475.i.i.i, %or412.i.i.i
  %xor480.i.i.i = xor i32 %xor479.i.i.i, %add409.i.i.i
  %80 = load i32* %arrayidx314.i.i.i, align 4
  %add489.i.i.i = add nsw i32 %ii229.i.i.0.i306, 12
  %and490.i.i.i = and i32 %add489.i.i.i, 15
  %arrayidx492.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and490.i.i.i
  %81 = load i32* %arrayidx492.i.i.i, align 4
  %xor493.i.i.i = xor i32 %81, %80
  %add495.i.i.i = add nsw i32 %ii229.i.i.0.i306, 6
  %and496.i.i.i = and i32 %add495.i.i.i, 15
  %arrayidx498.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and496.i.i.i
  %82 = load i32* %arrayidx498.i.i.i, align 4
  %xor499.i.i.i = xor i32 %xor493.i.i.i, %82
  %83 = load i32* %arrayidx372.i.i.i, align 4
  %xor504.i.i.i = xor i32 %xor499.i.i.i, %83
  %shl505.i.i.i = shl i32 %xor504.i.i.i, 1
  %shr528.i.i.i = lshr i32 %xor504.i.i.i, 31
  %or529.i.i.i = or i32 %shl505.i.i.i, %shr528.i.i.i
  store i32 %or529.i.i.i, i32* %arrayidx372.i.i.i, align 4
  %add481.i.i.i = add i32 %or349.i.i.i, 1859775393
  %add482.i.i.i = add i32 %add481.i.i.i, %xor480.i.i.i
  %add534.i.i.i = add i32 %add482.i.i.i, %or478.i.i.i
  %add535.i.i.i = add i32 %add534.i.i.i, %or529.i.i.i
  %shr536.i.i.i = lshr i32 %add409.i.i.i, 2
  %shl537.i.i.i = shl i32 %add409.i.i.i, 30
  %or538.i.i.i = or i32 %shr536.i.i.i, %shl537.i.i.i
  %cmp231.i.i.i = icmp slt i32 %add432.i.i.i, 40
  br i1 %cmp231.i.i.i, label %for.body232.i.i.i, label %for.body545.i.i.i

for.body545.i.i.i:                                ; preds = %for.body545.i.i.i, %for.body232.i.i.i
  %ii542.i.i.0.i300 = phi i32 [ %add757.i.i.i, %for.body545.i.i.i ], [ 40, %for.body232.i.i.i ]
  %v4.i.i.2.i299 = phi i32 [ %or734.i.i.i, %for.body545.i.i.i ], [ %or412.i.i.i, %for.body232.i.i.i ]
  %v3.i.i.2.i298 = phi i32 [ %or800.i.i.i, %for.body545.i.i.i ], [ %or475.i.i.i, %for.body232.i.i.i ]
  %v2.i.i.2.i297 = phi i32 [ %or866.i.i.i, %for.body545.i.i.i ], [ %or538.i.i.i, %for.body232.i.i.i ]
  %v1.i.i.2.i296 = phi i32 [ %add797.i.i.i, %for.body545.i.i.i ], [ %add472.i.i.i, %for.body232.i.i.i ]
  %v0.i.i.2.i295 = phi i32 [ %add863.i.i.i, %for.body545.i.i.i ], [ %add535.i.i.i, %for.body232.i.i.i ]
  %shr546.i.i.i = lshr i32 %v0.i.i.2.i295, 27
  %shl547.i.i.i = shl i32 %v0.i.i.2.i295, 5
  %or548.i.i.i = or i32 %shr546.i.i.i, %shl547.i.i.i
  %and550.i.i.i85 = xor i32 %v2.i.i.2.i297, %v3.i.i.2.i298
  %xor551.i.i.i = and i32 %and550.i.i.i85, %v1.i.i.2.i296
  %and552.i.i.i = and i32 %v2.i.i.2.i297, %v3.i.i.2.i298
  %xor553.i.i.i = xor i32 %xor551.i.i.i, %and552.i.i.i
  %add556.i.i.i = add nsw i32 %ii542.i.i.0.i300, 13
  %and557.i.i.i = and i32 %add556.i.i.i, 15
  %arrayidx559.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and557.i.i.i
  %84 = load i32* %arrayidx559.i.i.i, align 4
  %add560.i.i.i = add nsw i32 %ii542.i.i.0.i300, 8
  %and561.i.i.i = and i32 %add560.i.i.i, 15
  %arrayidx563.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and561.i.i.i
  %85 = load i32* %arrayidx563.i.i.i, align 4
  %xor564.i.i.i = xor i32 %85, %84
  %add565.i.i.i = add nsw i32 %ii542.i.i.0.i300, 2
  %and566.i.i.i = and i32 %add565.i.i.i, 15
  %arrayidx568.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and566.i.i.i
  %86 = load i32* %arrayidx568.i.i.i, align 4
  %xor569.i.i.i = xor i32 %xor564.i.i.i, %86
  %and570.i.i.i = and i32 %ii542.i.i.0.i300, 15
  %arrayidx572.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and570.i.i.i
  %87 = load i32* %arrayidx572.i.i.i, align 4
  %xor573.i.i.i = xor i32 %xor569.i.i.i, %87
  %shl574.i.i.i = shl i32 %xor573.i.i.i, 1
  %shr593.i.i.i = lshr i32 %xor573.i.i.i, 31
  %or594.i.i.i = or i32 %shl574.i.i.i, %shr593.i.i.i
  store i32 %or594.i.i.i, i32* %arrayidx572.i.i.i, align 4
  %add554.i.i.i = add i32 %v4.i.i.2.i299, -1894007588
  %add555.i.i.i = add i32 %add554.i.i.i, %xor553.i.i.i
  %add598.i.i.i = add i32 %add555.i.i.i, %or548.i.i.i
  %add599.i.i.i = add i32 %add598.i.i.i, %or594.i.i.i
  %shr600.i.i.i = lshr i32 %v1.i.i.2.i296, 2
  %shl601.i.i.i = shl i32 %v1.i.i.2.i296, 30
  %or602.i.i.i = or i32 %shr600.i.i.i, %shl601.i.i.i
  %shr603.i.i.i = lshr i32 %add599.i.i.i, 27
  %shl604.i.i.i = shl i32 %add599.i.i.i, 5
  %or605.i.i.i = or i32 %shr603.i.i.i, %shl604.i.i.i
  %and607.i.i.i90 = xor i32 %or602.i.i.i, %v2.i.i.2.i297
  %xor608.i.i.i = and i32 %and607.i.i.i90, %v0.i.i.2.i295
  %and609.i.i.i = and i32 %or602.i.i.i, %v2.i.i.2.i297
  %xor610.i.i.i = xor i32 %xor608.i.i.i, %and609.i.i.i
  %add613.i.i.i = add nsw i32 %ii542.i.i.0.i300, 1
  %add614.i.i.i = add nsw i32 %ii542.i.i.0.i300, 14
  %and615.i.i.i = and i32 %add614.i.i.i, 15
  %arrayidx617.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and615.i.i.i
  %88 = load i32* %arrayidx617.i.i.i, align 4
  %add619.i.i.i = add nsw i32 %ii542.i.i.0.i300, 9
  %and620.i.i.i = and i32 %add619.i.i.i, 15
  %arrayidx622.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and620.i.i.i
  %89 = load i32* %arrayidx622.i.i.i, align 4
  %xor623.i.i.i = xor i32 %89, %88
  %add625.i.i.i = add nsw i32 %ii542.i.i.0.i300, 3
  %and626.i.i.i = and i32 %add625.i.i.i, 15
  %arrayidx628.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and626.i.i.i
  %90 = load i32* %arrayidx628.i.i.i, align 4
  %xor629.i.i.i = xor i32 %xor623.i.i.i, %90
  %and631.i.i.i = and i32 %add613.i.i.i, 15
  %arrayidx633.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and631.i.i.i
  %91 = load i32* %arrayidx633.i.i.i, align 4
  %xor634.i.i.i = xor i32 %xor629.i.i.i, %91
  %shl635.i.i.i = shl i32 %xor634.i.i.i, 1
  %shr658.i.i.i = lshr i32 %xor634.i.i.i, 31
  %or659.i.i.i = or i32 %shl635.i.i.i, %shr658.i.i.i
  store i32 %or659.i.i.i, i32* %arrayidx633.i.i.i, align 4
  %add611.i.i.i = add i32 %v3.i.i.2.i298, -1894007588
  %add612.i.i.i = add i32 %add611.i.i.i, %xor610.i.i.i
  %add664.i.i.i = add i32 %add612.i.i.i, %or605.i.i.i
  %add665.i.i.i = add i32 %add664.i.i.i, %or659.i.i.i
  %shr666.i.i.i = lshr i32 %v0.i.i.2.i295, 2
  %shl667.i.i.i = shl i32 %v0.i.i.2.i295, 30
  %or668.i.i.i = or i32 %shr666.i.i.i, %shl667.i.i.i
  %shr669.i.i.i = lshr i32 %add665.i.i.i, 27
  %shl670.i.i.i = shl i32 %add665.i.i.i, 5
  %or671.i.i.i = or i32 %shr669.i.i.i, %shl670.i.i.i
  %and673.i.i.i95 = xor i32 %or668.i.i.i, %or602.i.i.i
  %xor674.i.i.i = and i32 %add599.i.i.i, %and673.i.i.i95
  %and675.i.i.i = and i32 %or668.i.i.i, %or602.i.i.i
  %xor676.i.i.i = xor i32 %xor674.i.i.i, %and675.i.i.i
  %add680.i.i.i = add nsw i32 %ii542.i.i.0.i300, 15
  %and681.i.i.i = and i32 %add680.i.i.i, 15
  %arrayidx683.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and681.i.i.i
  %92 = load i32* %arrayidx683.i.i.i, align 4
  %add685.i.i.i = add nsw i32 %ii542.i.i.0.i300, 10
  %and686.i.i.i = and i32 %add685.i.i.i, 15
  %arrayidx688.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and686.i.i.i
  %93 = load i32* %arrayidx688.i.i.i, align 4
  %xor689.i.i.i = xor i32 %93, %92
  %add691.i.i.i = add nsw i32 %ii542.i.i.0.i300, 4
  %and692.i.i.i = and i32 %add691.i.i.i, 15
  %arrayidx694.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and692.i.i.i
  %94 = load i32* %arrayidx694.i.i.i, align 4
  %xor695.i.i.i = xor i32 %xor689.i.i.i, %94
  %95 = load i32* %arrayidx568.i.i.i, align 4
  %xor700.i.i.i = xor i32 %xor695.i.i.i, %95
  %shl701.i.i.i = shl i32 %xor700.i.i.i, 1
  %shr724.i.i.i = lshr i32 %xor700.i.i.i, 31
  %or725.i.i.i = or i32 %shl701.i.i.i, %shr724.i.i.i
  store i32 %or725.i.i.i, i32* %arrayidx568.i.i.i, align 4
  %add677.i.i.i = add i32 %v2.i.i.2.i297, -1894007588
  %add678.i.i.i = add i32 %add677.i.i.i, %xor676.i.i.i
  %add730.i.i.i = add i32 %add678.i.i.i, %or671.i.i.i
  %add731.i.i.i = add i32 %add730.i.i.i, %or725.i.i.i
  %shr732.i.i.i = lshr i32 %add599.i.i.i, 2
  %shl733.i.i.i = shl i32 %add599.i.i.i, 30
  %or734.i.i.i = or i32 %shr732.i.i.i, %shl733.i.i.i
  %shr735.i.i.i = lshr i32 %add731.i.i.i, 27
  %shl736.i.i.i = shl i32 %add731.i.i.i, 5
  %or737.i.i.i = or i32 %shr735.i.i.i, %shl736.i.i.i
  %and739.i.i.i99 = xor i32 %or734.i.i.i, %or668.i.i.i
  %xor740.i.i.i = and i32 %add665.i.i.i, %and739.i.i.i99
  %and741.i.i.i = and i32 %or734.i.i.i, %or668.i.i.i
  %xor742.i.i.i = xor i32 %xor740.i.i.i, %and741.i.i.i
  %96 = load i32* %arrayidx572.i.i.i, align 4
  %add751.i.i.i = add nsw i32 %ii542.i.i.0.i300, 11
  %and752.i.i.i = and i32 %add751.i.i.i, 15
  %arrayidx754.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and752.i.i.i
  %97 = load i32* %arrayidx754.i.i.i, align 4
  %xor755.i.i.i = xor i32 %97, %96
  %add757.i.i.i = add nsw i32 %ii542.i.i.0.i300, 5
  %and758.i.i.i = and i32 %add757.i.i.i, 15
  %arrayidx760.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and758.i.i.i
  %98 = load i32* %arrayidx760.i.i.i, align 4
  %xor761.i.i.i = xor i32 %xor755.i.i.i, %98
  %99 = load i32* %arrayidx628.i.i.i, align 4
  %xor766.i.i.i = xor i32 %xor761.i.i.i, %99
  %shl767.i.i.i = shl i32 %xor766.i.i.i, 1
  %shr790.i.i.i = lshr i32 %xor766.i.i.i, 31
  %or791.i.i.i = or i32 %shl767.i.i.i, %shr790.i.i.i
  store i32 %or791.i.i.i, i32* %arrayidx628.i.i.i, align 4
  %add743.i.i.i = add i32 %or602.i.i.i, -1894007588
  %add744.i.i.i = add i32 %add743.i.i.i, %xor742.i.i.i
  %add796.i.i.i = add i32 %add744.i.i.i, %or737.i.i.i
  %add797.i.i.i = add i32 %add796.i.i.i, %or791.i.i.i
  %shr798.i.i.i = lshr i32 %add665.i.i.i, 2
  %shl799.i.i.i = shl i32 %add665.i.i.i, 30
  %or800.i.i.i = or i32 %shr798.i.i.i, %shl799.i.i.i
  %shr801.i.i.i = lshr i32 %add797.i.i.i, 27
  %shl802.i.i.i = shl i32 %add797.i.i.i, 5
  %or803.i.i.i = or i32 %shr801.i.i.i, %shl802.i.i.i
  %and805.i.i.i104 = xor i32 %or800.i.i.i, %or734.i.i.i
  %xor806.i.i.i = and i32 %add731.i.i.i, %and805.i.i.i104
  %and807.i.i.i = and i32 %or800.i.i.i, %or734.i.i.i
  %xor808.i.i.i = xor i32 %xor806.i.i.i, %and807.i.i.i
  %100 = load i32* %arrayidx633.i.i.i, align 4
  %add817.i.i.i = add nsw i32 %ii542.i.i.0.i300, 12
  %and818.i.i.i = and i32 %add817.i.i.i, 15
  %arrayidx820.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and818.i.i.i
  %101 = load i32* %arrayidx820.i.i.i, align 4
  %xor821.i.i.i = xor i32 %101, %100
  %add823.i.i.i = add nsw i32 %ii542.i.i.0.i300, 6
  %and824.i.i.i = and i32 %add823.i.i.i, 15
  %arrayidx826.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and824.i.i.i
  %102 = load i32* %arrayidx826.i.i.i, align 4
  %xor827.i.i.i = xor i32 %xor821.i.i.i, %102
  %103 = load i32* %arrayidx694.i.i.i, align 4
  %xor832.i.i.i = xor i32 %xor827.i.i.i, %103
  %shl833.i.i.i = shl i32 %xor832.i.i.i, 1
  %shr856.i.i.i = lshr i32 %xor832.i.i.i, 31
  %or857.i.i.i = or i32 %shl833.i.i.i, %shr856.i.i.i
  store i32 %or857.i.i.i, i32* %arrayidx694.i.i.i, align 4
  %add809.i.i.i = add i32 %or668.i.i.i, -1894007588
  %add810.i.i.i = add i32 %add809.i.i.i, %xor808.i.i.i
  %add862.i.i.i = add i32 %add810.i.i.i, %or803.i.i.i
  %add863.i.i.i = add i32 %add862.i.i.i, %or857.i.i.i
  %shr864.i.i.i = lshr i32 %add731.i.i.i, 2
  %shl865.i.i.i = shl i32 %add731.i.i.i, 30
  %or866.i.i.i = or i32 %shr864.i.i.i, %shl865.i.i.i
  %cmp544.i.i.i = icmp slt i32 %add757.i.i.i, 60
  br i1 %cmp544.i.i.i, label %for.body545.i.i.i, label %for.body873.i.i.i

for.body873.i.i.i:                                ; preds = %for.body873.i.i.i, %for.body545.i.i.i
  %ii870.i.i.0.i294 = phi i32 [ %add1073.i.i.i, %for.body873.i.i.i ], [ 60, %for.body545.i.i.i ]
  %v4.i.i.3.i293 = phi i32 [ %or1053.i.i.i, %for.body873.i.i.i ], [ %or734.i.i.i, %for.body545.i.i.i ]
  %v3.i.i.3.i292 = phi i32 [ %or1116.i.i.i, %for.body873.i.i.i ], [ %or800.i.i.i, %for.body545.i.i.i ]
  %v2.i.i.3.i291 = phi i32 [ %or1179.i.i.i, %for.body873.i.i.i ], [ %or866.i.i.i, %for.body545.i.i.i ]
  %v1.i.i.3.i290 = phi i32 [ %add1113.i.i.i, %for.body873.i.i.i ], [ %add797.i.i.i, %for.body545.i.i.i ]
  %v0.i.i.3.i289 = phi i32 [ %add1176.i.i.i, %for.body873.i.i.i ], [ %add863.i.i.i, %for.body545.i.i.i ]
  %shr874.i.i.i = lshr i32 %v0.i.i.3.i289, 27
  %shl875.i.i.i = shl i32 %v0.i.i.3.i289, 5
  %or876.i.i.i = or i32 %shr874.i.i.i, %shl875.i.i.i
  %xor877.i.i.i = xor i32 %v2.i.i.3.i291, %v3.i.i.3.i292
  %xor878.i.i.i = xor i32 %xor877.i.i.i, %v1.i.i.3.i290
  %add881.i.i.i = add nsw i32 %ii870.i.i.0.i294, 13
  %and882.i.i.i = and i32 %add881.i.i.i, 15
  %arrayidx884.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and882.i.i.i
  %104 = load i32* %arrayidx884.i.i.i, align 4
  %add885.i.i.i = add nsw i32 %ii870.i.i.0.i294, 8
  %and886.i.i.i = and i32 %add885.i.i.i, 15
  %arrayidx888.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and886.i.i.i
  %105 = load i32* %arrayidx888.i.i.i, align 4
  %xor889.i.i.i = xor i32 %105, %104
  %add890.i.i.i = add nsw i32 %ii870.i.i.0.i294, 2
  %and891.i.i.i = and i32 %add890.i.i.i, 15
  %arrayidx893.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and891.i.i.i
  %106 = load i32* %arrayidx893.i.i.i, align 4
  %xor894.i.i.i = xor i32 %xor889.i.i.i, %106
  %and895.i.i.i = and i32 %ii870.i.i.0.i294, 15
  %arrayidx897.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and895.i.i.i
  %107 = load i32* %arrayidx897.i.i.i, align 4
  %xor898.i.i.i = xor i32 %xor894.i.i.i, %107
  %shl899.i.i.i = shl i32 %xor898.i.i.i, 1
  %shr918.i.i.i = lshr i32 %xor898.i.i.i, 31
  %or919.i.i.i = or i32 %shl899.i.i.i, %shr918.i.i.i
  store i32 %or919.i.i.i, i32* %arrayidx897.i.i.i, align 4
  %add879.i.i.i = add i32 %v4.i.i.3.i293, -899497514
  %add880.i.i.i = add i32 %add879.i.i.i, %xor878.i.i.i
  %add923.i.i.i = add i32 %add880.i.i.i, %or876.i.i.i
  %add924.i.i.i = add i32 %add923.i.i.i, %or919.i.i.i
  %shr925.i.i.i = lshr i32 %v1.i.i.3.i290, 2
  %shl926.i.i.i = shl i32 %v1.i.i.3.i290, 30
  %or927.i.i.i = or i32 %shr925.i.i.i, %shl926.i.i.i
  %shr928.i.i.i = lshr i32 %add924.i.i.i, 27
  %shl929.i.i.i = shl i32 %add924.i.i.i, 5
  %or930.i.i.i = or i32 %shr928.i.i.i, %shl929.i.i.i
  %xor931.i.i.i = xor i32 %v0.i.i.3.i289, %v2.i.i.3.i291
  %xor932.i.i.i = xor i32 %xor931.i.i.i, %or927.i.i.i
  %add935.i.i.i = add nsw i32 %ii870.i.i.0.i294, 1
  %add936.i.i.i = add nsw i32 %ii870.i.i.0.i294, 14
  %and937.i.i.i = and i32 %add936.i.i.i, 15
  %arrayidx939.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and937.i.i.i
  %108 = load i32* %arrayidx939.i.i.i, align 4
  %add941.i.i.i = add nsw i32 %ii870.i.i.0.i294, 9
  %and942.i.i.i = and i32 %add941.i.i.i, 15
  %arrayidx944.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and942.i.i.i
  %109 = load i32* %arrayidx944.i.i.i, align 4
  %xor945.i.i.i = xor i32 %109, %108
  %add947.i.i.i = add nsw i32 %ii870.i.i.0.i294, 3
  %and948.i.i.i = and i32 %add947.i.i.i, 15
  %arrayidx950.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and948.i.i.i
  %110 = load i32* %arrayidx950.i.i.i, align 4
  %xor951.i.i.i = xor i32 %xor945.i.i.i, %110
  %and953.i.i.i = and i32 %add935.i.i.i, 15
  %arrayidx955.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and953.i.i.i
  %111 = load i32* %arrayidx955.i.i.i, align 4
  %xor956.i.i.i = xor i32 %xor951.i.i.i, %111
  %shl957.i.i.i = shl i32 %xor956.i.i.i, 1
  %shr980.i.i.i = lshr i32 %xor956.i.i.i, 31
  %or981.i.i.i = or i32 %shl957.i.i.i, %shr980.i.i.i
  store i32 %or981.i.i.i, i32* %arrayidx955.i.i.i, align 4
  %add933.i.i.i = add i32 %v3.i.i.3.i292, -899497514
  %add934.i.i.i = add i32 %add933.i.i.i, %xor932.i.i.i
  %add986.i.i.i = add i32 %add934.i.i.i, %or930.i.i.i
  %add987.i.i.i = add i32 %add986.i.i.i, %or981.i.i.i
  %shr988.i.i.i = lshr i32 %v0.i.i.3.i289, 2
  %shl989.i.i.i = shl i32 %v0.i.i.3.i289, 30
  %or990.i.i.i = or i32 %shr988.i.i.i, %shl989.i.i.i
  %shr991.i.i.i = lshr i32 %add987.i.i.i, 27
  %shl992.i.i.i = shl i32 %add987.i.i.i, 5
  %or993.i.i.i = or i32 %shr991.i.i.i, %shl992.i.i.i
  %xor994.i.i.i = xor i32 %or990.i.i.i, %or927.i.i.i
  %xor995.i.i.i = xor i32 %xor994.i.i.i, %add924.i.i.i
  %add999.i.i.i = add nsw i32 %ii870.i.i.0.i294, 15
  %and1000.i.i.i = and i32 %add999.i.i.i, 15
  %arrayidx1002.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1000.i.i.i
  %112 = load i32* %arrayidx1002.i.i.i, align 4
  %add1004.i.i.i = add nsw i32 %ii870.i.i.0.i294, 10
  %and1005.i.i.i = and i32 %add1004.i.i.i, 15
  %arrayidx1007.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1005.i.i.i
  %113 = load i32* %arrayidx1007.i.i.i, align 4
  %xor1008.i.i.i = xor i32 %113, %112
  %add1010.i.i.i = add nsw i32 %ii870.i.i.0.i294, 4
  %and1011.i.i.i = and i32 %add1010.i.i.i, 15
  %arrayidx1013.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1011.i.i.i
  %114 = load i32* %arrayidx1013.i.i.i, align 4
  %xor1014.i.i.i = xor i32 %xor1008.i.i.i, %114
  %115 = load i32* %arrayidx893.i.i.i, align 4
  %xor1019.i.i.i = xor i32 %xor1014.i.i.i, %115
  %shl1020.i.i.i = shl i32 %xor1019.i.i.i, 1
  %shr1043.i.i.i = lshr i32 %xor1019.i.i.i, 31
  %or1044.i.i.i = or i32 %shl1020.i.i.i, %shr1043.i.i.i
  store i32 %or1044.i.i.i, i32* %arrayidx893.i.i.i, align 4
  %add996.i.i.i = add i32 %v2.i.i.3.i291, -899497514
  %add997.i.i.i = add i32 %add996.i.i.i, %xor995.i.i.i
  %add1049.i.i.i = add i32 %add997.i.i.i, %or993.i.i.i
  %add1050.i.i.i = add i32 %add1049.i.i.i, %or1044.i.i.i
  %shr1051.i.i.i = lshr i32 %add924.i.i.i, 2
  %shl1052.i.i.i = shl i32 %add924.i.i.i, 30
  %or1053.i.i.i = or i32 %shr1051.i.i.i, %shl1052.i.i.i
  %shr1054.i.i.i = lshr i32 %add1050.i.i.i, 27
  %shl1055.i.i.i = shl i32 %add1050.i.i.i, 5
  %or1056.i.i.i = or i32 %shr1054.i.i.i, %shl1055.i.i.i
  %xor1057.i.i.i = xor i32 %or1053.i.i.i, %or990.i.i.i
  %xor1058.i.i.i = xor i32 %xor1057.i.i.i, %add987.i.i.i
  %116 = load i32* %arrayidx897.i.i.i, align 4
  %add1067.i.i.i = add nsw i32 %ii870.i.i.0.i294, 11
  %and1068.i.i.i = and i32 %add1067.i.i.i, 15
  %arrayidx1070.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1068.i.i.i
  %117 = load i32* %arrayidx1070.i.i.i, align 4
  %xor1071.i.i.i = xor i32 %117, %116
  %add1073.i.i.i = add nsw i32 %ii870.i.i.0.i294, 5
  %and1074.i.i.i = and i32 %add1073.i.i.i, 15
  %arrayidx1076.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1074.i.i.i
  %118 = load i32* %arrayidx1076.i.i.i, align 4
  %xor1077.i.i.i = xor i32 %xor1071.i.i.i, %118
  %119 = load i32* %arrayidx950.i.i.i, align 4
  %xor1082.i.i.i = xor i32 %xor1077.i.i.i, %119
  %shl1083.i.i.i = shl i32 %xor1082.i.i.i, 1
  %shr1106.i.i.i = lshr i32 %xor1082.i.i.i, 31
  %or1107.i.i.i = or i32 %shl1083.i.i.i, %shr1106.i.i.i
  store i32 %or1107.i.i.i, i32* %arrayidx950.i.i.i, align 4
  %add1059.i.i.i = add i32 %or927.i.i.i, -899497514
  %add1060.i.i.i = add i32 %add1059.i.i.i, %xor1058.i.i.i
  %add1112.i.i.i = add i32 %add1060.i.i.i, %or1056.i.i.i
  %add1113.i.i.i = add i32 %add1112.i.i.i, %or1107.i.i.i
  %shr1114.i.i.i = lshr i32 %add987.i.i.i, 2
  %shl1115.i.i.i = shl i32 %add987.i.i.i, 30
  %or1116.i.i.i = or i32 %shr1114.i.i.i, %shl1115.i.i.i
  %shr1117.i.i.i = lshr i32 %add1113.i.i.i, 27
  %shl1118.i.i.i = shl i32 %add1113.i.i.i, 5
  %or1119.i.i.i = or i32 %shr1117.i.i.i, %shl1118.i.i.i
  %xor1120.i.i.i = xor i32 %or1116.i.i.i, %or1053.i.i.i
  %xor1121.i.i.i = xor i32 %xor1120.i.i.i, %add1050.i.i.i
  %120 = load i32* %arrayidx955.i.i.i, align 4
  %add1130.i.i.i = add nsw i32 %ii870.i.i.0.i294, 12
  %and1131.i.i.i = and i32 %add1130.i.i.i, 15
  %arrayidx1133.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1131.i.i.i
  %121 = load i32* %arrayidx1133.i.i.i, align 4
  %xor1134.i.i.i = xor i32 %121, %120
  %add1136.i.i.i = add nsw i32 %ii870.i.i.0.i294, 6
  %and1137.i.i.i = and i32 %add1136.i.i.i, 15
  %arrayidx1139.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1137.i.i.i
  %122 = load i32* %arrayidx1139.i.i.i, align 4
  %xor1140.i.i.i = xor i32 %xor1134.i.i.i, %122
  %123 = load i32* %arrayidx1013.i.i.i, align 4
  %xor1145.i.i.i = xor i32 %xor1140.i.i.i, %123
  %shl1146.i.i.i = shl i32 %xor1145.i.i.i, 1
  %shr1169.i.i.i = lshr i32 %xor1145.i.i.i, 31
  %or1170.i.i.i = or i32 %shl1146.i.i.i, %shr1169.i.i.i
  store i32 %or1170.i.i.i, i32* %arrayidx1013.i.i.i, align 4
  %add1122.i.i.i = add i32 %or990.i.i.i, -899497514
  %add1123.i.i.i = add i32 %add1122.i.i.i, %xor1121.i.i.i
  %add1175.i.i.i = add i32 %add1123.i.i.i, %or1119.i.i.i
  %add1176.i.i.i = add i32 %add1175.i.i.i, %or1170.i.i.i
  %shr1177.i.i.i = lshr i32 %add1050.i.i.i, 2
  %shl1178.i.i.i = shl i32 %add1050.i.i.i, 30
  %or1179.i.i.i = or i32 %shr1177.i.i.i, %shl1178.i.i.i
  %cmp872.i.i.i = icmp slt i32 %add1073.i.i.i, 80
  br i1 %cmp872.i.i.i, label %for.body873.i.i.i, label %while.cond48.i.i.preheader.thread

while.cond48.i.i.preheader.thread:                ; preds = %for.body873.i.i.i
  %124 = load i32* %arrayidx3.i.i, align 4
  %add1185.i.i.i = add i32 %124, %add1176.i.i.i
  store i32 %add1185.i.i.i, i32* %arrayidx3.i.i, align 4
  %125 = load i32* %arrayidx5.i.i, align 4
  %add1188.i.i.i = add i32 %125, %add1113.i.i.i
  store i32 %add1188.i.i.i, i32* %arrayidx5.i.i, align 4
  %126 = load i32* %arrayidx7.i.i, align 4
  %add1191.i.i.i = add i32 %126, %or1179.i.i.i
  store i32 %add1191.i.i.i, i32* %arrayidx7.i.i, align 4
  %127 = load i32* %arrayidx9.i.i, align 4
  %add1194.i.i.i = add i32 %127, %or1116.i.i.i
  store i32 %add1194.i.i.i, i32* %arrayidx9.i.i, align 4
  %128 = load i32* %arrayidx11.i.i, align 4
  %add1197.i.i.i = add i32 %128, %or1053.i.i.i
  store i32 %add1197.i.i.i, i32* %arrayidx11.i.i, align 4
  br label %while.body50.i.i

while.cond48.i.i.preheader:                       ; preds = %while.end.i.i
  %add46.i.i = add i32 %shr27.i.i, 1
  %cmp49.i.i287 = icmp ult i32 %add46.i.i, 14
  br i1 %cmp49.i.i287, label %while.body50.i.i, label %while.end54.i.i

while.body50.i.i:                                 ; preds = %while.body50.i.i, %while.cond48.i.i.preheader, %while.cond48.i.i.preheader.thread
  %i.i.0.i288 = phi i32 [ %inc.i.i, %while.body50.i.i ], [ %add46.i.i, %while.cond48.i.i.preheader ], [ 0, %while.cond48.i.i.preheader.thread ]
  %inc.i.i = add i32 %i.i.0.i288, 1
  %arrayidx53.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %i.i.0.i288
  store i32 0, i32* %arrayidx53.i.i, align 4
  %exitcond425 = icmp eq i32 %inc.i.i, 14
  br i1 %exitcond425, label %while.end54.i.i, label %while.body50.i.i

while.end54.i.i:                                  ; preds = %while.body50.i.i, %while.cond48.i.i.preheader
  %129 = load i32* %arrayidx.i.i, align 4
  %shl57.i.i = shl i32 %129, 3
  %130 = load i32* %arrayidx2.i.i, align 4
  %shr60.i.i = lshr i32 %130, 29
  %or61.i.i = or i32 %shr60.i.i, %shl57.i.i
  %arrayidx63.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 14
  store i32 %or61.i.i, i32* %arrayidx63.i.i, align 4
  %shl66.i.i = shl i32 %130, 3
  %arrayidx68.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 15
  store i32 %shl66.i.i, i32* %arrayidx68.i.i, align 4
  %131 = load i32* %arrayidx3.i.i, align 4
  %132 = load i32* %arrayidx5.i.i, align 4
  %133 = load i32* %arrayidx7.i.i, align 4
  %134 = load i32* %arrayidx9.i.i, align 4
  %135 = load i32* %arrayidx11.i.i, align 4
  %shr.i105.i.i271 = lshr i32 %131, 27
  %shl.i106.i.i272 = shl i32 %131, 5
  %or.i107.i.i273 = or i32 %shr.i105.i.i271, %shl.i106.i.i272
  %and.i108.i.i274 = and i32 %133, %132
  %neg.i109.i.i275 = xor i32 %132, -1
  %and9.i110.i.i276 = and i32 %134, %neg.i109.i.i275
  %xor.i111.i.i277 = xor i32 %and9.i110.i.i276, %and.i108.i.i274
  %add.i112.i.i278 = add i32 %or.i107.i.i273, 1518500249
  %add10.i113.i.i279 = add i32 %add.i112.i.i278, %xor.i111.i.i277
  br label %for.body.i189.i.i

for.body.i189.i.i:                                ; preds = %for.body.i189.i.i, %while.end54.i.i
  %add10.i113.i.i286 = phi i32 [ %add10.i113.i.i279, %while.end54.i.i ], [ %add10.i113.i.i, %for.body.i189.i.i ]
  %ii.i87.i.0.i285 = phi i32 [ 0, %while.end54.i.i ], [ %add85.i190.i.i, %for.body.i189.i.i ]
  %v4.i86.i.0.i284 = phi i32 [ %135, %while.end54.i.i ], [ %or50.i154.i.i, %for.body.i189.i.i ]
  %v3.i85.i.0.i283 = phi i32 [ %134, %while.end54.i.i ], [ %or67.i171.i.i, %for.body.i189.i.i ]
  %v2.i84.i.0.i282 = phi i32 [ %133, %while.end54.i.i ], [ %or84.i188.i.i, %for.body.i189.i.i ]
  %v1.i83.i.0.i281 = phi i32 [ %132, %while.end54.i.i ], [ %add64.i168.i.i, %for.body.i189.i.i ]
  %v0.i82.i.0.i280 = phi i32 [ %131, %while.end54.i.i ], [ %add81.i185.i.i, %for.body.i189.i.i ]
  %arrayidx11.i115.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %ii.i87.i.0.i285
  %136 = load i32* %arrayidx11.i115.i.i, align 4
  %add12.i116.i.i = add i32 %v4.i86.i.0.i284, %add10.i113.i.i286
  %add13.i117.i.i = add i32 %add12.i116.i.i, %136
  %shr14.i118.i.i = lshr i32 %v1.i83.i.0.i281, 2
  %shl15.i119.i.i = shl i32 %v1.i83.i.0.i281, 30
  %or16.i120.i.i = or i32 %shr14.i118.i.i, %shl15.i119.i.i
  %shr17.i121.i.i = lshr i32 %add13.i117.i.i, 27
  %shl18.i122.i.i = shl i32 %add13.i117.i.i, 5
  %or19.i123.i.i = or i32 %shr17.i121.i.i, %shl18.i122.i.i
  %and20.i124.i.i = and i32 %or16.i120.i.i, %v0.i82.i.0.i280
  %neg21.i125.i.i = xor i32 %v0.i82.i.0.i280, -1
  %and22.i126.i.i = and i32 %v2.i84.i.0.i282, %neg21.i125.i.i
  %xor23.i127.i.i = xor i32 %and20.i124.i.i, %and22.i126.i.i
  %add26.i130.i.i = add nsw i32 %ii.i87.i.0.i285, 1
  %arrayidx28.i132.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add26.i130.i.i
  %137 = load i32* %arrayidx28.i132.i.i, align 4
  %add24.i128.i.i = add i32 %v3.i85.i.0.i283, 1518500249
  %add25.i129.i.i = add i32 %add24.i128.i.i, %137
  %add29.i133.i.i = add i32 %add25.i129.i.i, %xor23.i127.i.i
  %add30.i134.i.i = add i32 %add29.i133.i.i, %or19.i123.i.i
  %shr31.i135.i.i = lshr i32 %v0.i82.i.0.i280, 2
  %shl32.i136.i.i = shl i32 %v0.i82.i.0.i280, 30
  %or33.i137.i.i = or i32 %shr31.i135.i.i, %shl32.i136.i.i
  %shr34.i138.i.i = lshr i32 %add30.i134.i.i, 27
  %shl35.i139.i.i = shl i32 %add30.i134.i.i, 5
  %or36.i140.i.i = or i32 %shr34.i138.i.i, %shl35.i139.i.i
  %and37.i141.i.i = and i32 %add13.i117.i.i, %or33.i137.i.i
  %neg38.i142.i.i = xor i32 %add13.i117.i.i, -1
  %and39.i143.i.i = and i32 %or16.i120.i.i, %neg38.i142.i.i
  %xor40.i144.i.i = xor i32 %and37.i141.i.i, %and39.i143.i.i
  %add43.i147.i.i = add nsw i32 %ii.i87.i.0.i285, 2
  %arrayidx45.i149.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add43.i147.i.i
  %138 = load i32* %arrayidx45.i149.i.i, align 4
  %add41.i145.i.i = add i32 %v2.i84.i.0.i282, 1518500249
  %add42.i146.i.i = add i32 %add41.i145.i.i, %138
  %add46.i150.i.i = add i32 %add42.i146.i.i, %xor40.i144.i.i
  %add47.i151.i.i = add i32 %add46.i150.i.i, %or36.i140.i.i
  %shr48.i152.i.i = lshr i32 %add13.i117.i.i, 2
  %shl49.i153.i.i = shl i32 %add13.i117.i.i, 30
  %or50.i154.i.i = or i32 %shr48.i152.i.i, %shl49.i153.i.i
  %shr51.i155.i.i = lshr i32 %add47.i151.i.i, 27
  %shl52.i156.i.i = shl i32 %add47.i151.i.i, 5
  %or53.i157.i.i = or i32 %shr51.i155.i.i, %shl52.i156.i.i
  %and54.i158.i.i = and i32 %add30.i134.i.i, %or50.i154.i.i
  %neg55.i159.i.i = xor i32 %add30.i134.i.i, -1
  %and56.i160.i.i = and i32 %or33.i137.i.i, %neg55.i159.i.i
  %xor57.i161.i.i = xor i32 %and54.i158.i.i, %and56.i160.i.i
  %add60.i164.i.i = add nsw i32 %ii.i87.i.0.i285, 3
  %arrayidx62.i166.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add60.i164.i.i
  %139 = load i32* %arrayidx62.i166.i.i, align 4
  %add58.i162.i.i = add i32 %or16.i120.i.i, 1518500249
  %add59.i163.i.i = add i32 %add58.i162.i.i, %139
  %add63.i167.i.i = add i32 %add59.i163.i.i, %xor57.i161.i.i
  %add64.i168.i.i = add i32 %add63.i167.i.i, %or53.i157.i.i
  %shr65.i169.i.i = lshr i32 %add30.i134.i.i, 2
  %shl66.i170.i.i = shl i32 %add30.i134.i.i, 30
  %or67.i171.i.i = or i32 %shr65.i169.i.i, %shl66.i170.i.i
  %shr68.i172.i.i = lshr i32 %add64.i168.i.i, 27
  %shl69.i173.i.i = shl i32 %add64.i168.i.i, 5
  %or70.i174.i.i = or i32 %shr68.i172.i.i, %shl69.i173.i.i
  %and71.i175.i.i = and i32 %add47.i151.i.i, %or67.i171.i.i
  %neg72.i176.i.i = xor i32 %add47.i151.i.i, -1
  %and73.i177.i.i = and i32 %or50.i154.i.i, %neg72.i176.i.i
  %xor74.i178.i.i = xor i32 %and71.i175.i.i, %and73.i177.i.i
  %add77.i181.i.i = add nsw i32 %ii.i87.i.0.i285, 4
  %arrayidx79.i183.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add77.i181.i.i
  %140 = load i32* %arrayidx79.i183.i.i, align 4
  %add75.i179.i.i = add i32 %or33.i137.i.i, 1518500249
  %add76.i180.i.i = add i32 %add75.i179.i.i, %140
  %add80.i184.i.i = add i32 %add76.i180.i.i, %xor74.i178.i.i
  %add81.i185.i.i = add i32 %add80.i184.i.i, %or70.i174.i.i
  %shr82.i186.i.i = lshr i32 %add47.i151.i.i, 2
  %shl83.i187.i.i = shl i32 %add47.i151.i.i, 30
  %or84.i188.i.i = or i32 %shr82.i186.i.i, %shl83.i187.i.i
  %add85.i190.i.i = add nsw i32 %ii.i87.i.0.i285, 5
  %cmp.i103.i.i = icmp slt i32 %add85.i190.i.i, 15
  %shr.i105.i.i = lshr i32 %add81.i185.i.i, 27
  %shl.i106.i.i = shl i32 %add81.i185.i.i, 5
  %or.i107.i.i = or i32 %shr.i105.i.i, %shl.i106.i.i
  %and.i108.i.i = and i32 %add64.i168.i.i, %or84.i188.i.i
  %neg.i109.i.i = xor i32 %add64.i168.i.i, -1
  %and9.i110.i.i = and i32 %or67.i171.i.i, %neg.i109.i.i
  %xor.i111.i.i = xor i32 %and9.i110.i.i, %and.i108.i.i
  %add.i112.i.i = add i32 %xor.i111.i.i, 1518500249
  %add10.i113.i.i = add i32 %add.i112.i.i, %or.i107.i.i
  br i1 %cmp.i103.i.i, label %for.body.i189.i.i, label %for.end.i329.i.i

for.end.i329.i.i:                                 ; preds = %for.body.i189.i.i
  %arraydecay.i92.i.i = getelementptr inbounds [16 x i32]* %wbuf29.i.i, i32 0, i32 0
  %add96.i201.i.i = add i32 %or50.i154.i.i, %shl66.i.i
  %add97.i202.i.i = add i32 %add96.i201.i.i, %add10.i113.i.i
  %shr98.i203.i.i = lshr i32 %add64.i168.i.i, 2
  %shl99.i204.i.i = shl i32 %add64.i168.i.i, 30
  %or100.i205.i.i = or i32 %shr98.i203.i.i, %shl99.i204.i.i
  %shr101.i206.i.i = lshr i32 %add97.i202.i.i, 27
  %shl102.i207.i.i = shl i32 %add97.i202.i.i, 5
  %or103.i208.i.i = or i32 %shr101.i206.i.i, %shl102.i207.i.i
  %and104.i209.i.i = and i32 %add81.i185.i.i, %or100.i205.i.i
  %neg105.i210.i.i = xor i32 %add81.i185.i.i, -1
  %and106.i211.i.i = and i32 %or84.i188.i.i, %neg105.i210.i.i
  %xor107.i212.i.i = xor i32 %and104.i209.i.i, %and106.i211.i.i
  %arrayidx110.i215.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 13
  %141 = load i32* %arrayidx110.i215.i.i, align 4
  %arrayidx111.i216.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 8
  %142 = load i32* %arrayidx111.i216.i.i, align 4
  %xor112.i217.i.i = xor i32 %142, %141
  %arrayidx113.i218.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 2
  %143 = load i32* %arrayidx113.i218.i.i, align 4
  %xor114.i219.i.i = xor i32 %xor112.i217.i.i, %143
  %144 = load i32* %arraydecay.i92.i.i, align 4
  %xor116.i220.i.i = xor i32 %xor114.i219.i.i, %144
  %shl117.i221.i.i = shl i32 %xor116.i220.i.i, 1
  %shr125.i228.i.i = lshr i32 %xor116.i220.i.i, 31
  %or126.i229.i.i = or i32 %shl117.i221.i.i, %shr125.i228.i.i
  store i32 %or126.i229.i.i, i32* %arraydecay.i92.i.i, align 4
  %add108.i213.i.i = add i32 %or67.i171.i.i, 1518500249
  %add109.i214.i.i = add i32 %add108.i213.i.i, %xor107.i212.i.i
  %add128.i230.i.i = add i32 %add109.i214.i.i, %or103.i208.i.i
  %add129.i231.i.i = add i32 %add128.i230.i.i, %or126.i229.i.i
  %shr130.i232.i.i = lshr i32 %add81.i185.i.i, 2
  %shl131.i233.i.i = shl i32 %add81.i185.i.i, 30
  %or132.i234.i.i = or i32 %shr130.i232.i.i, %shl131.i233.i.i
  %shr133.i235.i.i = lshr i32 %add129.i231.i.i, 27
  %shl134.i236.i.i = shl i32 %add129.i231.i.i, 5
  %or135.i237.i.i = or i32 %shr133.i235.i.i, %shl134.i236.i.i
  %and136.i238.i.i = and i32 %add97.i202.i.i, %or132.i234.i.i
  %neg137.i239.i.i = xor i32 %add97.i202.i.i, -1
  %and138.i240.i.i = and i32 %or100.i205.i.i, %neg137.i239.i.i
  %xor139.i241.i.i = xor i32 %and136.i238.i.i, %and138.i240.i.i
  %arrayidx143.i245.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 9
  %145 = load i32* %arrayidx143.i245.i.i, align 4
  %xor144.i246.i.i = xor i32 %145, %or61.i.i
  %arrayidx145.i247.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 3
  %146 = load i32* %arrayidx145.i247.i.i, align 4
  %xor146.i248.i.i = xor i32 %xor144.i246.i.i, %146
  %arrayidx147.i249.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 1
  %147 = load i32* %arrayidx147.i249.i.i, align 4
  %xor148.i250.i.i = xor i32 %xor146.i248.i.i, %147
  %shl149.i251.i.i = shl i32 %xor148.i250.i.i, 1
  %shr157.i259.i.i = lshr i32 %xor148.i250.i.i, 31
  %or158.i260.i.i = or i32 %shl149.i251.i.i, %shr157.i259.i.i
  store i32 %or158.i260.i.i, i32* %arrayidx147.i249.i.i, align 4
  %add140.i242.i.i = add i32 %or84.i188.i.i, 1518500249
  %add141.i243.i.i = add i32 %add140.i242.i.i, %xor139.i241.i.i
  %add160.i262.i.i = add i32 %add141.i243.i.i, %or135.i237.i.i
  %add161.i263.i.i = add i32 %add160.i262.i.i, %or158.i260.i.i
  %shr162.i264.i.i = lshr i32 %add97.i202.i.i, 2
  %shl163.i265.i.i = shl i32 %add97.i202.i.i, 30
  %or164.i266.i.i = or i32 %shr162.i264.i.i, %shl163.i265.i.i
  %shr165.i267.i.i = lshr i32 %add161.i263.i.i, 27
  %shl166.i268.i.i = shl i32 %add161.i263.i.i, 5
  %or167.i269.i.i = or i32 %shr165.i267.i.i, %shl166.i268.i.i
  %and168.i270.i.i = and i32 %add129.i231.i.i, %or164.i266.i.i
  %neg169.i271.i.i = xor i32 %add129.i231.i.i, -1
  %and170.i272.i.i = and i32 %or132.i234.i.i, %neg169.i271.i.i
  %xor171.i273.i.i = xor i32 %and168.i270.i.i, %and170.i272.i.i
  %arrayidx175.i277.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 10
  %148 = load i32* %arrayidx175.i277.i.i, align 4
  %arrayidx177.i279.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 4
  %149 = load i32* %arrayidx177.i279.i.i, align 4
  %xor176.i278.i.i = xor i32 %143, %shl66.i.i
  %xor178.i280.i.i = xor i32 %xor176.i278.i.i, %148
  %xor180.i282.i.i = xor i32 %xor178.i280.i.i, %149
  %shl181.i283.i.i = shl i32 %xor180.i282.i.i, 1
  %shr189.i291.i.i = lshr i32 %xor180.i282.i.i, 31
  %or190.i292.i.i = or i32 %shl181.i283.i.i, %shr189.i291.i.i
  store i32 %or190.i292.i.i, i32* %arrayidx113.i218.i.i, align 4
  %add172.i274.i.i = add i32 %or100.i205.i.i, 1518500249
  %add173.i275.i.i = add i32 %add172.i274.i.i, %xor171.i273.i.i
  %add192.i294.i.i = add i32 %add173.i275.i.i, %or190.i292.i.i
  %add193.i295.i.i = add i32 %add192.i294.i.i, %or167.i269.i.i
  %shr194.i296.i.i = lshr i32 %add129.i231.i.i, 2
  %shl195.i297.i.i = shl i32 %add129.i231.i.i, 30
  %or196.i298.i.i = or i32 %shr194.i296.i.i, %shl195.i297.i.i
  %shr197.i299.i.i = lshr i32 %add193.i295.i.i, 27
  %shl198.i300.i.i = shl i32 %add193.i295.i.i, 5
  %or199.i301.i.i = or i32 %shr197.i299.i.i, %shl198.i300.i.i
  %and200.i302.i.i = and i32 %add161.i263.i.i, %or196.i298.i.i
  %neg201.i303.i.i = xor i32 %add161.i263.i.i, -1
  %and202.i304.i.i = and i32 %or164.i266.i.i, %neg201.i303.i.i
  %xor203.i305.i.i = xor i32 %and200.i302.i.i, %and202.i304.i.i
  %arrayidx207.i308.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 11
  %150 = load i32* %arrayidx207.i308.i.i, align 4
  %arrayidx209.i310.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 5
  %151 = load i32* %arrayidx209.i310.i.i, align 4
  %xor208.i309.i.i = xor i32 %or126.i229.i.i, %146
  %xor210.i311.i.i = xor i32 %xor208.i309.i.i, %150
  %xor212.i313.i.i = xor i32 %xor210.i311.i.i, %151
  %shl213.i314.i.i = shl i32 %xor212.i313.i.i, 1
  %shr221.i321.i.i = lshr i32 %xor212.i313.i.i, 31
  %or222.i322.i.i = or i32 %shl213.i314.i.i, %shr221.i321.i.i
  store i32 %or222.i322.i.i, i32* %arrayidx145.i247.i.i, align 4
  %add204.i306.i.i = add i32 %or132.i234.i.i, 1518500249
  %add205.i307.i.i = add i32 %add204.i306.i.i, %xor203.i305.i.i
  %add224.i324.i.i = add i32 %add205.i307.i.i, %or222.i322.i.i
  %add225.i325.i.i = add i32 %add224.i324.i.i, %or199.i301.i.i
  %shr226.i326.i.i = lshr i32 %add161.i263.i.i, 2
  %shl227.i327.i.i = shl i32 %add161.i263.i.i, 30
  %or228.i328.i.i = or i32 %shr226.i326.i.i, %shl227.i327.i.i
  br label %for.body232.i638.i.i

for.body232.i638.i.i:                             ; preds = %for.body232.i638.i.i, %for.end.i329.i.i
  %ii229.i88.i.0.i270 = phi i32 [ 20, %for.end.i329.i.i ], [ %add432.i531.i.i, %for.body232.i638.i.i ]
  %v4.i86.i.1.i269 = phi i32 [ %or164.i266.i.i, %for.end.i329.i.i ], [ %or412.i511.i.i, %for.body232.i638.i.i ]
  %v3.i85.i.1.i268 = phi i32 [ %or196.i298.i.i, %for.end.i329.i.i ], [ %or475.i574.i.i, %for.body232.i638.i.i ]
  %v2.i84.i.1.i267 = phi i32 [ %or228.i328.i.i, %for.end.i329.i.i ], [ %or538.i637.i.i, %for.body232.i638.i.i ]
  %v1.i83.i.1.i266 = phi i32 [ %add193.i295.i.i, %for.end.i329.i.i ], [ %add472.i571.i.i, %for.body232.i638.i.i ]
  %v0.i82.i.1.i265 = phi i32 [ %add225.i325.i.i, %for.end.i329.i.i ], [ %add535.i634.i.i, %for.body232.i638.i.i ]
  %shr233.i332.i.i = lshr i32 %v0.i82.i.1.i265, 27
  %shl234.i333.i.i = shl i32 %v0.i82.i.1.i265, 5
  %or235.i334.i.i = or i32 %shr233.i332.i.i, %shl234.i333.i.i
  %xor236.i335.i.i = xor i32 %v2.i84.i.1.i267, %v3.i85.i.1.i268
  %xor237.i336.i.i = xor i32 %xor236.i335.i.i, %v1.i83.i.1.i266
  %add240.i339.i.i = add nsw i32 %ii229.i88.i.0.i270, 13
  %and241.i340.i.i = and i32 %add240.i339.i.i, 15
  %arrayidx243.i342.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and241.i340.i.i
  %152 = load i32* %arrayidx243.i342.i.i, align 4
  %add244.i343.i.i = add nsw i32 %ii229.i88.i.0.i270, 8
  %and245.i344.i.i = and i32 %add244.i343.i.i, 15
  %arrayidx247.i346.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and245.i344.i.i
  %153 = load i32* %arrayidx247.i346.i.i, align 4
  %xor248.i347.i.i = xor i32 %153, %152
  %add249.i348.i.i = add nsw i32 %ii229.i88.i.0.i270, 2
  %and250.i349.i.i = and i32 %add249.i348.i.i, 15
  %arrayidx252.i351.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and250.i349.i.i
  %154 = load i32* %arrayidx252.i351.i.i, align 4
  %xor253.i352.i.i = xor i32 %xor248.i347.i.i, %154
  %and254.i353.i.i = and i32 %ii229.i88.i.0.i270, 15
  %arrayidx256.i355.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and254.i353.i.i
  %155 = load i32* %arrayidx256.i355.i.i, align 4
  %xor257.i356.i.i = xor i32 %xor253.i352.i.i, %155
  %shl258.i357.i.i = shl i32 %xor257.i356.i.i, 1
  %shr277.i376.i.i = lshr i32 %xor257.i356.i.i, 31
  %or278.i377.i.i = or i32 %shl258.i357.i.i, %shr277.i376.i.i
  store i32 %or278.i377.i.i, i32* %arrayidx256.i355.i.i, align 4
  %add238.i337.i.i = add i32 %v4.i86.i.1.i269, 1859775393
  %add239.i338.i.i = add i32 %add238.i337.i.i, %xor237.i336.i.i
  %add282.i381.i.i = add i32 %add239.i338.i.i, %or235.i334.i.i
  %add283.i382.i.i = add i32 %add282.i381.i.i, %or278.i377.i.i
  %shr284.i383.i.i = lshr i32 %v1.i83.i.1.i266, 2
  %shl285.i384.i.i = shl i32 %v1.i83.i.1.i266, 30
  %or286.i385.i.i = or i32 %shr284.i383.i.i, %shl285.i384.i.i
  %shr287.i386.i.i = lshr i32 %add283.i382.i.i, 27
  %shl288.i387.i.i = shl i32 %add283.i382.i.i, 5
  %or289.i388.i.i = or i32 %shr287.i386.i.i, %shl288.i387.i.i
  %xor290.i389.i.i = xor i32 %v0.i82.i.1.i265, %v2.i84.i.1.i267
  %xor291.i390.i.i = xor i32 %xor290.i389.i.i, %or286.i385.i.i
  %add294.i393.i.i = add nsw i32 %ii229.i88.i.0.i270, 1
  %add295.i394.i.i = add nsw i32 %ii229.i88.i.0.i270, 14
  %and296.i395.i.i = and i32 %add295.i394.i.i, 15
  %arrayidx298.i397.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and296.i395.i.i
  %156 = load i32* %arrayidx298.i397.i.i, align 4
  %add300.i399.i.i = add nsw i32 %ii229.i88.i.0.i270, 9
  %and301.i400.i.i = and i32 %add300.i399.i.i, 15
  %arrayidx303.i402.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and301.i400.i.i
  %157 = load i32* %arrayidx303.i402.i.i, align 4
  %xor304.i403.i.i = xor i32 %157, %156
  %add306.i405.i.i = add nsw i32 %ii229.i88.i.0.i270, 3
  %and307.i406.i.i = and i32 %add306.i405.i.i, 15
  %arrayidx309.i408.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and307.i406.i.i
  %158 = load i32* %arrayidx309.i408.i.i, align 4
  %xor310.i409.i.i = xor i32 %xor304.i403.i.i, %158
  %and312.i411.i.i = and i32 %add294.i393.i.i, 15
  %arrayidx314.i413.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and312.i411.i.i
  %159 = load i32* %arrayidx314.i413.i.i, align 4
  %xor315.i414.i.i = xor i32 %xor310.i409.i.i, %159
  %shl316.i415.i.i = shl i32 %xor315.i414.i.i, 1
  %shr339.i438.i.i = lshr i32 %xor315.i414.i.i, 31
  %or340.i439.i.i = or i32 %shl316.i415.i.i, %shr339.i438.i.i
  store i32 %or340.i439.i.i, i32* %arrayidx314.i413.i.i, align 4
  %add292.i391.i.i = add i32 %v3.i85.i.1.i268, 1859775393
  %add293.i392.i.i = add i32 %add292.i391.i.i, %xor291.i390.i.i
  %add345.i444.i.i = add i32 %add293.i392.i.i, %or289.i388.i.i
  %add346.i445.i.i = add i32 %add345.i444.i.i, %or340.i439.i.i
  %shr347.i446.i.i = lshr i32 %v0.i82.i.1.i265, 2
  %shl348.i447.i.i = shl i32 %v0.i82.i.1.i265, 30
  %or349.i448.i.i = or i32 %shr347.i446.i.i, %shl348.i447.i.i
  %shr350.i449.i.i = lshr i32 %add346.i445.i.i, 27
  %shl351.i450.i.i = shl i32 %add346.i445.i.i, 5
  %or352.i451.i.i = or i32 %shr350.i449.i.i, %shl351.i450.i.i
  %xor353.i452.i.i = xor i32 %or349.i448.i.i, %or286.i385.i.i
  %xor354.i453.i.i = xor i32 %xor353.i452.i.i, %add283.i382.i.i
  %add358.i457.i.i = add nsw i32 %ii229.i88.i.0.i270, 15
  %and359.i458.i.i = and i32 %add358.i457.i.i, 15
  %arrayidx361.i460.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and359.i458.i.i
  %160 = load i32* %arrayidx361.i460.i.i, align 4
  %add363.i462.i.i = add nsw i32 %ii229.i88.i.0.i270, 10
  %and364.i463.i.i = and i32 %add363.i462.i.i, 15
  %arrayidx366.i465.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and364.i463.i.i
  %161 = load i32* %arrayidx366.i465.i.i, align 4
  %xor367.i466.i.i = xor i32 %161, %160
  %add369.i468.i.i = add nsw i32 %ii229.i88.i.0.i270, 4
  %and370.i469.i.i = and i32 %add369.i468.i.i, 15
  %arrayidx372.i471.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and370.i469.i.i
  %162 = load i32* %arrayidx372.i471.i.i, align 4
  %xor373.i472.i.i = xor i32 %xor367.i466.i.i, %162
  %163 = load i32* %arrayidx252.i351.i.i, align 4
  %xor378.i477.i.i = xor i32 %xor373.i472.i.i, %163
  %shl379.i478.i.i = shl i32 %xor378.i477.i.i, 1
  %shr402.i501.i.i = lshr i32 %xor378.i477.i.i, 31
  %or403.i502.i.i = or i32 %shl379.i478.i.i, %shr402.i501.i.i
  store i32 %or403.i502.i.i, i32* %arrayidx252.i351.i.i, align 4
  %add355.i454.i.i = add i32 %v2.i84.i.1.i267, 1859775393
  %add356.i455.i.i = add i32 %add355.i454.i.i, %xor354.i453.i.i
  %add408.i507.i.i = add i32 %add356.i455.i.i, %or352.i451.i.i
  %add409.i508.i.i = add i32 %add408.i507.i.i, %or403.i502.i.i
  %shr410.i509.i.i = lshr i32 %add283.i382.i.i, 2
  %shl411.i510.i.i = shl i32 %add283.i382.i.i, 30
  %or412.i511.i.i = or i32 %shr410.i509.i.i, %shl411.i510.i.i
  %shr413.i512.i.i = lshr i32 %add409.i508.i.i, 27
  %shl414.i513.i.i = shl i32 %add409.i508.i.i, 5
  %or415.i514.i.i = or i32 %shr413.i512.i.i, %shl414.i513.i.i
  %xor416.i515.i.i = xor i32 %or412.i511.i.i, %or349.i448.i.i
  %xor417.i516.i.i = xor i32 %xor416.i515.i.i, %add346.i445.i.i
  %164 = load i32* %arrayidx256.i355.i.i, align 4
  %add426.i525.i.i = add nsw i32 %ii229.i88.i.0.i270, 11
  %and427.i526.i.i = and i32 %add426.i525.i.i, 15
  %arrayidx429.i528.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and427.i526.i.i
  %165 = load i32* %arrayidx429.i528.i.i, align 4
  %xor430.i529.i.i = xor i32 %165, %164
  %add432.i531.i.i = add nsw i32 %ii229.i88.i.0.i270, 5
  %and433.i532.i.i = and i32 %add432.i531.i.i, 15
  %arrayidx435.i534.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and433.i532.i.i
  %166 = load i32* %arrayidx435.i534.i.i, align 4
  %xor436.i535.i.i = xor i32 %xor430.i529.i.i, %166
  %167 = load i32* %arrayidx309.i408.i.i, align 4
  %xor441.i540.i.i = xor i32 %xor436.i535.i.i, %167
  %shl442.i541.i.i = shl i32 %xor441.i540.i.i, 1
  %shr465.i564.i.i = lshr i32 %xor441.i540.i.i, 31
  %or466.i565.i.i = or i32 %shl442.i541.i.i, %shr465.i564.i.i
  store i32 %or466.i565.i.i, i32* %arrayidx309.i408.i.i, align 4
  %add418.i517.i.i = add i32 %or286.i385.i.i, 1859775393
  %add419.i518.i.i = add i32 %add418.i517.i.i, %xor417.i516.i.i
  %add471.i570.i.i = add i32 %add419.i518.i.i, %or415.i514.i.i
  %add472.i571.i.i = add i32 %add471.i570.i.i, %or466.i565.i.i
  %shr473.i572.i.i = lshr i32 %add346.i445.i.i, 2
  %shl474.i573.i.i = shl i32 %add346.i445.i.i, 30
  %or475.i574.i.i = or i32 %shr473.i572.i.i, %shl474.i573.i.i
  %shr476.i575.i.i = lshr i32 %add472.i571.i.i, 27
  %shl477.i576.i.i = shl i32 %add472.i571.i.i, 5
  %or478.i577.i.i = or i32 %shr476.i575.i.i, %shl477.i576.i.i
  %xor479.i578.i.i = xor i32 %or475.i574.i.i, %or412.i511.i.i
  %xor480.i579.i.i = xor i32 %xor479.i578.i.i, %add409.i508.i.i
  %168 = load i32* %arrayidx314.i413.i.i, align 4
  %add489.i588.i.i = add nsw i32 %ii229.i88.i.0.i270, 12
  %and490.i589.i.i = and i32 %add489.i588.i.i, 15
  %arrayidx492.i591.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and490.i589.i.i
  %169 = load i32* %arrayidx492.i591.i.i, align 4
  %xor493.i592.i.i = xor i32 %169, %168
  %add495.i594.i.i = add nsw i32 %ii229.i88.i.0.i270, 6
  %and496.i595.i.i = and i32 %add495.i594.i.i, 15
  %arrayidx498.i597.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and496.i595.i.i
  %170 = load i32* %arrayidx498.i597.i.i, align 4
  %xor499.i598.i.i = xor i32 %xor493.i592.i.i, %170
  %171 = load i32* %arrayidx372.i471.i.i, align 4
  %xor504.i603.i.i = xor i32 %xor499.i598.i.i, %171
  %shl505.i604.i.i = shl i32 %xor504.i603.i.i, 1
  %shr528.i627.i.i = lshr i32 %xor504.i603.i.i, 31
  %or529.i628.i.i = or i32 %shl505.i604.i.i, %shr528.i627.i.i
  store i32 %or529.i628.i.i, i32* %arrayidx372.i471.i.i, align 4
  %add481.i580.i.i = add i32 %or349.i448.i.i, 1859775393
  %add482.i581.i.i = add i32 %add481.i580.i.i, %xor480.i579.i.i
  %add534.i633.i.i = add i32 %add482.i581.i.i, %or478.i577.i.i
  %add535.i634.i.i = add i32 %add534.i633.i.i, %or529.i628.i.i
  %shr536.i635.i.i = lshr i32 %add409.i508.i.i, 2
  %shl537.i636.i.i = shl i32 %add409.i508.i.i, 30
  %or538.i637.i.i = or i32 %shr536.i635.i.i, %shl537.i636.i.i
  %cmp231.i330.i.i = icmp slt i32 %add432.i531.i.i, 40
  br i1 %cmp231.i330.i.i, label %for.body232.i638.i.i, label %for.body545.i964.i.i

for.body545.i964.i.i:                             ; preds = %for.body545.i964.i.i, %for.body232.i638.i.i
  %ii542.i89.i.0.i264 = phi i32 [ %add757.i854.i.i, %for.body545.i964.i.i ], [ 40, %for.body232.i638.i.i ]
  %v4.i86.i.2.i263 = phi i32 [ %or734.i831.i.i, %for.body545.i964.i.i ], [ %or412.i511.i.i, %for.body232.i638.i.i ]
  %v3.i85.i.2.i262 = phi i32 [ %or800.i897.i.i, %for.body545.i964.i.i ], [ %or475.i574.i.i, %for.body232.i638.i.i ]
  %v2.i84.i.2.i261 = phi i32 [ %or866.i963.i.i, %for.body545.i964.i.i ], [ %or538.i637.i.i, %for.body232.i638.i.i ]
  %v1.i83.i.2.i260 = phi i32 [ %add797.i894.i.i, %for.body545.i964.i.i ], [ %add472.i571.i.i, %for.body232.i638.i.i ]
  %v0.i82.i.2.i259 = phi i32 [ %add863.i960.i.i, %for.body545.i964.i.i ], [ %add535.i634.i.i, %for.body232.i638.i.i ]
  %shr546.i643.i.i = lshr i32 %v0.i82.i.2.i259, 27
  %shl547.i644.i.i = shl i32 %v0.i82.i.2.i259, 5
  %or548.i645.i.i = or i32 %shr546.i643.i.i, %shl547.i644.i.i
  %and550.i647.i.i23 = xor i32 %v2.i84.i.2.i261, %v3.i85.i.2.i262
  %xor551.i648.i.i = and i32 %and550.i647.i.i23, %v1.i83.i.2.i260
  %and552.i649.i.i = and i32 %v2.i84.i.2.i261, %v3.i85.i.2.i262
  %xor553.i650.i.i = xor i32 %xor551.i648.i.i, %and552.i649.i.i
  %add556.i653.i.i = add nsw i32 %ii542.i89.i.0.i264, 13
  %and557.i654.i.i = and i32 %add556.i653.i.i, 15
  %arrayidx559.i656.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and557.i654.i.i
  %172 = load i32* %arrayidx559.i656.i.i, align 4
  %add560.i657.i.i = add nsw i32 %ii542.i89.i.0.i264, 8
  %and561.i658.i.i = and i32 %add560.i657.i.i, 15
  %arrayidx563.i660.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and561.i658.i.i
  %173 = load i32* %arrayidx563.i660.i.i, align 4
  %xor564.i661.i.i = xor i32 %173, %172
  %add565.i662.i.i = add nsw i32 %ii542.i89.i.0.i264, 2
  %and566.i663.i.i = and i32 %add565.i662.i.i, 15
  %arrayidx568.i665.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and566.i663.i.i
  %174 = load i32* %arrayidx568.i665.i.i, align 4
  %xor569.i666.i.i = xor i32 %xor564.i661.i.i, %174
  %and570.i667.i.i = and i32 %ii542.i89.i.0.i264, 15
  %arrayidx572.i669.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and570.i667.i.i
  %175 = load i32* %arrayidx572.i669.i.i, align 4
  %xor573.i670.i.i = xor i32 %xor569.i666.i.i, %175
  %shl574.i671.i.i = shl i32 %xor573.i670.i.i, 1
  %shr593.i690.i.i = lshr i32 %xor573.i670.i.i, 31
  %or594.i691.i.i = or i32 %shl574.i671.i.i, %shr593.i690.i.i
  store i32 %or594.i691.i.i, i32* %arrayidx572.i669.i.i, align 4
  %add554.i651.i.i = add i32 %v4.i86.i.2.i263, -1894007588
  %add555.i652.i.i = add i32 %add554.i651.i.i, %xor553.i650.i.i
  %add598.i695.i.i = add i32 %add555.i652.i.i, %or548.i645.i.i
  %add599.i696.i.i = add i32 %add598.i695.i.i, %or594.i691.i.i
  %shr600.i697.i.i = lshr i32 %v1.i83.i.2.i260, 2
  %shl601.i698.i.i = shl i32 %v1.i83.i.2.i260, 30
  %or602.i699.i.i = or i32 %shr600.i697.i.i, %shl601.i698.i.i
  %shr603.i700.i.i = lshr i32 %add599.i696.i.i, 27
  %shl604.i701.i.i = shl i32 %add599.i696.i.i, 5
  %or605.i702.i.i = or i32 %shr603.i700.i.i, %shl604.i701.i.i
  %and607.i704.i.i28 = xor i32 %or602.i699.i.i, %v2.i84.i.2.i261
  %xor608.i705.i.i = and i32 %and607.i704.i.i28, %v0.i82.i.2.i259
  %and609.i706.i.i = and i32 %or602.i699.i.i, %v2.i84.i.2.i261
  %xor610.i707.i.i = xor i32 %xor608.i705.i.i, %and609.i706.i.i
  %add613.i710.i.i = add nsw i32 %ii542.i89.i.0.i264, 1
  %add614.i711.i.i = add nsw i32 %ii542.i89.i.0.i264, 14
  %and615.i712.i.i = and i32 %add614.i711.i.i, 15
  %arrayidx617.i714.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and615.i712.i.i
  %176 = load i32* %arrayidx617.i714.i.i, align 4
  %add619.i716.i.i = add nsw i32 %ii542.i89.i.0.i264, 9
  %and620.i717.i.i = and i32 %add619.i716.i.i, 15
  %arrayidx622.i719.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and620.i717.i.i
  %177 = load i32* %arrayidx622.i719.i.i, align 4
  %xor623.i720.i.i = xor i32 %177, %176
  %add625.i722.i.i = add nsw i32 %ii542.i89.i.0.i264, 3
  %and626.i723.i.i = and i32 %add625.i722.i.i, 15
  %arrayidx628.i725.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and626.i723.i.i
  %178 = load i32* %arrayidx628.i725.i.i, align 4
  %xor629.i726.i.i = xor i32 %xor623.i720.i.i, %178
  %and631.i728.i.i = and i32 %add613.i710.i.i, 15
  %arrayidx633.i730.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and631.i728.i.i
  %179 = load i32* %arrayidx633.i730.i.i, align 4
  %xor634.i731.i.i = xor i32 %xor629.i726.i.i, %179
  %shl635.i732.i.i = shl i32 %xor634.i731.i.i, 1
  %shr658.i755.i.i = lshr i32 %xor634.i731.i.i, 31
  %or659.i756.i.i = or i32 %shl635.i732.i.i, %shr658.i755.i.i
  store i32 %or659.i756.i.i, i32* %arrayidx633.i730.i.i, align 4
  %add611.i708.i.i = add i32 %v3.i85.i.2.i262, -1894007588
  %add612.i709.i.i = add i32 %add611.i708.i.i, %xor610.i707.i.i
  %add664.i761.i.i = add i32 %add612.i709.i.i, %or605.i702.i.i
  %add665.i762.i.i = add i32 %add664.i761.i.i, %or659.i756.i.i
  %shr666.i763.i.i = lshr i32 %v0.i82.i.2.i259, 2
  %shl667.i764.i.i = shl i32 %v0.i82.i.2.i259, 30
  %or668.i765.i.i = or i32 %shr666.i763.i.i, %shl667.i764.i.i
  %shr669.i766.i.i = lshr i32 %add665.i762.i.i, 27
  %shl670.i767.i.i = shl i32 %add665.i762.i.i, 5
  %or671.i768.i.i = or i32 %shr669.i766.i.i, %shl670.i767.i.i
  %and673.i770.i.i33 = xor i32 %or668.i765.i.i, %or602.i699.i.i
  %xor674.i771.i.i = and i32 %add599.i696.i.i, %and673.i770.i.i33
  %and675.i772.i.i = and i32 %or668.i765.i.i, %or602.i699.i.i
  %xor676.i773.i.i = xor i32 %xor674.i771.i.i, %and675.i772.i.i
  %add680.i777.i.i = add nsw i32 %ii542.i89.i.0.i264, 15
  %and681.i778.i.i = and i32 %add680.i777.i.i, 15
  %arrayidx683.i780.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and681.i778.i.i
  %180 = load i32* %arrayidx683.i780.i.i, align 4
  %add685.i782.i.i = add nsw i32 %ii542.i89.i.0.i264, 10
  %and686.i783.i.i = and i32 %add685.i782.i.i, 15
  %arrayidx688.i785.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and686.i783.i.i
  %181 = load i32* %arrayidx688.i785.i.i, align 4
  %xor689.i786.i.i = xor i32 %181, %180
  %add691.i788.i.i = add nsw i32 %ii542.i89.i.0.i264, 4
  %and692.i789.i.i = and i32 %add691.i788.i.i, 15
  %arrayidx694.i791.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and692.i789.i.i
  %182 = load i32* %arrayidx694.i791.i.i, align 4
  %xor695.i792.i.i = xor i32 %xor689.i786.i.i, %182
  %183 = load i32* %arrayidx568.i665.i.i, align 4
  %xor700.i797.i.i = xor i32 %xor695.i792.i.i, %183
  %shl701.i798.i.i = shl i32 %xor700.i797.i.i, 1
  %shr724.i821.i.i = lshr i32 %xor700.i797.i.i, 31
  %or725.i822.i.i = or i32 %shl701.i798.i.i, %shr724.i821.i.i
  store i32 %or725.i822.i.i, i32* %arrayidx568.i665.i.i, align 4
  %add677.i774.i.i = add i32 %v2.i84.i.2.i261, -1894007588
  %add678.i775.i.i = add i32 %add677.i774.i.i, %xor676.i773.i.i
  %add730.i827.i.i = add i32 %add678.i775.i.i, %or671.i768.i.i
  %add731.i828.i.i = add i32 %add730.i827.i.i, %or725.i822.i.i
  %shr732.i829.i.i = lshr i32 %add599.i696.i.i, 2
  %shl733.i830.i.i = shl i32 %add599.i696.i.i, 30
  %or734.i831.i.i = or i32 %shr732.i829.i.i, %shl733.i830.i.i
  %shr735.i832.i.i = lshr i32 %add731.i828.i.i, 27
  %shl736.i833.i.i = shl i32 %add731.i828.i.i, 5
  %or737.i834.i.i = or i32 %shr735.i832.i.i, %shl736.i833.i.i
  %and739.i836.i.i37 = xor i32 %or734.i831.i.i, %or668.i765.i.i
  %xor740.i837.i.i = and i32 %add665.i762.i.i, %and739.i836.i.i37
  %and741.i838.i.i = and i32 %or734.i831.i.i, %or668.i765.i.i
  %xor742.i839.i.i = xor i32 %xor740.i837.i.i, %and741.i838.i.i
  %184 = load i32* %arrayidx572.i669.i.i, align 4
  %add751.i848.i.i = add nsw i32 %ii542.i89.i.0.i264, 11
  %and752.i849.i.i = and i32 %add751.i848.i.i, 15
  %arrayidx754.i851.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and752.i849.i.i
  %185 = load i32* %arrayidx754.i851.i.i, align 4
  %xor755.i852.i.i = xor i32 %185, %184
  %add757.i854.i.i = add nsw i32 %ii542.i89.i.0.i264, 5
  %and758.i855.i.i = and i32 %add757.i854.i.i, 15
  %arrayidx760.i857.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and758.i855.i.i
  %186 = load i32* %arrayidx760.i857.i.i, align 4
  %xor761.i858.i.i = xor i32 %xor755.i852.i.i, %186
  %187 = load i32* %arrayidx628.i725.i.i, align 4
  %xor766.i863.i.i = xor i32 %xor761.i858.i.i, %187
  %shl767.i864.i.i = shl i32 %xor766.i863.i.i, 1
  %shr790.i887.i.i = lshr i32 %xor766.i863.i.i, 31
  %or791.i888.i.i = or i32 %shl767.i864.i.i, %shr790.i887.i.i
  store i32 %or791.i888.i.i, i32* %arrayidx628.i725.i.i, align 4
  %add743.i840.i.i = add i32 %or602.i699.i.i, -1894007588
  %add744.i841.i.i = add i32 %add743.i840.i.i, %xor742.i839.i.i
  %add796.i893.i.i = add i32 %add744.i841.i.i, %or737.i834.i.i
  %add797.i894.i.i = add i32 %add796.i893.i.i, %or791.i888.i.i
  %shr798.i895.i.i = lshr i32 %add665.i762.i.i, 2
  %shl799.i896.i.i = shl i32 %add665.i762.i.i, 30
  %or800.i897.i.i = or i32 %shr798.i895.i.i, %shl799.i896.i.i
  %shr801.i898.i.i = lshr i32 %add797.i894.i.i, 27
  %shl802.i899.i.i = shl i32 %add797.i894.i.i, 5
  %or803.i900.i.i = or i32 %shr801.i898.i.i, %shl802.i899.i.i
  %and805.i902.i.i42 = xor i32 %or800.i897.i.i, %or734.i831.i.i
  %xor806.i903.i.i = and i32 %add731.i828.i.i, %and805.i902.i.i42
  %and807.i904.i.i = and i32 %or800.i897.i.i, %or734.i831.i.i
  %xor808.i905.i.i = xor i32 %xor806.i903.i.i, %and807.i904.i.i
  %188 = load i32* %arrayidx633.i730.i.i, align 4
  %add817.i914.i.i = add nsw i32 %ii542.i89.i.0.i264, 12
  %and818.i915.i.i = and i32 %add817.i914.i.i, 15
  %arrayidx820.i917.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and818.i915.i.i
  %189 = load i32* %arrayidx820.i917.i.i, align 4
  %xor821.i918.i.i = xor i32 %189, %188
  %add823.i920.i.i = add nsw i32 %ii542.i89.i.0.i264, 6
  %and824.i921.i.i = and i32 %add823.i920.i.i, 15
  %arrayidx826.i923.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and824.i921.i.i
  %190 = load i32* %arrayidx826.i923.i.i, align 4
  %xor827.i924.i.i = xor i32 %xor821.i918.i.i, %190
  %191 = load i32* %arrayidx694.i791.i.i, align 4
  %xor832.i929.i.i = xor i32 %xor827.i924.i.i, %191
  %shl833.i930.i.i = shl i32 %xor832.i929.i.i, 1
  %shr856.i953.i.i = lshr i32 %xor832.i929.i.i, 31
  %or857.i954.i.i = or i32 %shl833.i930.i.i, %shr856.i953.i.i
  store i32 %or857.i954.i.i, i32* %arrayidx694.i791.i.i, align 4
  %add809.i906.i.i = add i32 %or668.i765.i.i, -1894007588
  %add810.i907.i.i = add i32 %add809.i906.i.i, %xor808.i905.i.i
  %add862.i959.i.i = add i32 %add810.i907.i.i, %or803.i900.i.i
  %add863.i960.i.i = add i32 %add862.i959.i.i, %or857.i954.i.i
  %shr864.i961.i.i = lshr i32 %add731.i828.i.i, 2
  %shl865.i962.i.i = shl i32 %add731.i828.i.i, 30
  %or866.i963.i.i = or i32 %shr864.i961.i.i, %shl865.i962.i.i
  %cmp544.i641.i.i = icmp slt i32 %add757.i854.i.i, 60
  br i1 %cmp544.i641.i.i, label %for.body545.i964.i.i, label %for.body873.i1275.i.i

for.body873.i1275.i.i:                            ; preds = %for.body873.i1275.i.i, %for.body545.i964.i.i
  %ii870.i90.i.0.i258 = phi i32 [ %add1073.i1168.i.i, %for.body873.i1275.i.i ], [ 60, %for.body545.i964.i.i ]
  %v4.i86.i.3.i257 = phi i32 [ %or1053.i1148.i.i, %for.body873.i1275.i.i ], [ %or734.i831.i.i, %for.body545.i964.i.i ]
  %v3.i85.i.3.i256 = phi i32 [ %or1116.i1211.i.i, %for.body873.i1275.i.i ], [ %or800.i897.i.i, %for.body545.i964.i.i ]
  %v2.i84.i.3.i255 = phi i32 [ %or1179.i1274.i.i, %for.body873.i1275.i.i ], [ %or866.i963.i.i, %for.body545.i964.i.i ]
  %v1.i83.i.3.i254 = phi i32 [ %add1113.i1208.i.i, %for.body873.i1275.i.i ], [ %add797.i894.i.i, %for.body545.i964.i.i ]
  %v0.i82.i.3.i253 = phi i32 [ %add1176.i1271.i.i, %for.body873.i1275.i.i ], [ %add863.i960.i.i, %for.body545.i964.i.i ]
  %shr874.i969.i.i = lshr i32 %v0.i82.i.3.i253, 27
  %shl875.i970.i.i = shl i32 %v0.i82.i.3.i253, 5
  %or876.i971.i.i = or i32 %shr874.i969.i.i, %shl875.i970.i.i
  %xor877.i972.i.i = xor i32 %v2.i84.i.3.i255, %v3.i85.i.3.i256
  %xor878.i973.i.i = xor i32 %xor877.i972.i.i, %v1.i83.i.3.i254
  %add881.i976.i.i = add nsw i32 %ii870.i90.i.0.i258, 13
  %and882.i977.i.i = and i32 %add881.i976.i.i, 15
  %arrayidx884.i979.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and882.i977.i.i
  %192 = load i32* %arrayidx884.i979.i.i, align 4
  %add885.i980.i.i = add nsw i32 %ii870.i90.i.0.i258, 8
  %and886.i981.i.i = and i32 %add885.i980.i.i, 15
  %arrayidx888.i983.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and886.i981.i.i
  %193 = load i32* %arrayidx888.i983.i.i, align 4
  %xor889.i984.i.i = xor i32 %193, %192
  %add890.i985.i.i = add nsw i32 %ii870.i90.i.0.i258, 2
  %and891.i986.i.i = and i32 %add890.i985.i.i, 15
  %arrayidx893.i988.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and891.i986.i.i
  %194 = load i32* %arrayidx893.i988.i.i, align 4
  %xor894.i989.i.i = xor i32 %xor889.i984.i.i, %194
  %and895.i990.i.i = and i32 %ii870.i90.i.0.i258, 15
  %arrayidx897.i992.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and895.i990.i.i
  %195 = load i32* %arrayidx897.i992.i.i, align 4
  %xor898.i993.i.i = xor i32 %xor894.i989.i.i, %195
  %shl899.i994.i.i = shl i32 %xor898.i993.i.i, 1
  %shr918.i1013.i.i = lshr i32 %xor898.i993.i.i, 31
  %or919.i1014.i.i = or i32 %shl899.i994.i.i, %shr918.i1013.i.i
  store i32 %or919.i1014.i.i, i32* %arrayidx897.i992.i.i, align 4
  %add879.i974.i.i = add i32 %v4.i86.i.3.i257, -899497514
  %add880.i975.i.i = add i32 %add879.i974.i.i, %xor878.i973.i.i
  %add923.i1018.i.i = add i32 %add880.i975.i.i, %or876.i971.i.i
  %add924.i1019.i.i = add i32 %add923.i1018.i.i, %or919.i1014.i.i
  %shr925.i1020.i.i = lshr i32 %v1.i83.i.3.i254, 2
  %shl926.i1021.i.i = shl i32 %v1.i83.i.3.i254, 30
  %or927.i1022.i.i = or i32 %shr925.i1020.i.i, %shl926.i1021.i.i
  %shr928.i1023.i.i = lshr i32 %add924.i1019.i.i, 27
  %shl929.i1024.i.i = shl i32 %add924.i1019.i.i, 5
  %or930.i1025.i.i = or i32 %shr928.i1023.i.i, %shl929.i1024.i.i
  %xor931.i1026.i.i = xor i32 %v0.i82.i.3.i253, %v2.i84.i.3.i255
  %xor932.i1027.i.i = xor i32 %xor931.i1026.i.i, %or927.i1022.i.i
  %add935.i1030.i.i = add nsw i32 %ii870.i90.i.0.i258, 1
  %add936.i1031.i.i = add nsw i32 %ii870.i90.i.0.i258, 14
  %and937.i1032.i.i = and i32 %add936.i1031.i.i, 15
  %arrayidx939.i1034.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and937.i1032.i.i
  %196 = load i32* %arrayidx939.i1034.i.i, align 4
  %add941.i1036.i.i = add nsw i32 %ii870.i90.i.0.i258, 9
  %and942.i1037.i.i = and i32 %add941.i1036.i.i, 15
  %arrayidx944.i1039.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and942.i1037.i.i
  %197 = load i32* %arrayidx944.i1039.i.i, align 4
  %xor945.i1040.i.i = xor i32 %197, %196
  %add947.i1042.i.i = add nsw i32 %ii870.i90.i.0.i258, 3
  %and948.i1043.i.i = and i32 %add947.i1042.i.i, 15
  %arrayidx950.i1045.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and948.i1043.i.i
  %198 = load i32* %arrayidx950.i1045.i.i, align 4
  %xor951.i1046.i.i = xor i32 %xor945.i1040.i.i, %198
  %and953.i1048.i.i = and i32 %add935.i1030.i.i, 15
  %arrayidx955.i1050.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and953.i1048.i.i
  %199 = load i32* %arrayidx955.i1050.i.i, align 4
  %xor956.i1051.i.i = xor i32 %xor951.i1046.i.i, %199
  %shl957.i1052.i.i = shl i32 %xor956.i1051.i.i, 1
  %shr980.i1075.i.i = lshr i32 %xor956.i1051.i.i, 31
  %or981.i1076.i.i = or i32 %shl957.i1052.i.i, %shr980.i1075.i.i
  store i32 %or981.i1076.i.i, i32* %arrayidx955.i1050.i.i, align 4
  %add933.i1028.i.i = add i32 %v3.i85.i.3.i256, -899497514
  %add934.i1029.i.i = add i32 %add933.i1028.i.i, %xor932.i1027.i.i
  %add986.i1081.i.i = add i32 %add934.i1029.i.i, %or930.i1025.i.i
  %add987.i1082.i.i = add i32 %add986.i1081.i.i, %or981.i1076.i.i
  %shr988.i1083.i.i = lshr i32 %v0.i82.i.3.i253, 2
  %shl989.i1084.i.i = shl i32 %v0.i82.i.3.i253, 30
  %or990.i1085.i.i = or i32 %shr988.i1083.i.i, %shl989.i1084.i.i
  %shr991.i1086.i.i = lshr i32 %add987.i1082.i.i, 27
  %shl992.i1087.i.i = shl i32 %add987.i1082.i.i, 5
  %or993.i1088.i.i = or i32 %shr991.i1086.i.i, %shl992.i1087.i.i
  %xor994.i1089.i.i = xor i32 %or990.i1085.i.i, %or927.i1022.i.i
  %xor995.i1090.i.i = xor i32 %xor994.i1089.i.i, %add924.i1019.i.i
  %add999.i1094.i.i = add nsw i32 %ii870.i90.i.0.i258, 15
  %and1000.i1095.i.i = and i32 %add999.i1094.i.i, 15
  %arrayidx1002.i1097.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1000.i1095.i.i
  %200 = load i32* %arrayidx1002.i1097.i.i, align 4
  %add1004.i1099.i.i = add nsw i32 %ii870.i90.i.0.i258, 10
  %and1005.i1100.i.i = and i32 %add1004.i1099.i.i, 15
  %arrayidx1007.i1102.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1005.i1100.i.i
  %201 = load i32* %arrayidx1007.i1102.i.i, align 4
  %xor1008.i1103.i.i = xor i32 %201, %200
  %add1010.i1105.i.i = add nsw i32 %ii870.i90.i.0.i258, 4
  %and1011.i1106.i.i = and i32 %add1010.i1105.i.i, 15
  %arrayidx1013.i1108.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1011.i1106.i.i
  %202 = load i32* %arrayidx1013.i1108.i.i, align 4
  %xor1014.i1109.i.i = xor i32 %xor1008.i1103.i.i, %202
  %203 = load i32* %arrayidx893.i988.i.i, align 4
  %xor1019.i1114.i.i = xor i32 %xor1014.i1109.i.i, %203
  %shl1020.i1115.i.i = shl i32 %xor1019.i1114.i.i, 1
  %shr1043.i1138.i.i = lshr i32 %xor1019.i1114.i.i, 31
  %or1044.i1139.i.i = or i32 %shl1020.i1115.i.i, %shr1043.i1138.i.i
  store i32 %or1044.i1139.i.i, i32* %arrayidx893.i988.i.i, align 4
  %add996.i1091.i.i = add i32 %v2.i84.i.3.i255, -899497514
  %add997.i1092.i.i = add i32 %add996.i1091.i.i, %xor995.i1090.i.i
  %add1049.i1144.i.i = add i32 %add997.i1092.i.i, %or993.i1088.i.i
  %add1050.i1145.i.i = add i32 %add1049.i1144.i.i, %or1044.i1139.i.i
  %shr1051.i1146.i.i = lshr i32 %add924.i1019.i.i, 2
  %shl1052.i1147.i.i = shl i32 %add924.i1019.i.i, 30
  %or1053.i1148.i.i = or i32 %shr1051.i1146.i.i, %shl1052.i1147.i.i
  %shr1054.i1149.i.i = lshr i32 %add1050.i1145.i.i, 27
  %shl1055.i1150.i.i = shl i32 %add1050.i1145.i.i, 5
  %or1056.i1151.i.i = or i32 %shr1054.i1149.i.i, %shl1055.i1150.i.i
  %xor1057.i1152.i.i = xor i32 %or1053.i1148.i.i, %or990.i1085.i.i
  %xor1058.i1153.i.i = xor i32 %xor1057.i1152.i.i, %add987.i1082.i.i
  %204 = load i32* %arrayidx897.i992.i.i, align 4
  %add1067.i1162.i.i = add nsw i32 %ii870.i90.i.0.i258, 11
  %and1068.i1163.i.i = and i32 %add1067.i1162.i.i, 15
  %arrayidx1070.i1165.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1068.i1163.i.i
  %205 = load i32* %arrayidx1070.i1165.i.i, align 4
  %xor1071.i1166.i.i = xor i32 %205, %204
  %add1073.i1168.i.i = add nsw i32 %ii870.i90.i.0.i258, 5
  %and1074.i1169.i.i = and i32 %add1073.i1168.i.i, 15
  %arrayidx1076.i1171.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1074.i1169.i.i
  %206 = load i32* %arrayidx1076.i1171.i.i, align 4
  %xor1077.i1172.i.i = xor i32 %xor1071.i1166.i.i, %206
  %207 = load i32* %arrayidx950.i1045.i.i, align 4
  %xor1082.i1177.i.i = xor i32 %xor1077.i1172.i.i, %207
  %shl1083.i1178.i.i = shl i32 %xor1082.i1177.i.i, 1
  %shr1106.i1201.i.i = lshr i32 %xor1082.i1177.i.i, 31
  %or1107.i1202.i.i = or i32 %shl1083.i1178.i.i, %shr1106.i1201.i.i
  store i32 %or1107.i1202.i.i, i32* %arrayidx950.i1045.i.i, align 4
  %add1059.i1154.i.i = add i32 %or927.i1022.i.i, -899497514
  %add1060.i1155.i.i = add i32 %add1059.i1154.i.i, %xor1058.i1153.i.i
  %add1112.i1207.i.i = add i32 %add1060.i1155.i.i, %or1056.i1151.i.i
  %add1113.i1208.i.i = add i32 %add1112.i1207.i.i, %or1107.i1202.i.i
  %shr1114.i1209.i.i = lshr i32 %add987.i1082.i.i, 2
  %shl1115.i1210.i.i = shl i32 %add987.i1082.i.i, 30
  %or1116.i1211.i.i = or i32 %shr1114.i1209.i.i, %shl1115.i1210.i.i
  %shr1117.i1212.i.i = lshr i32 %add1113.i1208.i.i, 27
  %shl1118.i1213.i.i = shl i32 %add1113.i1208.i.i, 5
  %or1119.i1214.i.i = or i32 %shr1117.i1212.i.i, %shl1118.i1213.i.i
  %xor1120.i1215.i.i = xor i32 %or1116.i1211.i.i, %or1053.i1148.i.i
  %xor1121.i1216.i.i = xor i32 %xor1120.i1215.i.i, %add1050.i1145.i.i
  %208 = load i32* %arrayidx955.i1050.i.i, align 4
  %add1130.i1225.i.i = add nsw i32 %ii870.i90.i.0.i258, 12
  %and1131.i1226.i.i = and i32 %add1130.i1225.i.i, 15
  %arrayidx1133.i1228.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1131.i1226.i.i
  %209 = load i32* %arrayidx1133.i1228.i.i, align 4
  %xor1134.i1229.i.i = xor i32 %209, %208
  %add1136.i1231.i.i = add nsw i32 %ii870.i90.i.0.i258, 6
  %and1137.i1232.i.i = and i32 %add1136.i1231.i.i, 15
  %arrayidx1139.i1234.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1137.i1232.i.i
  %210 = load i32* %arrayidx1139.i1234.i.i, align 4
  %xor1140.i1235.i.i = xor i32 %xor1134.i1229.i.i, %210
  %211 = load i32* %arrayidx1013.i1108.i.i, align 4
  %xor1145.i1240.i.i = xor i32 %xor1140.i1235.i.i, %211
  %shl1146.i1241.i.i = shl i32 %xor1145.i1240.i.i, 1
  %shr1169.i1264.i.i = lshr i32 %xor1145.i1240.i.i, 31
  %or1170.i1265.i.i = or i32 %shl1146.i1241.i.i, %shr1169.i1264.i.i
  store i32 %or1170.i1265.i.i, i32* %arrayidx1013.i1108.i.i, align 4
  %add1122.i1217.i.i = add i32 %or990.i1085.i.i, -899497514
  %add1123.i1218.i.i = add i32 %add1122.i1217.i.i, %xor1121.i1216.i.i
  %add1175.i1270.i.i = add i32 %add1123.i1218.i.i, %or1119.i1214.i.i
  %add1176.i1271.i.i = add i32 %add1175.i1270.i.i, %or1170.i1265.i.i
  %shr1177.i1272.i.i = lshr i32 %add1050.i1145.i.i, 2
  %shl1178.i1273.i.i = shl i32 %add1050.i1145.i.i, 30
  %or1179.i1274.i.i = or i32 %shr1177.i1272.i.i, %shl1178.i1273.i.i
  %cmp872.i967.i.i = icmp slt i32 %add1073.i1168.i.i, 80
  br i1 %cmp872.i967.i.i, label %for.body873.i1275.i.i, label %sha1_compile.exit1292.i.i

sha1_compile.exit1292.i.i:                        ; preds = %for.body873.i1275.i.i
  %212 = load i32* %arrayidx3.i.i, align 4
  %add1185.i1279.i.i = add i32 %212, %add1176.i1271.i.i
  store i32 %add1185.i1279.i.i, i32* %arrayidx3.i.i, align 4
  %213 = load i32* %arrayidx5.i.i, align 4
  %add1188.i1282.i.i = add i32 %213, %add1113.i1208.i.i
  store i32 %add1188.i1282.i.i, i32* %arrayidx5.i.i, align 4
  %214 = load i32* %arrayidx7.i.i, align 4
  %add1191.i1285.i.i = add i32 %214, %or1179.i1274.i.i
  store i32 %add1191.i1285.i.i, i32* %arrayidx7.i.i, align 4
  %215 = load i32* %arrayidx9.i.i, align 4
  %add1194.i1288.i.i = add i32 %215, %or1116.i1211.i.i
  store i32 %add1194.i1288.i.i, i32* %arrayidx9.i.i, align 4
  %216 = load i32* %arrayidx11.i.i, align 4
  %add1197.i1291.i.i = add i32 %216, %or1053.i1148.i.i
  store i32 %add1197.i1291.i.i, i32* %arrayidx11.i.i, align 4
  br label %for.body.i.i

for.body.i.i:                                     ; preds = %for.body.i.i, %sha1_compile.exit1292.i.i
  %i.i.1.i252 = phi i32 [ 0, %sha1_compile.exit1292.i.i ], [ %inc79.i.i, %for.body.i.i ]
  %shr70.i.i = lshr i32 %i.i.1.i252, 2
  %arrayidx72.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 1, i32 %shr70.i.i
  %217 = load i32* %arrayidx72.i.i, align 4
  %neg73.i.i = shl i32 %i.i.1.i252, 3
  %and74.i.i = and i32 %neg73.i.i, 24
  %mul75.i.i = xor i32 %and74.i.i, 24
  %shr76.i.i = lshr i32 %217, %mul75.i.i
  %conv.i.i = trunc i32 %shr76.i.i to i8
  %arrayidx78.i.i = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 %i.i.1.i252
  store i8 %conv.i.i, i8* %arrayidx78.i.i, align 1
  %inc79.i.i = add i32 %i.i.1.i252, 1
  %exitcond = icmp eq i32 %inc79.i.i, 20
  br i1 %exitcond, label %rng_spawn.exit, label %for.body.i.i

rng_spawn.exit:                                   ; preds = %for.body.i.i
  %diff = sub i32 %0, %1
  %218 = load i32* %9, align 4
  %219 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 0
  store i32 %218, i32* %219, align 8
  %220 = load i32* %13, align 4
  %221 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 1
  store i32 %220, i32* %221, align 4
  %222 = load i32* %14, align 4
  %223 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 2
  store i32 %222, i32* %223, align 8
  %224 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 0
  %225 = load i8* %224, align 1
  %226 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 0
  store i8 %225, i8* %226, align 4
  %227 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 1
  %228 = load i8* %227, align 1
  %229 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 1
  store i8 %228, i8* %229, align 1
  %230 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 2
  %231 = load i8* %230, align 1
  %232 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 2
  store i8 %231, i8* %232, align 2
  %233 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 3
  %234 = load i8* %233, align 1
  %235 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 3
  store i8 %234, i8* %235, align 1
  %236 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 4
  %237 = load i8* %236, align 1
  %238 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 4
  store i8 %237, i8* %238, align 4
  %239 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 5
  %240 = load i8* %239, align 1
  %241 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 5
  store i8 %240, i8* %241, align 1
  %242 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 6
  %243 = load i8* %242, align 1
  %244 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 6
  store i8 %243, i8* %244, align 2
  %245 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 7
  %246 = load i8* %245, align 1
  %247 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 7
  store i8 %246, i8* %247, align 1
  %248 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 8
  %249 = load i8* %248, align 1
  %250 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 8
  store i8 %249, i8* %250, align 4
  %251 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 9
  %252 = load i8* %251, align 1
  %253 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 9
  store i8 %252, i8* %253, align 1
  %254 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 10
  %255 = load i8* %254, align 1
  %256 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 10
  store i8 %255, i8* %256, align 2
  %257 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 11
  %258 = load i8* %257, align 1
  %259 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 11
  store i8 %258, i8* %259, align 1
  %260 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 12
  %261 = load i8* %260, align 1
  %262 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 12
  store i8 %261, i8* %262, align 4
  %263 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 13
  %264 = load i8* %263, align 1
  %265 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 13
  store i8 %264, i8* %265, align 1
  %266 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 14
  %267 = load i8* %266, align 1
  %268 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 14
  store i8 %267, i8* %268, align 2
  %269 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 15
  %270 = load i8* %269, align 1
  %271 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 15
  store i8 %270, i8* %271, align 1
  %272 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 16
  %273 = load i8* %272, align 1
  %274 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 16
  store i8 %273, i8* %274, align 4
  %275 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 17
  %276 = load i8* %275, align 1
  %277 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 17
  store i8 %276, i8* %277, align 1
  %278 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 18
  %279 = load i8* %278, align 1
  %280 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 18
  store i8 %279, i8* %280, align 2
  %281 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 19
  %282 = load i8* %281, align 1
  %283 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 19
  store i8 %282, i8* %283, align 1
  %falloc_reg = call i32 @llvm.falloc(i32 0)
  call void @llvm.fbind(i32 6, i32 %falloc_reg)
  %falloc_reg1 = call i32 @llvm.falloc(i32 0)
  call void @llvm.fbind(i32 14, i32 %falloc_reg1)
  %memframe_reg = call i8* @llvm.getmemframe(i32 %falloc_reg1)
  %284 = bitcast %struct.node_t* %8 to i8*
  br label %for.body.i

for.body.i:                                       ; preds = %for.body.i, %rng_spawn.exit
  %indvars.iv.i = phi i32 [ %indvars.iv.next.i, %for.body.i ], [ 0, %rng_spawn.exit ]
  %arrayidx.i = getelementptr inbounds i8* %284, i32 %indvars.iv.i
  %285 = load i8* %arrayidx.i, align 1
  %arrayidx2.i = getelementptr inbounds i8* %memframe_reg, i32 %indvars.iv.i
  store i8 %285, i8* %arrayidx2.i, align 1
  %indvars.iv.next.i = add i32 %indvars.iv.i, 1
  %exitcond.i = icmp eq i32 %indvars.iv.next.i, 32
  br i1 %exitcond.i, label %redefine_memcpy.exit, label %for.body.i

redefine_memcpy.exit:                             ; preds = %for.body.i
  call void @llvm.writecm(i32 %falloc_reg1, i32 0, i32 %falloc_reg)
  call void @llvm.writecm(i32 %falloc_reg, i32 20, i32 %diff)
  call void @llvm.writecm(i32 %falloc_reg, i32 16, i32 %2)
  ret void
}

; Function Attrs: nounwind
define void @uts_function111111(i32 inreg, i32 inreg) #1 {
uts_function11.if.else:
  tail call void @llvm.writecm(i32 %1, i32 24, i32 1)
  ret void
}

; Function Attrs: nounwind
define void @uts_function121111(i32 inreg, i32 inreg, i32 inreg, i32 inreg) #1 {
uts_function12.entry:
  %4 = icmp sgt i32 %3, 0
  %5 = shl i32 %1, 6
  %6 = add i32 %5, -64
  %7 = add nuw i32 %6, %2
  %8 = zext i1 %4 to i32
  br label %uts_function_for.body1511111_create_body

uts_function_for.body1511111_create_body:         ; preds = %uts_function_for.body1511111_create_body, %uts_function12.entry
  %9 = phi i32 [ %2, %uts_function12.entry ], [ %10, %uts_function_for.body1511111_create_body ]
  tail call void @llvm.writecm(i32 %9, i32 12, i32 0)
  tail call void @llvm.writecmp(i32 %9, i32 %8)
  %10 = add nuw i32 %9, 64
  %cmpinst = icmp ugt i32 %10, %7
  br i1 %cmpinst, label %uts_function_for.body1511111_create_end, label %uts_function_for.body1511111_create_body

uts_function_for.body1511111_create_end:          ; preds = %uts_function_for.body1511111_create_body
  ret void
}

; Function Attrs: alwaysinline nounwind ssp
define void @uts_function131111(i32 inreg, i32 inreg, %struct.node_t*) #0 {
uts_function13.entry:
  %3 = alloca %struct.node_t, align 8
  %4 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 0
  %5 = load i32* %4, align 4
  %6 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 0
  store i32 %5, i32* %6, align 8
  %7 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 1
  %8 = load i32* %7, align 4
  %9 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 1
  store i32 %8, i32* %9, align 4
  %10 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 2
  %11 = load i32* %10, align 4
  %12 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 2
  store i32 %11, i32* %12, align 8
  %13 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 0
  %14 = load i8* %13, align 1
  %15 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 0
  store i8 %14, i8* %15, align 4
  %16 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 1
  %17 = load i8* %16, align 1
  %18 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 1
  store i8 %17, i8* %18, align 1
  %19 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 2
  %20 = load i8* %19, align 1
  %21 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 2
  store i8 %20, i8* %21, align 2
  %22 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 3
  %23 = load i8* %22, align 1
  %24 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 3
  store i8 %23, i8* %24, align 1
  %25 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 4
  %26 = load i8* %25, align 1
  %27 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 4
  store i8 %26, i8* %27, align 4
  %28 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 5
  %29 = load i8* %28, align 1
  %30 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 5
  store i8 %29, i8* %30, align 1
  %31 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 6
  %32 = load i8* %31, align 1
  %33 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 6
  store i8 %32, i8* %33, align 2
  %34 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 7
  %35 = load i8* %34, align 1
  %36 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 7
  store i8 %35, i8* %36, align 1
  %37 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 8
  %38 = load i8* %37, align 1
  %39 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 8
  store i8 %38, i8* %39, align 4
  %40 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 9
  %41 = load i8* %40, align 1
  %42 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 9
  store i8 %41, i8* %42, align 1
  %43 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 10
  %44 = load i8* %43, align 1
  %45 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 10
  store i8 %44, i8* %45, align 2
  %46 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 11
  %47 = load i8* %46, align 1
  %48 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 11
  store i8 %47, i8* %48, align 1
  %49 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 12
  %50 = load i8* %49, align 1
  %51 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 12
  store i8 %50, i8* %51, align 4
  %52 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 13
  %53 = load i8* %52, align 1
  %54 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 13
  store i8 %53, i8* %54, align 1
  %55 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 14
  %56 = load i8* %55, align 1
  %57 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 14
  store i8 %56, i8* %57, align 2
  %58 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 15
  %59 = load i8* %58, align 1
  %60 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 15
  store i8 %59, i8* %60, align 1
  %61 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 16
  %62 = load i8* %61, align 1
  %63 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 16
  store i8 %62, i8* %63, align 4
  %64 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 17
  %65 = load i8* %64, align 1
  %66 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 17
  store i8 %65, i8* %66, align 1
  %67 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 18
  %68 = load i8* %67, align 1
  %69 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 18
  store i8 %68, i8* %69, align 2
  %70 = getelementptr inbounds %struct.node_t* %2, i32 0, i32 3, i32 0, i32 19
  %71 = load i8* %70, align 1
  %72 = getelementptr inbounds %struct.node_t* %3, i32 0, i32 3, i32 0, i32 19
  store i8 %71, i8* %72, align 1
  %73 = alloca %struct.node_t, align 4
  %74 = load float* @b_0, align 4
  %cmp.i.i = icmp sgt i32 %8, 0
  br i1 %cmp.i.i, label %if.then.i.i, label %if.end.i.i

if.then.i.i:                                      ; preds = %uts_function13.entry
  %conv.i.i = sitofp i32 %8 to float
  %75 = load i32* @gen_mx, align 4
  %conv1.i.i = sitofp i32 %75 to float
  %mul.i.i.i = fmul float %conv1.i.i, %conv1.i.i
  %76 = bitcast float %mul.i.i.i to i32
  %mul.i.i.i.i = fmul float %mul.i.i.i, 5.000000e-01
  %shr.i.i.i.i = ashr i32 %76, 1
  %sub.i.i.i.i = sub nsw i32 1597463007, %shr.i.i.i.i
  %77 = bitcast i32 %sub.i.i.i.i to float
  %mul1.i.i.i.i = fmul float %mul.i.i.i.i, %77
  %mul2.i.i.i.i = fmul float %77, %mul1.i.i.i.i
  %sub3.i.i.i.i = fsub float 1.500000e+00, %mul2.i.i.i.i
  %mul4.i.i.i.i = fmul float %77, %sub3.i.i.i.i
  %cmp.i.i.i = fcmp olt float %conv1.i.i, 0.000000e+00
  br i1 %cmp.i.i.i, label %if.then.i.i.i, label %redefine_div.exit.i.i

if.then.i.i.i:                                    ; preds = %if.then.i.i
  %sub.i.i.i = fsub float 0.000000e+00, %mul4.i.i.i.i
  br label %redefine_div.exit.i.i

redefine_div.exit.i.i:                            ; preds = %if.then.i.i.i, %if.then.i.i
  %inv.i.i.0.i = phi float [ %sub.i.i.i, %if.then.i.i.i ], [ %mul4.i.i.i.i, %if.then.i.i ]
  %mul1.i.i.i = fmul float %conv.i.i, %inv.i.i.0.i
  %sub.i.i = fsub float 1.000000e+00, %mul1.i.i.i
  %mul.i.i = fmul float %74, %sub.i.i
  br label %if.end.i.i

if.end.i.i:                                       ; preds = %redefine_div.exit.i.i, %uts_function13.entry
  %b_i.i.0.i = phi float [ %mul.i.i, %redefine_div.exit.i.i ], [ %74, %uts_function13.entry ]
  %add.i.i = fadd float %b_i.i.0.i, 1.000000e+00
  %mul.i263.i.i = fmul float %add.i.i, %add.i.i
  %78 = bitcast float %mul.i263.i.i to i32
  %mul.i.i264.i.i = fmul float %mul.i263.i.i, 5.000000e-01
  %shr.i.i265.i.i = ashr i32 %78, 1
  %sub.i.i266.i.i = sub nsw i32 1597463007, %shr.i.i265.i.i
  %79 = bitcast i32 %sub.i.i266.i.i to float
  %mul1.i.i267.i.i = fmul float %mul.i.i264.i.i, %79
  %mul2.i.i268.i.i = fmul float %79, %mul1.i.i267.i.i
  %sub3.i.i269.i.i = fsub float 1.500000e+00, %mul2.i.i268.i.i
  %mul4.i.i270.i.i = fmul float %79, %sub3.i.i269.i.i
  %cmp.i271.i.i = fcmp olt float %add.i.i, 0.000000e+00
  br i1 %cmp.i271.i.i, label %if.then.i273.i.i, label %redefine_div.exit276.i.i

if.then.i273.i.i:                                 ; preds = %if.end.i.i
  %sub.i272.i.i = fsub float 0.000000e+00, %mul4.i.i270.i.i
  br label %redefine_div.exit276.i.i

redefine_div.exit276.i.i:                         ; preds = %if.then.i273.i.i, %if.end.i.i
  %inv.i262.i.0.i = phi float [ %sub.i272.i.i, %if.then.i273.i.i ], [ %mul4.i.i270.i.i, %if.end.i.i ]
  %conv.i254.i.i = zext i8 %62 to i32
  %shl.i.i.i = shl nuw i32 %conv.i254.i.i, 24
  %conv2.i.i.i = zext i8 %65 to i32
  %shl3.i.i.i = shl nuw nsw i32 %conv2.i.i.i, 16
  %conv5.i.i.i = zext i8 %68 to i32
  %shl6.i.i.i = shl nuw nsw i32 %conv5.i.i.i, 8
  %conv9.i.i.i = zext i8 %71 to i32
  %shl.i.i.i.masked = and i32 %shl.i.i.i, 2130706432
  %or.i255.i.i.masked = or i32 %shl.i.i.i.masked, %shl3.i.i.i
  %or7.i.i.i.masked = or i32 %or.i255.i.i.masked, %shl6.i.i.i
  %and.i256.i.i = or i32 %or7.i.i.i.masked, %conv9.i.i.i
  %conv.i250.i.i = sitofp i32 %and.i256.i.i to float
  %mul1.i.i253.i.i = fmul float %conv.i250.i.i, 0x3DFFF221E0000000
  %sub6.i.i = fsub float 1.000000e+00, %mul1.i.i253.i.i
  %80 = bitcast float %sub6.i.i to i32
  %cmp.i105.i.i = icmp slt i32 %80, 8388608
  br i1 %cmp.i105.i.i, label %if.then.i108.i.i, label %if.end17.i144.i.i

if.then.i108.i.i:                                 ; preds = %redefine_div.exit276.i.i
  %and.i106.i.i = and i32 %80, 2147483647
  %cmp1.i107.i.i = icmp eq i32 %and.i106.i.i, 0
  br i1 %cmp1.i107.i.i, label %redefine_logf.exit247.i.i, label %if.end.i119.i.i

if.end.i119.i.i:                                  ; preds = %if.then.i108.i.i
  %cmp3.i118.i.i = icmp slt i32 %80, 0
  br i1 %cmp3.i118.i.i, label %if.then4.i137.i.i, label %if.end9.i140.i.i

if.then4.i137.i.i:                                ; preds = %if.end.i119.i.i
  %sub.i120.i.i = fsub float %sub6.i.i, %sub6.i.i
  %mul6.i128.i.i = fmul float %sub.i120.i.i, 0x43F1306CE0000000
  %mul8.i136.i.i = fmul float %mul6.i128.i.i, 0x43F1306CE0000000
  br label %redefine_logf.exit247.i.i

if.end9.i140.i.i:                                 ; preds = %if.end.i119.i.i
  %mul11.i139.i.i = fmul float %sub6.i.i, 0x4180000000000000
  %81 = bitcast float %mul11.i139.i.i to i32
  br label %if.end17.i144.i.i

if.end17.i144.i.i:                                ; preds = %if.end9.i140.i.i, %redefine_div.exit276.i.i
  %ix.i97.i.0.i = phi i32 [ %81, %if.end9.i140.i.i ], [ %80, %redefine_div.exit276.i.i ]
  %k.i96.i.0.i = phi i32 [ -25, %if.end9.i140.i.i ], [ 0, %redefine_div.exit276.i.i ]
  %x.addr.i86.i.0.i = phi float [ %mul11.i139.i.i, %if.end9.i140.i.i ], [ %sub6.i.i, %redefine_div.exit276.i.i ]
  %cmp18.i143.i.i = icmp sgt i32 %ix.i97.i.0.i, 2139095039
  br i1 %cmp18.i143.i.i, label %if.then19.i146.i.i, label %if.end20.i153.i.i

if.then19.i146.i.i:                               ; preds = %if.end17.i144.i.i
  %add.i145.i.i = fadd float %x.addr.i86.i.0.i, %x.addr.i86.i.0.i
  br label %redefine_logf.exit247.i.i

if.end20.i153.i.i:                                ; preds = %if.end17.i144.i.i
  %shr.i147.i.i = ashr i32 %ix.i97.i.0.i, 23
  %sub21.i148.i.i = add nsw i32 %shr.i147.i.i, -127
  %add22.i149.i.i = add nsw i32 %sub21.i148.i.i, %k.i96.i.0.i
  %82 = and i32 %ix.i97.i.0.i, 8388607
  %add24.i151.i.i = add nsw i32 %82, 4913952
  %and25.i152.i.i = and i32 %add24.i151.i.i, 8388608
  %and23.i150.i.i = or i32 %and25.i152.i.i, %82
  %or.i155.i.i = xor i32 %and23.i150.i.i, 1065353216
  %83 = bitcast i32 %or.i155.i.i to float
  %shr30.i158.i.i14 = lshr exact i32 %and25.i152.i.i, 23
  %add31.i159.i.i = add nsw i32 %add22.i149.i.i, %shr30.i158.i.i14
  %sub32.i160.i.i = fadd float %83, -1.000000e+00
  %add33.i161.i.i = add nsw i32 %ix.i97.i.0.i, 32768
  %and34.i162.i.i = and i32 %add33.i161.i.i, 8372224
  %cmp35.i163.i.i = icmp ult i32 %and34.i162.i.i, 49152
  br i1 %cmp35.i163.i.i, label %if.then36.i165.i.i, label %if.end60.i214.i.i

if.then36.i165.i.i:                               ; preds = %if.end20.i153.i.i
  %cmp37.i164.i.i = fcmp oeq float %sub32.i160.i.i, 0.000000e+00
  br i1 %cmp37.i164.i.i, label %if.then38.i167.i.i, label %if.end44.i179.i.i

if.then38.i167.i.i:                               ; preds = %if.then36.i165.i.i
  %cmp39.i166.i.i = icmp eq i32 %add31.i159.i.i, 0
  br i1 %cmp39.i166.i.i, label %redefine_logf.exit247.i.i, label %if.else.i173.i.i

if.else.i173.i.i:                                 ; preds = %if.then38.i167.i.i
  %conv.i169.i.i = sitofp i32 %add31.i159.i.i to float
  %mul41.i170.i.i = fmul float %conv.i169.i.i, 0x3FE62E3000000000
  %mul42.i171.i.i = fmul float %conv.i169.i.i, 0x3EE2FEFA20000000
  %add43.i172.i.i = fadd float %mul41.i170.i.i, %mul42.i171.i.i
  br label %redefine_logf.exit247.i.i

if.end44.i179.i.i:                                ; preds = %if.then36.i165.i.i
  %mul45.i174.i.i = fmul float %sub32.i160.i.i, %sub32.i160.i.i
  %mul46.i175.i.i = fmul float %sub32.i160.i.i, 0x3FD5555560000000
  %sub47.i176.i.i = fsub float 5.000000e-01, %mul46.i175.i.i
  %mul48.i177.i.i = fmul float %mul45.i174.i.i, %sub47.i176.i.i
  %cmp49.i178.i.i = icmp eq i32 %add31.i159.i.i, 0
  br i1 %cmp49.i178.i.i, label %if.then51.i181.i.i, label %if.else53.i188.i.i

if.then51.i181.i.i:                               ; preds = %if.end44.i179.i.i
  %sub52.i180.i.i = fsub float %sub32.i160.i.i, %mul48.i177.i.i
  br label %redefine_logf.exit247.i.i

if.else53.i188.i.i:                               ; preds = %if.end44.i179.i.i
  %conv54.i182.i.i = sitofp i32 %add31.i159.i.i to float
  %mul55.i183.i.i = fmul float %conv54.i182.i.i, 0x3FE62E3000000000
  %mul56.i184.i.i = fmul float %conv54.i182.i.i, 0x3EE2FEFA20000000
  %sub57.i185.i.i = fsub float %mul48.i177.i.i, %mul56.i184.i.i
  %sub58.i186.i.i = fsub float %sub57.i185.i.i, %sub32.i160.i.i
  %sub59.i187.i.i = fsub float %mul55.i183.i.i, %sub58.i186.i.i
  br label %redefine_logf.exit247.i.i

if.end60.i214.i.i:                                ; preds = %if.end20.i153.i.i
  %add61.i189.i.i = fadd float %sub32.i160.i.i, 2.000000e+00
  %mul63.i191.i.i = fmul float %add61.i189.i.i, %add61.i189.i.i
  %84 = bitcast float %mul63.i191.i.i to i32
  %mul.i118.i192.i.i = fmul float %mul63.i191.i.i, 5.000000e-01
  %shr.i119.i193.i.i = ashr i32 %84, 1
  %sub.i120.i194.i.i = sub nsw i32 1597463007, %shr.i119.i193.i.i
  %85 = bitcast i32 %sub.i120.i194.i.i to float
  %mul1.i121.i195.i.i = fmul float %mul.i118.i192.i.i, %85
  %mul2.i122.i196.i.i = fmul float %85, %mul1.i121.i195.i.i
  %sub3.i123.i197.i.i = fsub float 1.500000e+00, %mul2.i122.i196.i.i
  %mul4.i124.i198.i.i = fmul float %85, %sub3.i123.i197.i.i
  %mul65.i199.i.i = fmul float %sub32.i160.i.i, %mul4.i124.i198.i.i
  %conv66.i200.i.i = sitofp i32 %add31.i159.i.i to float
  %mul67.i201.i.i = fmul float %mul65.i199.i.i, %mul65.i199.i.i
  %sub68.i202.i.i = add nsw i32 %82, -3187664
  %mul69.i203.i.i = fmul float %mul67.i201.i.i, %mul67.i201.i.i
  %sub70.i204.i.i = sub nsw i32 3523208, %82
  %mul71.i205.i.i = fmul float %mul69.i203.i.i, 0x3FCF13C4C0000000
  %add72.i206.i.i = fadd float %mul71.i205.i.i, 0x3FD999C260000000
  %mul73.i207.i.i = fmul float %mul69.i203.i.i, %add72.i206.i.i
  %mul74.i208.i.i = fmul float %mul69.i203.i.i, 0x3FD23D3DC0000000
  %add75.i209.i.i = fadd float %mul74.i208.i.i, 0x3FE5555540000000
  %mul76.i210.i.i = fmul float %mul67.i201.i.i, %add75.i209.i.i
  %or77.i211.i.i = or i32 %sub68.i202.i.i, %sub70.i204.i.i
  %add78.i212.i.i = fadd float %mul76.i210.i.i, %mul73.i207.i.i
  %cmp79.i213.i.i = icmp sgt i32 %or77.i211.i.i, 0
  br i1 %cmp79.i213.i.i, label %if.then81.i218.i.i, label %if.else100.i234.i.i

if.then81.i218.i.i:                               ; preds = %if.end60.i214.i.i
  %mul82.i215.i.i = fmul float %sub32.i160.i.i, 5.000000e-01
  %mul83.i216.i.i = fmul float %sub32.i160.i.i, %mul82.i215.i.i
  %cmp84.i217.i.i = icmp eq i32 %add31.i159.i.i, 0
  br i1 %cmp84.i217.i.i, label %if.then86.i223.i.i, label %if.else91.i232.i.i

if.then86.i223.i.i:                               ; preds = %if.then81.i218.i.i
  %add87.i219.i.i = fadd float %mul83.i216.i.i, %add78.i212.i.i
  %mul88.i220.i.i = fmul float %mul65.i199.i.i, %add87.i219.i.i
  %sub89.i221.i.i = fsub float %mul83.i216.i.i, %mul88.i220.i.i
  %sub90.i222.i.i = fsub float %sub32.i160.i.i, %sub89.i221.i.i
  br label %redefine_logf.exit247.i.i

if.else91.i232.i.i:                               ; preds = %if.then81.i218.i.i
  %mul92.i224.i.i = fmul float %conv66.i200.i.i, 0x3FE62E3000000000
  %add93.i225.i.i = fadd float %mul83.i216.i.i, %add78.i212.i.i
  %mul94.i226.i.i = fmul float %mul65.i199.i.i, %add93.i225.i.i
  %mul95.i227.i.i = fmul float %conv66.i200.i.i, 0x3EE2FEFA20000000
  %add96.i228.i.i = fadd float %mul95.i227.i.i, %mul94.i226.i.i
  %sub97.i229.i.i = fsub float %mul83.i216.i.i, %add96.i228.i.i
  %sub98.i230.i.i = fsub float %sub97.i229.i.i, %sub32.i160.i.i
  %sub99.i231.i.i = fsub float %mul92.i224.i.i, %sub98.i230.i.i
  br label %redefine_logf.exit247.i.i

if.else100.i234.i.i:                              ; preds = %if.end60.i214.i.i
  %cmp101.i233.i.i = icmp eq i32 %add31.i159.i.i, 0
  br i1 %cmp101.i233.i.i, label %if.then103.i238.i.i, label %if.else107.i246.i.i

if.then103.i238.i.i:                              ; preds = %if.else100.i234.i.i
  %sub104.i235.i.i = fsub float %sub32.i160.i.i, %add78.i212.i.i
  %mul105.i236.i.i = fmul float %mul65.i199.i.i, %sub104.i235.i.i
  %sub106.i237.i.i = fsub float %sub32.i160.i.i, %mul105.i236.i.i
  br label %redefine_logf.exit247.i.i

if.else107.i246.i.i:                              ; preds = %if.else100.i234.i.i
  %mul108.i239.i.i = fmul float %conv66.i200.i.i, 0x3FE62E3000000000
  %sub109.i240.i.i = fsub float %sub32.i160.i.i, %add78.i212.i.i
  %mul110.i241.i.i = fmul float %mul65.i199.i.i, %sub109.i240.i.i
  %mul111.i242.i.i = fmul float %conv66.i200.i.i, 0x3EE2FEFA20000000
  %sub112.i243.i.i = fsub float %mul110.i241.i.i, %mul111.i242.i.i
  %sub113.i244.i.i = fsub float %sub112.i243.i.i, %sub32.i160.i.i
  %sub114.i245.i.i = fsub float %mul108.i239.i.i, %sub113.i244.i.i
  br label %redefine_logf.exit247.i.i

redefine_logf.exit247.i.i:                        ; preds = %if.else107.i246.i.i, %if.then103.i238.i.i, %if.else91.i232.i.i, %if.then86.i223.i.i, %if.else53.i188.i.i, %if.then51.i181.i.i, %if.else.i173.i.i, %if.then38.i167.i.i, %if.then19.i146.i.i, %if.then4.i137.i.i, %if.then.i108.i.i
  %retval.i85.i.0.i = phi float [ %mul8.i136.i.i, %if.then4.i137.i.i ], [ %add.i145.i.i, %if.then19.i146.i.i ], [ %add43.i172.i.i, %if.else.i173.i.i ], [ %sub52.i180.i.i, %if.then51.i181.i.i ], [ %sub59.i187.i.i, %if.else53.i188.i.i ], [ %sub90.i222.i.i, %if.then86.i223.i.i ], [ %sub99.i231.i.i, %if.else91.i232.i.i ], [ %sub106.i237.i.i, %if.then103.i238.i.i ], [ %sub114.i245.i.i, %if.else107.i246.i.i ], [ 0xC581306CE0000000, %if.then.i108.i.i ], [ 0.000000e+00, %if.then38.i167.i.i ]
  %sub8.i.i = fsub float 1.000000e+00, %inv.i262.i.0.i
  %86 = bitcast float %sub8.i.i to i32
  %cmp.i51.i.i = icmp slt i32 %86, 8388608
  br i1 %cmp.i51.i.i, label %if.then.i54.i.i, label %if.end17.i68.i.i

if.then.i54.i.i:                                  ; preds = %redefine_logf.exit247.i.i
  %and.i52.i.i = and i32 %86, 2147483647
  %cmp1.i53.i.i = icmp eq i32 %and.i52.i.i, 0
  br i1 %cmp1.i53.i.i, label %redefine_logf.exit.i.i, label %if.end.i65.i.i

if.end.i65.i.i:                                   ; preds = %if.then.i54.i.i
  %cmp3.i64.i.i = icmp slt i32 %86, 0
  br i1 %cmp3.i64.i.i, label %if.then4.i67.i.i, label %if.end9.i.i.i

if.then4.i67.i.i:                                 ; preds = %if.end.i65.i.i
  %sub.i66.i.i = fsub float %sub8.i.i, %sub8.i.i
  %mul6.i.i.i = fmul float %sub.i66.i.i, 0x43F1306CE0000000
  %mul8.i.i.i = fmul float %mul6.i.i.i, 0x43F1306CE0000000
  br label %redefine_logf.exit.i.i

if.end9.i.i.i:                                    ; preds = %if.end.i65.i.i
  %mul11.i.i.i = fmul float %sub8.i.i, 0x4180000000000000
  %87 = bitcast float %mul11.i.i.i to i32
  br label %if.end17.i68.i.i

if.end17.i68.i.i:                                 ; preds = %if.end9.i.i.i, %redefine_logf.exit247.i.i
  %ix.i.i.0.i = phi i32 [ %87, %if.end9.i.i.i ], [ %86, %redefine_logf.exit247.i.i ]
  %k.i.i.0.i = phi i32 [ -25, %if.end9.i.i.i ], [ 0, %redefine_logf.exit247.i.i ]
  %x.addr.i45.i.0.i = phi float [ %mul11.i.i.i, %if.end9.i.i.i ], [ %sub8.i.i, %redefine_logf.exit247.i.i ]
  %cmp18.i.i.i = icmp sgt i32 %ix.i.i.0.i, 2139095039
  br i1 %cmp18.i.i.i, label %if.then19.i.i.i, label %if.end20.i.i.i

if.then19.i.i.i:                                  ; preds = %if.end17.i68.i.i
  %add.i69.i.i = fadd float %x.addr.i45.i.0.i, %x.addr.i45.i.0.i
  br label %redefine_logf.exit.i.i

if.end20.i.i.i:                                   ; preds = %if.end17.i68.i.i
  %shr.i70.i.i = ashr i32 %ix.i.i.0.i, 23
  %sub21.i.i.i = add nsw i32 %shr.i70.i.i, -127
  %add22.i.i.i = add nsw i32 %sub21.i.i.i, %k.i.i.0.i
  %88 = and i32 %ix.i.i.0.i, 8388607
  %add24.i71.i.i = add nsw i32 %88, 4913952
  %and25.i.i.i = and i32 %add24.i71.i.i, 8388608
  %and23.i.i.i = or i32 %and25.i.i.i, %88
  %or.i.i.i = xor i32 %and23.i.i.i, 1065353216
  %89 = bitcast i32 %or.i.i.i to float
  %shr30.i.i.i15 = lshr exact i32 %and25.i.i.i, 23
  %add31.i.i.i = add nsw i32 %add22.i.i.i, %shr30.i.i.i15
  %sub32.i.i.i = fadd float %89, -1.000000e+00
  %add33.i.i.i = add nsw i32 %ix.i.i.0.i, 32768
  %and34.i.i.i = and i32 %add33.i.i.i, 8372224
  %cmp35.i.i.i = icmp ult i32 %and34.i.i.i, 49152
  br i1 %cmp35.i.i.i, label %if.then36.i.i.i, label %if.end60.i.i.i

if.then36.i.i.i:                                  ; preds = %if.end20.i.i.i
  %cmp37.i.i.i = fcmp oeq float %sub32.i.i.i, 0.000000e+00
  br i1 %cmp37.i.i.i, label %if.then38.i.i.i, label %if.end44.i.i.i

if.then38.i.i.i:                                  ; preds = %if.then36.i.i.i
  %cmp39.i.i.i = icmp eq i32 %add31.i.i.i, 0
  br i1 %cmp39.i.i.i, label %redefine_logf.exit.i.i, label %if.else.i72.i.i

if.else.i72.i.i:                                  ; preds = %if.then38.i.i.i
  %conv.i.i.i = sitofp i32 %add31.i.i.i to float
  %mul41.i.i.i = fmul float %conv.i.i.i, 0x3FE62E3000000000
  %mul42.i.i.i = fmul float %conv.i.i.i, 0x3EE2FEFA20000000
  %add43.i.i.i = fadd float %mul41.i.i.i, %mul42.i.i.i
  br label %redefine_logf.exit.i.i

if.end44.i.i.i:                                   ; preds = %if.then36.i.i.i
  %mul45.i.i.i = fmul float %sub32.i.i.i, %sub32.i.i.i
  %mul46.i.i.i = fmul float %sub32.i.i.i, 0x3FD5555560000000
  %sub47.i.i.i = fsub float 5.000000e-01, %mul46.i.i.i
  %mul48.i.i.i = fmul float %mul45.i.i.i, %sub47.i.i.i
  %cmp49.i.i.i = icmp eq i32 %add31.i.i.i, 0
  br i1 %cmp49.i.i.i, label %if.then51.i.i.i, label %if.else53.i.i.i

if.then51.i.i.i:                                  ; preds = %if.end44.i.i.i
  %sub52.i.i.i = fsub float %sub32.i.i.i, %mul48.i.i.i
  br label %redefine_logf.exit.i.i

if.else53.i.i.i:                                  ; preds = %if.end44.i.i.i
  %conv54.i.i.i = sitofp i32 %add31.i.i.i to float
  %mul55.i.i.i = fmul float %conv54.i.i.i, 0x3FE62E3000000000
  %mul56.i.i.i = fmul float %conv54.i.i.i, 0x3EE2FEFA20000000
  %sub57.i.i.i = fsub float %mul48.i.i.i, %mul56.i.i.i
  %sub58.i.i.i = fsub float %sub57.i.i.i, %sub32.i.i.i
  %sub59.i.i.i = fsub float %mul55.i.i.i, %sub58.i.i.i
  br label %redefine_logf.exit.i.i

if.end60.i.i.i:                                   ; preds = %if.end20.i.i.i
  %add61.i.i.i = fadd float %sub32.i.i.i, 2.000000e+00
  %mul63.i.i.i = fmul float %add61.i.i.i, %add61.i.i.i
  %90 = bitcast float %mul63.i.i.i to i32
  %mul.i118.i.i.i = fmul float %mul63.i.i.i, 5.000000e-01
  %shr.i119.i.i.i = ashr i32 %90, 1
  %sub.i120.i.i.i = sub nsw i32 1597463007, %shr.i119.i.i.i
  %91 = bitcast i32 %sub.i120.i.i.i to float
  %mul1.i121.i.i.i = fmul float %mul.i118.i.i.i, %91
  %mul2.i122.i.i.i = fmul float %91, %mul1.i121.i.i.i
  %sub3.i123.i.i.i = fsub float 1.500000e+00, %mul2.i122.i.i.i
  %mul4.i124.i.i.i = fmul float %91, %sub3.i123.i.i.i
  %mul65.i.i.i = fmul float %sub32.i.i.i, %mul4.i124.i.i.i
  %conv66.i.i.i = sitofp i32 %add31.i.i.i to float
  %mul67.i.i.i = fmul float %mul65.i.i.i, %mul65.i.i.i
  %sub68.i.i.i = add nsw i32 %88, -3187664
  %mul69.i.i.i = fmul float %mul67.i.i.i, %mul67.i.i.i
  %sub70.i.i.i = sub nsw i32 3523208, %88
  %mul71.i.i.i = fmul float %mul69.i.i.i, 0x3FCF13C4C0000000
  %add72.i.i.i = fadd float %mul71.i.i.i, 0x3FD999C260000000
  %mul73.i.i.i = fmul float %mul69.i.i.i, %add72.i.i.i
  %mul74.i.i.i = fmul float %mul69.i.i.i, 0x3FD23D3DC0000000
  %add75.i.i.i = fadd float %mul74.i.i.i, 0x3FE5555540000000
  %mul76.i.i.i = fmul float %mul67.i.i.i, %add75.i.i.i
  %or77.i.i.i = or i32 %sub68.i.i.i, %sub70.i.i.i
  %add78.i.i.i = fadd float %mul76.i.i.i, %mul73.i.i.i
  %cmp79.i.i.i = icmp sgt i32 %or77.i.i.i, 0
  br i1 %cmp79.i.i.i, label %if.then81.i.i.i, label %if.else100.i.i.i

if.then81.i.i.i:                                  ; preds = %if.end60.i.i.i
  %mul82.i.i.i = fmul float %sub32.i.i.i, 5.000000e-01
  %mul83.i.i.i = fmul float %sub32.i.i.i, %mul82.i.i.i
  %cmp84.i.i.i = icmp eq i32 %add31.i.i.i, 0
  br i1 %cmp84.i.i.i, label %if.then86.i.i.i, label %if.else91.i.i.i

if.then86.i.i.i:                                  ; preds = %if.then81.i.i.i
  %add87.i.i.i = fadd float %mul83.i.i.i, %add78.i.i.i
  %mul88.i.i.i = fmul float %mul65.i.i.i, %add87.i.i.i
  %sub89.i.i.i = fsub float %mul83.i.i.i, %mul88.i.i.i
  %sub90.i.i.i = fsub float %sub32.i.i.i, %sub89.i.i.i
  br label %redefine_logf.exit.i.i

if.else91.i.i.i:                                  ; preds = %if.then81.i.i.i
  %mul92.i.i.i = fmul float %conv66.i.i.i, 0x3FE62E3000000000
  %add93.i.i.i = fadd float %mul83.i.i.i, %add78.i.i.i
  %mul94.i.i.i = fmul float %mul65.i.i.i, %add93.i.i.i
  %mul95.i.i.i = fmul float %conv66.i.i.i, 0x3EE2FEFA20000000
  %add96.i.i.i = fadd float %mul95.i.i.i, %mul94.i.i.i
  %sub97.i.i.i = fsub float %mul83.i.i.i, %add96.i.i.i
  %sub98.i.i.i = fsub float %sub97.i.i.i, %sub32.i.i.i
  %sub99.i.i.i = fsub float %mul92.i.i.i, %sub98.i.i.i
  br label %redefine_logf.exit.i.i

if.else100.i.i.i:                                 ; preds = %if.end60.i.i.i
  %cmp101.i.i.i = icmp eq i32 %add31.i.i.i, 0
  br i1 %cmp101.i.i.i, label %if.then103.i.i.i, label %if.else107.i.i.i

if.then103.i.i.i:                                 ; preds = %if.else100.i.i.i
  %sub104.i.i.i = fsub float %sub32.i.i.i, %add78.i.i.i
  %mul105.i.i.i = fmul float %mul65.i.i.i, %sub104.i.i.i
  %sub106.i.i.i = fsub float %sub32.i.i.i, %mul105.i.i.i
  br label %redefine_logf.exit.i.i

if.else107.i.i.i:                                 ; preds = %if.else100.i.i.i
  %mul108.i.i.i = fmul float %conv66.i.i.i, 0x3FE62E3000000000
  %sub109.i.i.i = fsub float %sub32.i.i.i, %add78.i.i.i
  %mul110.i.i.i = fmul float %mul65.i.i.i, %sub109.i.i.i
  %mul111.i.i.i = fmul float %conv66.i.i.i, 0x3EE2FEFA20000000
  %sub112.i.i.i = fsub float %mul110.i.i.i, %mul111.i.i.i
  %sub113.i.i.i = fsub float %sub112.i.i.i, %sub32.i.i.i
  %sub114.i.i.i = fsub float %mul108.i.i.i, %sub113.i.i.i
  br label %redefine_logf.exit.i.i

redefine_logf.exit.i.i:                           ; preds = %if.else107.i.i.i, %if.then103.i.i.i, %if.else91.i.i.i, %if.then86.i.i.i, %if.else53.i.i.i, %if.then51.i.i.i, %if.else.i72.i.i, %if.then38.i.i.i, %if.then19.i.i.i, %if.then4.i67.i.i, %if.then.i54.i.i
  %retval.i44.i.0.i = phi float [ %mul8.i.i.i, %if.then4.i67.i.i ], [ %add.i69.i.i, %if.then19.i.i.i ], [ %add43.i.i.i, %if.else.i72.i.i ], [ %sub52.i.i.i, %if.then51.i.i.i ], [ %sub59.i.i.i, %if.else53.i.i.i ], [ %sub90.i.i.i, %if.then86.i.i.i ], [ %sub99.i.i.i, %if.else91.i.i.i ], [ %sub106.i.i.i, %if.then103.i.i.i ], [ %sub114.i.i.i, %if.else107.i.i.i ], [ 0xC581306CE0000000, %if.then.i54.i.i ], [ 0.000000e+00, %if.then38.i.i.i ]
  %mul.i27.i.i = fmul float %retval.i44.i.0.i, %retval.i44.i.0.i
  %92 = bitcast float %mul.i27.i.i to i32
  %mul.i.i28.i.i = fmul float %mul.i27.i.i, 5.000000e-01
  %shr.i.i29.i.i = ashr i32 %92, 1
  %sub.i.i30.i.i = sub nsw i32 1597463007, %shr.i.i29.i.i
  %93 = bitcast i32 %sub.i.i30.i.i to float
  %mul1.i.i31.i.i = fmul float %mul.i.i28.i.i, %93
  %mul2.i.i32.i.i = fmul float %93, %mul1.i.i31.i.i
  %sub3.i.i33.i.i = fsub float 1.500000e+00, %mul2.i.i32.i.i
  %mul4.i.i34.i.i = fmul float %93, %sub3.i.i33.i.i
  %cmp.i35.i.i = fcmp olt float %retval.i44.i.0.i, 0.000000e+00
  br i1 %cmp.i35.i.i, label %if.then.i37.i.i, label %redefine_div.exit40.i.i

if.then.i37.i.i:                                  ; preds = %redefine_logf.exit.i.i
  %sub.i36.i.i = fsub float 0.000000e+00, %mul4.i.i34.i.i
  br label %redefine_div.exit40.i.i

redefine_div.exit40.i.i:                          ; preds = %if.then.i37.i.i, %redefine_logf.exit.i.i
  %inv.i26.i.0.i = phi float [ %sub.i36.i.i, %if.then.i37.i.i ], [ %mul4.i.i34.i.i, %redefine_logf.exit.i.i ]
  %mul1.i38.i.i = fmul float %retval.i85.i.0.i, %inv.i26.i.0.i
  %94 = bitcast float %mul1.i38.i.i to i32
  %shr.i.i.i16 = lshr i32 %94, 23
  %and.i.i.i = and i32 %shr.i.i.i16, 255
  %sub.i18.i.i = add nsw i32 %and.i.i.i, -127
  %cmp.i19.i.i = icmp slt i32 %sub.i18.i.i, 23
  br i1 %cmp.i19.i.i, label %if.then.i20.i.i, label %if.else29.i.i.i

if.then.i20.i.i:                                  ; preds = %redefine_div.exit40.i.i
  %cmp1.i.i.i = icmp slt i32 %sub.i18.i.i, 0
  br i1 %cmp1.i.i.i, label %if.then2.i.i.i, label %if.else12.i.i.i

if.then2.i.i.i:                                   ; preds = %if.then.i20.i.i
  %add.i.i.i = fadd float %mul1.i38.i.i, 0x46293E5940000000
  %cmp3.i.i.i = fcmp ogt float %add.i.i.i, 0.000000e+00
  br i1 %cmp3.i.i.i, label %if.then4.i.i.i, label %if.end28.i.i.i

if.then4.i.i.i:                                   ; preds = %if.then2.i.i.i
  %cmp5.i.i.i = icmp sgt i32 %94, -1
  br i1 %cmp5.i.i.i, label %if.end28.i.i.i, label %if.else.i.i.i

if.else.i.i.i:                                    ; preds = %if.then4.i.i.i
  %and7.i.i.i = and i32 %94, 2147483647
  %cmp8.i.i.i = icmp eq i32 %and7.i.i.i, 0
  %. = select i1 %cmp8.i.i.i, i32 %94, i32 -1082130432
  br label %if.end28.i.i.i

if.else12.i.i.i:                                  ; preds = %if.then.i20.i.i
  %shr13.i.i.i = lshr i32 8388607, %sub.i18.i.i
  %and14.i.i.i = and i32 %shr13.i.i.i, %94
  %cmp15.i.i.i = icmp eq i32 %and14.i.i.i, 0
  br i1 %cmp15.i.i.i, label %redefine_floorf.exit.i.i, label %if.end17.i.i.i

if.end17.i.i.i:                                   ; preds = %if.else12.i.i.i
  %add18.i.i.i = fadd float %mul1.i38.i.i, 0x46293E5940000000
  %cmp19.i.i.i = fcmp ogt float %add18.i.i.i, 0.000000e+00
  br i1 %cmp19.i.i.i, label %if.then20.i.i.i, label %if.end28.i.i.i

if.then20.i.i.i:                                  ; preds = %if.end17.i.i.i
  %cmp21.i.i.i = icmp slt i32 %94, 0
  br i1 %cmp21.i.i.i, label %if.then22.i.i.i, label %if.end25.i.i.i

if.then22.i.i.i:                                  ; preds = %if.then20.i.i.i
  %shr23.i.i.i = lshr i32 8388608, %sub.i18.i.i
  %add24.i.i.i = add nsw i32 %shr23.i.i.i, %94
  br label %if.end25.i.i.i

if.end25.i.i.i:                                   ; preds = %if.then22.i.i.i, %if.then20.i.i.i
  %i0.i.i.0.i = phi i32 [ %add24.i.i.i, %if.then22.i.i.i ], [ %94, %if.then20.i.i.i ]
  %neg.i.i.i = xor i32 %shr13.i.i.i, -1
  %and26.i.i.i = and i32 %i0.i.i.0.i, %neg.i.i.i
  br label %if.end28.i.i.i

if.end28.i.i.i:                                   ; preds = %if.end25.i.i.i, %if.end17.i.i.i, %if.else.i.i.i, %if.then4.i.i.i, %if.then2.i.i.i
  %i0.i.i.1.i = phi i32 [ %94, %if.then2.i.i.i ], [ %and26.i.i.i, %if.end25.i.i.i ], [ %94, %if.end17.i.i.i ], [ 0, %if.then4.i.i.i ], [ %., %if.else.i.i.i ]
  %95 = bitcast i32 %i0.i.i.1.i to float
  br label %redefine_floorf.exit.i.i

if.else29.i.i.i:                                  ; preds = %redefine_div.exit40.i.i
  %cmp30.i.i.i = icmp eq i32 %sub.i18.i.i, 128
  br i1 %cmp30.i.i.i, label %if.then31.i.i.i, label %redefine_floorf.exit.i.i

if.then31.i.i.i:                                  ; preds = %if.else29.i.i.i
  %add32.i.i.i = fadd float %mul1.i38.i.i, %mul1.i38.i.i
  br label %redefine_floorf.exit.i.i

redefine_floorf.exit.i.i:                         ; preds = %if.then31.i.i.i, %if.else29.i.i.i, %if.end28.i.i.i, %if.else12.i.i.i
  %retval.i.i.0.i = phi float [ %95, %if.end28.i.i.i ], [ %add32.i.i.i, %if.then31.i.i.i ], [ %mul1.i38.i.i, %if.else12.i.i.i ], [ %mul1.i38.i.i, %if.else29.i.i.i ]
  %cmp13.i.i = icmp slt i32 %8, 5
  br i1 %cmp13.i.i, label %if.then15.i.i, label %uts_numChildren.exit

if.then15.i.i:                                    ; preds = %redefine_floorf.exit.i.i
  %conv12.i.i = fptosi float %retval.i.i.0.i to i32
  br label %uts_numChildren.exit

uts_numChildren.exit:                             ; preds = %if.then15.i.i, %redefine_floorf.exit.i.i
  %retval.i.0.i = phi i32 [ %conv12.i.i, %if.then15.i.i ], [ 0, %redefine_floorf.exit.i.i ]
  %96 = icmp sgt i32 %retval.i.0.i, 0
  %retval.i.0.i. = select i1 %96, i32 %retval.i.0.i, i32 0
  %falloc_reg = call i32 @llvm.falloc(i32 0)
  call void @llvm.fbind(i32 12, i32 %falloc_reg)
  %falloc_reg1 = call i32 @llvm.falloc(i32 0)
  call void @llvm.fbind(i32 13, i32 %falloc_reg1)
  %falloc_reg2 = call i32 @llvm.falloc(i32 0)
  call void @llvm.fbind(i32 15, i32 %falloc_reg2)
  call void @llvm.writecm(i32 %falloc_reg2, i32 60, i32 1)
  %97 = add i32 %retval.i.0.i., -1
  %falloc_reg3 = call i32 @llvm.falloc(i32 %97)
  %98 = shl nuw i32 %97, 6
  %99 = add nuw i32 %falloc_reg3, %98
  br label %uts_function_for.body1511111_create_body

uts_function_for.body1511111_create_body:         ; preds = %uts_function_for.body1511111_create_body, %uts_numChildren.exit
  %100 = phi i32 [ %falloc_reg3, %uts_numChildren.exit ], [ %101, %uts_function_for.body1511111_create_body ]
  call void @llvm.fbind(i32 16, i32 %100)
  %101 = add nuw i32 %100, 64
  %cmpinst = icmp ugt i32 %101, %99
  br i1 %cmpinst, label %uts_function_for.body1511111_create_end, label %uts_function_for.body1511111_create_body

uts_function_for.body1511111_create_end:          ; preds = %uts_function_for.body1511111_create_body
  call void @llvm.writecm(i32 %falloc_reg, i32 0, i32 %1)
  %102 = zext i1 %96 to i32
  %invertedPred = xor i32 %102, 1
  call void @llvm.writecmp(i32 %falloc_reg, i32 %invertedPred)
  call void @llvm.writecm(i32 %falloc_reg1, i32 8, i32 %retval.i.0.i)
  call void @llvm.writecm(i32 %falloc_reg1, i32 4, i32 %falloc_reg3)
  call void @llvm.writecm(i32 %falloc_reg1, i32 0, i32 %retval.i.0.i.)
  call void @llvm.writecmp(i32 %falloc_reg1, i32 %102)
  %103 = bitcast %struct.node_t* %3 to i8*
  %104 = bitcast %struct.node_t* %73 to i8*
  br label %uts_function_for.body1511111_create_body5

uts_function_for.body1511111_create_body5:        ; preds = %redefine_memcpy.exit7, %uts_function_for.body1511111_create_end
  %105 = phi i32 [ %falloc_reg3, %uts_function_for.body1511111_create_end ], [ %108, %redefine_memcpy.exit7 ]
  %memframe_reg = call i8* @llvm.getmemframe(i32 %105)
  %ptr_val = getelementptr i8* %memframe_reg, i32 32
  br label %for.body.i

for.body.i:                                       ; preds = %for.body.i, %uts_function_for.body1511111_create_body5
  %indvars.iv.i = phi i32 [ %indvars.iv.next.i, %for.body.i ], [ 0, %uts_function_for.body1511111_create_body5 ]
  %arrayidx.i = getelementptr inbounds i8* %103, i32 %indvars.iv.i
  %106 = load i8* %arrayidx.i, align 1
  %arrayidx2.i = getelementptr inbounds i8* %ptr_val, i32 %indvars.iv.i
  store i8 %106, i8* %arrayidx2.i, align 1
  %indvars.iv.next.i = add i32 %indvars.iv.i, 1
  %exitcond.i = icmp eq i32 %indvars.iv.next.i, 32
  br i1 %exitcond.i, label %redefine_memcpy.exit, label %for.body.i

redefine_memcpy.exit:                             ; preds = %for.body.i
  br label %for.body.i6

for.body.i6:                                      ; preds = %for.body.i6, %redefine_memcpy.exit
  %indvars.iv.i1 = phi i32 [ %indvars.iv.next.i4, %for.body.i6 ], [ 0, %redefine_memcpy.exit ]
  %arrayidx.i2 = getelementptr inbounds i8* %104, i32 %indvars.iv.i1
  %107 = load i8* %arrayidx.i2, align 1
  %arrayidx2.i3 = getelementptr inbounds i8* %memframe_reg, i32 %indvars.iv.i1
  store i8 %107, i8* %arrayidx2.i3, align 1
  %indvars.iv.next.i4 = add i32 %indvars.iv.i1, 1
  %exitcond.i5 = icmp eq i32 %indvars.iv.next.i4, 32
  br i1 %exitcond.i5, label %redefine_memcpy.exit7, label %for.body.i6

redefine_memcpy.exit7:                            ; preds = %for.body.i6
  call void @llvm.writecm(i32 %105, i32 16, i32 1)
  call void @llvm.writecm(i32 %105, i32 8, i32 %retval.i.0.i.)
  call void @llvm.writecm(i32 %105, i32 4, i32 %falloc_reg2)
  call void @llvm.writecm(i32 %105, i32 0, i32 %falloc_reg3)
  %108 = add nuw i32 %105, 64
  %cmpinst7 = icmp ugt i32 %108, %99
  br i1 %cmpinst7, label %uts_function_for.body1511111_create_end6, label %uts_function_for.body1511111_create_body5

uts_function_for.body1511111_create_end6:         ; preds = %redefine_memcpy.exit7
  call void @llvm.writecm(i32 %falloc_reg2, i32 4, i32 %retval.i.0.i.)
  call void @llvm.writecm(i32 %falloc_reg2, i32 0, i32 %1)
  call void @llvm.writecm(i32 %1, i32 12, i32 %falloc_reg)
  call void @llvm.writecm(i32 %1, i32 8, i32 %falloc_reg1)
  call void @llvm.writecm(i32 %1, i32 4, i32 %falloc_reg3)
  call void @llvm.writecm(i32 %1, i32 0, i32 %retval.i.0.i.)
  call void @llvm.fdelete(i32 %0)
  ret void
}

; Function Attrs: nounwind
define void @uts_function_for.body141111(i32 inreg, i32 inreg, i32 inreg, i32* nocapture) #1 {
entry:
  %4 = icmp sgt i32 %2, 0
  br i1 %4, label %for.body, label %for.end

for.body:                                         ; preds = %for.body, %entry
  %.09 = phi i32 [ %7, %for.body ], [ 0, %entry ]
  %.078 = phi i32 [ %8, %for.body ], [ 0, %entry ]
  %5 = getelementptr inbounds i32* %3, i32 %.078
  %6 = load i32* %5, align 4
  %7 = add nsw i32 %6, %.09
  %8 = add nsw i32 %.078, 1
  %exitcond = icmp eq i32 %8, %2
  br i1 %exitcond, label %for.end, label %for.body

for.end:                                          ; preds = %for.body, %entry
  %.0.lcssa = phi i32 [ 0, %entry ], [ %7, %for.body ]
  tail call void @llvm.writecm(i32 %1, i32 24, i32 %.0.lcssa)
  tail call void @llvm.fdelete(i32 %0)
  ret void
}

; Function Attrs: alwaysinline nounwind ssp
define void @uts_function_for.body1511111(i32 inreg, i32 inreg, i32 inreg, i32 inreg, i32 inreg, i32 inreg, %struct.node_t*, %struct.node_t* nocapture) #0 {
for.body54.i.i.lr.ph:
  %ctx.i = alloca %struct.sha1_ctx_s, align 4
  %bytes.i = alloca [4 x i8], align 1
  %8 = alloca %struct.node_t, align 8
  %9 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 0
  store i32 %5, i32* %9, align 4
  %10 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 1
  %11 = load i32* %10, align 4
  %12 = add nsw i32 %11, 1
  %13 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 1
  store i32 %12, i32* %13, align 4
  %14 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 2
  store i32 -1, i32* %14, align 4
  %15 = getelementptr inbounds [4 x i8]* %bytes.i, i32 0, i32 0
  %shr.i1 = lshr i32 %4, 24
  %conv.i = trunc i32 %shr.i1 to i8
  store i8 %conv.i, i8* %15, align 1
  %shr1.i2 = lshr i32 %4, 16
  %conv3.i = trunc i32 %shr1.i2 to i8
  %arrayidx4.i = getelementptr inbounds [4 x i8]* %bytes.i, i32 0, i32 1
  store i8 %conv3.i, i8* %arrayidx4.i, align 1
  %shr5.i3 = lshr i32 %4, 8
  %conv7.i = trunc i32 %shr5.i3 to i8
  %arrayidx8.i = getelementptr inbounds [4 x i8]* %bytes.i, i32 0, i32 2
  store i8 %conv7.i, i8* %arrayidx8.i, align 1
  %conv10.i = trunc i32 %4 to i8
  %arrayidx11.i = getelementptr inbounds [4 x i8]* %bytes.i, i32 0, i32 3
  store i8 %conv10.i, i8* %arrayidx11.i, align 1
  %arrayidx.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 0, i32 1
  store i32 0, i32* %arrayidx.i.i, align 4
  %arrayidx2.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 0, i32 0
  %arrayidx3.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 1, i32 0
  store i32 1732584193, i32* %arrayidx3.i.i, align 4
  %arrayidx5.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 1, i32 1
  store i32 -271733879, i32* %arrayidx5.i.i, align 4
  %arrayidx7.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 1, i32 2
  store i32 -1732584194, i32* %arrayidx7.i.i, align 4
  %arrayidx9.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 1, i32 3
  store i32 271733878, i32* %arrayidx9.i.i, align 4
  %arrayidx11.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 1, i32 4
  store i32 -1009589776, i32* %arrayidx11.i.i, align 4
  %arraydecay60.i2562.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 0
  %16 = bitcast i32* %arraydecay60.i2562.i to i8*
  %arrayidx56.i2558.i = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 0
  %17 = load i8* %arrayidx56.i2558.i, align 1
  store i8 %17, i8* %16, align 4
  %arrayidx56.i2558.i.1 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 1
  %18 = load i8* %arrayidx56.i2558.i.1, align 1
  %arrayidx61.i2563.i.1 = getelementptr inbounds i8* %16, i32 1
  store i8 %18, i8* %arrayidx61.i2563.i.1, align 1
  %arrayidx56.i2558.i.2 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 2
  %19 = load i8* %arrayidx56.i2558.i.2, align 1
  %arrayidx61.i2563.i.2 = getelementptr inbounds i8* %16, i32 2
  store i8 %19, i8* %arrayidx61.i2563.i.2, align 2
  %arrayidx56.i2558.i.3 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 3
  %20 = load i8* %arrayidx56.i2558.i.3, align 1
  %arrayidx61.i2563.i.3 = getelementptr inbounds i8* %16, i32 3
  store i8 %20, i8* %arrayidx61.i2563.i.3, align 1
  %arrayidx56.i2558.i.4 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 4
  %21 = load i8* %arrayidx56.i2558.i.4, align 1
  %arrayidx61.i2563.i.453 = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 1
  %arrayidx61.i2563.i.4 = bitcast i32* %arrayidx61.i2563.i.453 to i8*
  store i8 %21, i8* %arrayidx61.i2563.i.4, align 4
  %arrayidx56.i2558.i.5 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 5
  %22 = load i8* %arrayidx56.i2558.i.5, align 1
  %arrayidx61.i2563.i.5 = getelementptr inbounds i8* %16, i32 5
  store i8 %22, i8* %arrayidx61.i2563.i.5, align 1
  %arrayidx56.i2558.i.6 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 6
  %23 = load i8* %arrayidx56.i2558.i.6, align 1
  %arrayidx61.i2563.i.6 = getelementptr inbounds i8* %16, i32 6
  store i8 %23, i8* %arrayidx61.i2563.i.6, align 2
  %arrayidx56.i2558.i.7 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 7
  %24 = load i8* %arrayidx56.i2558.i.7, align 1
  %arrayidx61.i2563.i.7 = getelementptr inbounds i8* %16, i32 7
  store i8 %24, i8* %arrayidx61.i2563.i.7, align 1
  %arrayidx56.i2558.i.8 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 8
  %25 = load i8* %arrayidx56.i2558.i.8, align 1
  %arrayidx61.i2563.i.854 = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 2
  %arrayidx61.i2563.i.8 = bitcast i32* %arrayidx61.i2563.i.854 to i8*
  store i8 %25, i8* %arrayidx61.i2563.i.8, align 4
  %arrayidx56.i2558.i.9 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 9
  %26 = load i8* %arrayidx56.i2558.i.9, align 1
  %arrayidx61.i2563.i.9 = getelementptr inbounds i8* %16, i32 9
  store i8 %26, i8* %arrayidx61.i2563.i.9, align 1
  %arrayidx56.i2558.i.10 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 10
  %27 = load i8* %arrayidx56.i2558.i.10, align 1
  %arrayidx61.i2563.i.10 = getelementptr inbounds i8* %16, i32 10
  store i8 %27, i8* %arrayidx61.i2563.i.10, align 2
  %arrayidx56.i2558.i.11 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 11
  %28 = load i8* %arrayidx56.i2558.i.11, align 1
  %arrayidx61.i2563.i.11 = getelementptr inbounds i8* %16, i32 11
  store i8 %28, i8* %arrayidx61.i2563.i.11, align 1
  %arrayidx56.i2558.i.12 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 12
  %29 = load i8* %arrayidx56.i2558.i.12, align 1
  %arrayidx61.i2563.i.1255 = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 3
  %arrayidx61.i2563.i.12 = bitcast i32* %arrayidx61.i2563.i.1255 to i8*
  store i8 %29, i8* %arrayidx61.i2563.i.12, align 4
  %arrayidx56.i2558.i.13 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 13
  %30 = load i8* %arrayidx56.i2558.i.13, align 1
  %arrayidx61.i2563.i.13 = getelementptr inbounds i8* %16, i32 13
  store i8 %30, i8* %arrayidx61.i2563.i.13, align 1
  %arrayidx56.i2558.i.14 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 14
  %31 = load i8* %arrayidx56.i2558.i.14, align 1
  %arrayidx61.i2563.i.14 = getelementptr inbounds i8* %16, i32 14
  store i8 %31, i8* %arrayidx61.i2563.i.14, align 2
  %arrayidx56.i2558.i.15 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 15
  %32 = load i8* %arrayidx56.i2558.i.15, align 1
  %arrayidx61.i2563.i.15 = getelementptr inbounds i8* %16, i32 15
  store i8 %32, i8* %arrayidx61.i2563.i.15, align 1
  %arrayidx56.i2558.i.16 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 16
  %33 = load i8* %arrayidx56.i2558.i.16, align 1
  %arrayidx61.i2563.i.1656 = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 4
  %arrayidx61.i2563.i.16 = bitcast i32* %arrayidx61.i2563.i.1656 to i8*
  store i8 %33, i8* %arrayidx61.i2563.i.16, align 4
  %arrayidx56.i2558.i.17 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 17
  %34 = load i8* %arrayidx56.i2558.i.17, align 1
  %arrayidx61.i2563.i.17 = getelementptr inbounds i8* %16, i32 17
  store i8 %34, i8* %arrayidx61.i2563.i.17, align 1
  %arrayidx56.i2558.i.18 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 18
  %35 = load i8* %arrayidx56.i2558.i.18, align 1
  %arrayidx61.i2563.i.18 = getelementptr inbounds i8* %16, i32 18
  store i8 %35, i8* %arrayidx61.i2563.i.18, align 2
  %arrayidx56.i2558.i.19 = getelementptr inbounds %struct.node_t* %7, i32 0, i32 3, i32 0, i32 19
  %36 = load i8* %arrayidx56.i2558.i.19, align 1
  %arrayidx61.i2563.i.19 = getelementptr inbounds i8* %16, i32 19
  store i8 %36, i8* %arrayidx61.i2563.i.19, align 1
  store i32 24, i32* %arrayidx2.i.i, align 4
  br label %for.body54.i.i

for.body54.i.i:                                   ; preds = %for.body54.i.i, %for.body54.i.i.lr.ph
  %i49.i.0.i327 = phi i32 [ 0, %for.body54.i.i.lr.ph ], [ %inc63.i.i, %for.body54.i.i ]
  %arrayidx56.i1260.i = getelementptr inbounds [4 x i8]* %bytes.i, i32 0, i32 %i49.i.0.i327
  %37 = load i8* %arrayidx56.i1260.i, align 1
  %add57.i.i = add i32 %i49.i.0.i327, 20
  %arrayidx61.i.i = getelementptr inbounds i8* %16, i32 %add57.i.i
  store i8 %37, i8* %arrayidx61.i.i, align 1
  %inc63.i.i = add nsw i32 %i49.i.0.i327, 1
  %cmp52.i.i = icmp ult i32 %inc63.i.i, 4
  br i1 %cmp52.i.i, label %for.body54.i.i, label %sha1_hash.exit.i

sha1_hash.exit.i:                                 ; preds = %for.body54.i.i
  %38 = load i32* %arrayidx2.i.i, align 4
  %and.i.i = and i32 %38, 63
  %add.i.i = add i32 %and.i.i, 3
  %shr.i.i = lshr i32 %add.i.i, 2
  %tobool.i.i324 = icmp eq i32 %shr.i.i, 0
  br i1 %tobool.i.i324, label %while.end.i.i, label %while.body.i.i

while.body.i.i:                                   ; preds = %while.body.i.i, %sha1_hash.exit.i
  %dec.i.i325.in = phi i32 [ %dec.i.i325, %while.body.i.i ], [ %shr.i.i, %sha1_hash.exit.i ]
  %dec.i.i325 = add nsw i32 %dec.i.i325.in, -1
  %arrayidx1.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %dec.i.i325
  %39 = load i32* %arrayidx1.i.i, align 4
  %shr.i2 = ashr i32 %39, 24
  %shl.i3 = shl i32 %39, 8
  %or.i4 = or i32 %shr.i2, %shl.i3
  %and.i5 = and i32 %or.i4, 16711935
  %shr1.i6 = ashr i32 %39, 8
  %shl2.i7 = shl i32 %39, 24
  %shr1.i6.masked = and i32 %shr1.i6, -16711936
  %and4.i9 = or i32 %shr1.i6.masked, %shl2.i7
  %or5.i10 = or i32 %and4.i9, %and.i5
  store i32 %or5.i10, i32* %arrayidx1.i.i, align 4
  %tobool.i.i = icmp eq i32 %dec.i.i325, 0
  br i1 %tobool.i.i, label %while.end.i.i, label %while.body.i.i

while.end.i.i:                                    ; preds = %while.body.i.i, %sha1_hash.exit.i
  %neg.i.i = shl i32 %38, 3
  %and25.i.i = and i32 %neg.i.i, 24
  %mul.i.i = xor i32 %and25.i.i, 24
  %shl26.i.i = shl i32 -128, %mul.i.i
  %shr27.i.i = lshr i32 %and.i.i, 2
  %wbuf29.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2
  %arrayidx30.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %shr27.i.i
  %40 = load i32* %arrayidx30.i.i, align 4
  %and31.i.i = and i32 %40, %shl26.i.i
  %shl35.i.i = shl i32 128, %mul.i.i
  %or40.i.i = or i32 %and31.i.i, %shl35.i.i
  store i32 %or40.i.i, i32* %arrayidx30.i.i, align 4
  %cmp.i.i = icmp ugt i32 %and.i.i, 55
  br i1 %cmp.i.i, label %if.then.i.i, label %while.cond48.i.i.preheader

if.then.i.i:                                      ; preds = %while.end.i.i
  %cmp41.i.i = icmp ult i32 %and.i.i, 60
  br i1 %cmp41.i.i, label %if.then42.i.i, label %if.end.i.i

if.then42.i.i:                                    ; preds = %if.then.i.i
  %arrayidx44.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 15
  store i32 0, i32* %arrayidx44.i.i, align 4
  br label %if.end.i.i

if.end.i.i:                                       ; preds = %if.then42.i.i, %if.then.i.i
  %41 = load i32* %arrayidx3.i.i, align 4
  %42 = load i32* %arrayidx5.i.i, align 4
  %43 = load i32* %arrayidx7.i.i, align 4
  %44 = load i32* %arrayidx9.i.i, align 4
  %45 = load i32* %arrayidx11.i.i, align 4
  %shr.i.i.i307 = lshr i32 %41, 27
  %shl.i.i.i308 = shl i32 %41, 5
  %or.i.i.i309 = or i32 %shr.i.i.i307, %shl.i.i.i308
  %and.i.i.i310 = and i32 %43, %42
  %neg.i.i.i311 = xor i32 %42, -1
  %and9.i.i.i312 = and i32 %44, %neg.i.i.i311
  %xor.i.i.i313 = xor i32 %and9.i.i.i312, %and.i.i.i310
  %add.i.i.i314 = add i32 %or.i.i.i309, 1518500249
  %add10.i.i.i315 = add i32 %add.i.i.i314, %xor.i.i.i313
  br label %for.body.i.i.i

for.body.i.i.i:                                   ; preds = %for.body.i.i.i, %if.end.i.i
  %add10.i.i.i322 = phi i32 [ %add10.i.i.i315, %if.end.i.i ], [ %add10.i.i.i, %for.body.i.i.i ]
  %ii.i.i.0.i321 = phi i32 [ 0, %if.end.i.i ], [ %add85.i.i.i, %for.body.i.i.i ]
  %v4.i.i.0.i320 = phi i32 [ %45, %if.end.i.i ], [ %or50.i.i.i, %for.body.i.i.i ]
  %v3.i.i.0.i319 = phi i32 [ %44, %if.end.i.i ], [ %or67.i.i.i, %for.body.i.i.i ]
  %v2.i.i.0.i318 = phi i32 [ %43, %if.end.i.i ], [ %or84.i.i.i, %for.body.i.i.i ]
  %v1.i.i.0.i317 = phi i32 [ %42, %if.end.i.i ], [ %add64.i.i.i, %for.body.i.i.i ]
  %v0.i.i.0.i316 = phi i32 [ %41, %if.end.i.i ], [ %add81.i.i.i, %for.body.i.i.i ]
  %arrayidx11.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %ii.i.i.0.i321
  %46 = load i32* %arrayidx11.i.i.i, align 4
  %add12.i.i.i = add i32 %v4.i.i.0.i320, %add10.i.i.i322
  %add13.i.i.i = add i32 %add12.i.i.i, %46
  %shr14.i.i.i = lshr i32 %v1.i.i.0.i317, 2
  %shl15.i.i.i = shl i32 %v1.i.i.0.i317, 30
  %or16.i.i.i = or i32 %shr14.i.i.i, %shl15.i.i.i
  %shr17.i.i.i = lshr i32 %add13.i.i.i, 27
  %shl18.i.i.i = shl i32 %add13.i.i.i, 5
  %or19.i.i.i = or i32 %shr17.i.i.i, %shl18.i.i.i
  %and20.i.i.i = and i32 %or16.i.i.i, %v0.i.i.0.i316
  %neg21.i.i.i = xor i32 %v0.i.i.0.i316, -1
  %and22.i.i.i = and i32 %v2.i.i.0.i318, %neg21.i.i.i
  %xor23.i.i.i = xor i32 %and20.i.i.i, %and22.i.i.i
  %add26.i.i.i = add nsw i32 %ii.i.i.0.i321, 1
  %arrayidx28.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add26.i.i.i
  %47 = load i32* %arrayidx28.i.i.i, align 4
  %add24.i.i.i = add i32 %v3.i.i.0.i319, 1518500249
  %add25.i.i.i = add i32 %add24.i.i.i, %47
  %add29.i.i.i = add i32 %add25.i.i.i, %xor23.i.i.i
  %add30.i.i.i = add i32 %add29.i.i.i, %or19.i.i.i
  %shr31.i.i.i = lshr i32 %v0.i.i.0.i316, 2
  %shl32.i.i.i = shl i32 %v0.i.i.0.i316, 30
  %or33.i.i.i = or i32 %shr31.i.i.i, %shl32.i.i.i
  %shr34.i.i.i = lshr i32 %add30.i.i.i, 27
  %shl35.i.i.i = shl i32 %add30.i.i.i, 5
  %or36.i.i.i = or i32 %shr34.i.i.i, %shl35.i.i.i
  %and37.i.i.i = and i32 %add13.i.i.i, %or33.i.i.i
  %neg38.i.i.i = xor i32 %add13.i.i.i, -1
  %and39.i.i.i = and i32 %or16.i.i.i, %neg38.i.i.i
  %xor40.i.i.i = xor i32 %and37.i.i.i, %and39.i.i.i
  %add43.i.i.i = add nsw i32 %ii.i.i.0.i321, 2
  %arrayidx45.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add43.i.i.i
  %48 = load i32* %arrayidx45.i.i.i, align 4
  %add41.i.i.i = add i32 %v2.i.i.0.i318, 1518500249
  %add42.i.i.i = add i32 %add41.i.i.i, %48
  %add46.i.i.i = add i32 %add42.i.i.i, %xor40.i.i.i
  %add47.i.i.i = add i32 %add46.i.i.i, %or36.i.i.i
  %shr48.i.i.i = lshr i32 %add13.i.i.i, 2
  %shl49.i.i.i = shl i32 %add13.i.i.i, 30
  %or50.i.i.i = or i32 %shr48.i.i.i, %shl49.i.i.i
  %shr51.i.i.i = lshr i32 %add47.i.i.i, 27
  %shl52.i.i.i = shl i32 %add47.i.i.i, 5
  %or53.i.i.i = or i32 %shr51.i.i.i, %shl52.i.i.i
  %and54.i.i.i = and i32 %add30.i.i.i, %or50.i.i.i
  %neg55.i.i.i = xor i32 %add30.i.i.i, -1
  %and56.i.i.i = and i32 %or33.i.i.i, %neg55.i.i.i
  %xor57.i.i.i = xor i32 %and54.i.i.i, %and56.i.i.i
  %add60.i.i.i = add nsw i32 %ii.i.i.0.i321, 3
  %arrayidx62.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add60.i.i.i
  %49 = load i32* %arrayidx62.i.i.i, align 4
  %add58.i.i.i = add i32 %or16.i.i.i, 1518500249
  %add59.i.i.i = add i32 %add58.i.i.i, %49
  %add63.i.i.i = add i32 %add59.i.i.i, %xor57.i.i.i
  %add64.i.i.i = add i32 %add63.i.i.i, %or53.i.i.i
  %shr65.i.i.i = lshr i32 %add30.i.i.i, 2
  %shl66.i.i.i = shl i32 %add30.i.i.i, 30
  %or67.i.i.i = or i32 %shr65.i.i.i, %shl66.i.i.i
  %shr68.i.i.i = lshr i32 %add64.i.i.i, 27
  %shl69.i.i.i = shl i32 %add64.i.i.i, 5
  %or70.i.i.i = or i32 %shr68.i.i.i, %shl69.i.i.i
  %and71.i.i.i = and i32 %add47.i.i.i, %or67.i.i.i
  %neg72.i.i.i = xor i32 %add47.i.i.i, -1
  %and73.i.i.i = and i32 %or50.i.i.i, %neg72.i.i.i
  %xor74.i.i.i = xor i32 %and71.i.i.i, %and73.i.i.i
  %add77.i.i.i = add nsw i32 %ii.i.i.0.i321, 4
  %arrayidx79.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add77.i.i.i
  %50 = load i32* %arrayidx79.i.i.i, align 4
  %add75.i.i.i = add i32 %or33.i.i.i, 1518500249
  %add76.i.i.i = add i32 %add75.i.i.i, %50
  %add80.i.i.i = add i32 %add76.i.i.i, %xor74.i.i.i
  %add81.i.i.i = add i32 %add80.i.i.i, %or70.i.i.i
  %shr82.i.i.i = lshr i32 %add47.i.i.i, 2
  %shl83.i.i.i = shl i32 %add47.i.i.i, 30
  %or84.i.i.i = or i32 %shr82.i.i.i, %shl83.i.i.i
  %add85.i.i.i = add nsw i32 %ii.i.i.0.i321, 5
  %cmp.i.i.i = icmp slt i32 %add85.i.i.i, 15
  %shr.i.i.i = lshr i32 %add81.i.i.i, 27
  %shl.i.i.i = shl i32 %add81.i.i.i, 5
  %or.i.i.i = or i32 %shr.i.i.i, %shl.i.i.i
  %and.i.i.i = and i32 %add64.i.i.i, %or84.i.i.i
  %neg.i.i.i = xor i32 %add64.i.i.i, -1
  %and9.i.i.i = and i32 %or67.i.i.i, %neg.i.i.i
  %xor.i.i.i = xor i32 %and9.i.i.i, %and.i.i.i
  %add.i.i.i = add i32 %xor.i.i.i, 1518500249
  %add10.i.i.i = add i32 %add.i.i.i, %or.i.i.i
  br i1 %cmp.i.i.i, label %for.body.i.i.i, label %for.end.i.i.i

for.end.i.i.i:                                    ; preds = %for.body.i.i.i
  %arraydecay.i.i.i = getelementptr inbounds [16 x i32]* %wbuf29.i.i, i32 0, i32 0
  %arrayidx95.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 15
  %51 = load i32* %arrayidx95.i.i.i, align 4
  %add96.i.i.i = add i32 %add10.i.i.i, %or50.i.i.i
  %add97.i.i.i = add i32 %add96.i.i.i, %51
  %shr98.i.i.i = lshr i32 %add64.i.i.i, 2
  %shl99.i.i.i = shl i32 %add64.i.i.i, 30
  %or100.i.i.i = or i32 %shr98.i.i.i, %shl99.i.i.i
  %shr101.i.i.i = lshr i32 %add97.i.i.i, 27
  %shl102.i.i.i = shl i32 %add97.i.i.i, 5
  %or103.i.i.i = or i32 %shr101.i.i.i, %shl102.i.i.i
  %and104.i.i.i = and i32 %add81.i.i.i, %or100.i.i.i
  %neg105.i.i.i = xor i32 %add81.i.i.i, -1
  %and106.i.i.i = and i32 %or84.i.i.i, %neg105.i.i.i
  %xor107.i.i.i = xor i32 %and104.i.i.i, %and106.i.i.i
  %arrayidx110.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 13
  %52 = load i32* %arrayidx110.i.i.i, align 4
  %arrayidx111.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 8
  %53 = load i32* %arrayidx111.i.i.i, align 4
  %xor112.i.i.i = xor i32 %53, %52
  %arrayidx113.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 2
  %54 = load i32* %arrayidx113.i.i.i, align 4
  %xor114.i.i.i = xor i32 %xor112.i.i.i, %54
  %55 = load i32* %arraydecay.i.i.i, align 4
  %xor116.i.i.i = xor i32 %xor114.i.i.i, %55
  %shl117.i.i.i = shl i32 %xor116.i.i.i, 1
  %shr125.i.i.i = lshr i32 %xor116.i.i.i, 31
  %or126.i.i.i = or i32 %shl117.i.i.i, %shr125.i.i.i
  store i32 %or126.i.i.i, i32* %arraydecay.i.i.i, align 4
  %add108.i.i.i = add i32 %or67.i.i.i, 1518500249
  %add109.i.i.i = add i32 %add108.i.i.i, %xor107.i.i.i
  %add128.i.i.i = add i32 %add109.i.i.i, %or103.i.i.i
  %add129.i.i.i = add i32 %add128.i.i.i, %or126.i.i.i
  %shr130.i.i.i = lshr i32 %add81.i.i.i, 2
  %shl131.i.i.i = shl i32 %add81.i.i.i, 30
  %or132.i.i.i = or i32 %shr130.i.i.i, %shl131.i.i.i
  %shr133.i.i.i = lshr i32 %add129.i.i.i, 27
  %shl134.i.i.i = shl i32 %add129.i.i.i, 5
  %or135.i.i.i = or i32 %shr133.i.i.i, %shl134.i.i.i
  %and136.i.i.i = and i32 %add97.i.i.i, %or132.i.i.i
  %neg137.i.i.i = xor i32 %add97.i.i.i, -1
  %and138.i.i.i = and i32 %or100.i.i.i, %neg137.i.i.i
  %xor139.i.i.i = xor i32 %and136.i.i.i, %and138.i.i.i
  %arrayidx142.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 14
  %56 = load i32* %arrayidx142.i.i.i, align 4
  %arrayidx143.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 9
  %57 = load i32* %arrayidx143.i.i.i, align 4
  %xor144.i.i.i = xor i32 %57, %56
  %arrayidx145.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 3
  %58 = load i32* %arrayidx145.i.i.i, align 4
  %xor146.i.i.i = xor i32 %xor144.i.i.i, %58
  %arrayidx147.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 1
  %59 = load i32* %arrayidx147.i.i.i, align 4
  %xor148.i.i.i = xor i32 %xor146.i.i.i, %59
  %shl149.i.i.i = shl i32 %xor148.i.i.i, 1
  %shr157.i.i.i = lshr i32 %xor148.i.i.i, 31
  %or158.i.i.i = or i32 %shl149.i.i.i, %shr157.i.i.i
  store i32 %or158.i.i.i, i32* %arrayidx147.i.i.i, align 4
  %add140.i.i.i = add i32 %or84.i.i.i, 1518500249
  %add141.i.i.i = add i32 %add140.i.i.i, %xor139.i.i.i
  %add160.i.i.i = add i32 %add141.i.i.i, %or135.i.i.i
  %add161.i.i.i = add i32 %add160.i.i.i, %or158.i.i.i
  %shr162.i.i.i = lshr i32 %add97.i.i.i, 2
  %shl163.i.i.i = shl i32 %add97.i.i.i, 30
  %or164.i.i.i = or i32 %shr162.i.i.i, %shl163.i.i.i
  %shr165.i.i.i = lshr i32 %add161.i.i.i, 27
  %shl166.i.i.i = shl i32 %add161.i.i.i, 5
  %or167.i.i.i = or i32 %shr165.i.i.i, %shl166.i.i.i
  %and168.i.i.i = and i32 %add129.i.i.i, %or164.i.i.i
  %neg169.i.i.i = xor i32 %add129.i.i.i, -1
  %and170.i.i.i = and i32 %or132.i.i.i, %neg169.i.i.i
  %xor171.i.i.i = xor i32 %and168.i.i.i, %and170.i.i.i
  %arrayidx175.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 10
  %60 = load i32* %arrayidx175.i.i.i, align 4
  %arrayidx177.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 4
  %61 = load i32* %arrayidx177.i.i.i, align 4
  %xor176.i.i.i = xor i32 %54, %51
  %xor178.i.i.i = xor i32 %xor176.i.i.i, %60
  %xor180.i.i.i = xor i32 %xor178.i.i.i, %61
  %shl181.i.i.i = shl i32 %xor180.i.i.i, 1
  %shr189.i.i.i = lshr i32 %xor180.i.i.i, 31
  %or190.i.i.i = or i32 %shl181.i.i.i, %shr189.i.i.i
  store i32 %or190.i.i.i, i32* %arrayidx113.i.i.i, align 4
  %add172.i.i.i = add i32 %or100.i.i.i, 1518500249
  %add173.i.i.i = add i32 %add172.i.i.i, %xor171.i.i.i
  %add192.i.i.i = add i32 %add173.i.i.i, %or190.i.i.i
  %add193.i.i.i = add i32 %add192.i.i.i, %or167.i.i.i
  %shr194.i.i.i = lshr i32 %add129.i.i.i, 2
  %shl195.i.i.i = shl i32 %add129.i.i.i, 30
  %or196.i.i.i = or i32 %shr194.i.i.i, %shl195.i.i.i
  %shr197.i.i.i = lshr i32 %add193.i.i.i, 27
  %shl198.i.i.i = shl i32 %add193.i.i.i, 5
  %or199.i.i.i = or i32 %shr197.i.i.i, %shl198.i.i.i
  %and200.i.i.i = and i32 %add161.i.i.i, %or196.i.i.i
  %neg201.i.i.i = xor i32 %add161.i.i.i, -1
  %and202.i.i.i = and i32 %or164.i.i.i, %neg201.i.i.i
  %xor203.i.i.i = xor i32 %and200.i.i.i, %and202.i.i.i
  %arrayidx207.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 11
  %62 = load i32* %arrayidx207.i.i.i, align 4
  %arrayidx209.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 5
  %63 = load i32* %arrayidx209.i.i.i, align 4
  %xor208.i.i.i = xor i32 %58, %or126.i.i.i
  %xor210.i.i.i = xor i32 %xor208.i.i.i, %62
  %xor212.i.i.i = xor i32 %xor210.i.i.i, %63
  %shl213.i.i.i = shl i32 %xor212.i.i.i, 1
  %shr221.i.i.i = lshr i32 %xor212.i.i.i, 31
  %or222.i.i.i = or i32 %shl213.i.i.i, %shr221.i.i.i
  store i32 %or222.i.i.i, i32* %arrayidx145.i.i.i, align 4
  %add204.i.i.i = add i32 %or132.i.i.i, 1518500249
  %add205.i.i.i = add i32 %add204.i.i.i, %xor203.i.i.i
  %add224.i.i.i = add i32 %add205.i.i.i, %or222.i.i.i
  %add225.i.i.i = add i32 %add224.i.i.i, %or199.i.i.i
  %shr226.i.i.i = lshr i32 %add161.i.i.i, 2
  %shl227.i.i.i = shl i32 %add161.i.i.i, 30
  %or228.i.i.i = or i32 %shr226.i.i.i, %shl227.i.i.i
  br label %for.body232.i.i.i

for.body232.i.i.i:                                ; preds = %for.body232.i.i.i, %for.end.i.i.i
  %ii229.i.i.0.i306 = phi i32 [ 20, %for.end.i.i.i ], [ %add432.i.i.i, %for.body232.i.i.i ]
  %v4.i.i.1.i305 = phi i32 [ %or164.i.i.i, %for.end.i.i.i ], [ %or412.i.i.i, %for.body232.i.i.i ]
  %v3.i.i.1.i304 = phi i32 [ %or196.i.i.i, %for.end.i.i.i ], [ %or475.i.i.i, %for.body232.i.i.i ]
  %v2.i.i.1.i303 = phi i32 [ %or228.i.i.i, %for.end.i.i.i ], [ %or538.i.i.i, %for.body232.i.i.i ]
  %v1.i.i.1.i302 = phi i32 [ %add193.i.i.i, %for.end.i.i.i ], [ %add472.i.i.i, %for.body232.i.i.i ]
  %v0.i.i.1.i301 = phi i32 [ %add225.i.i.i, %for.end.i.i.i ], [ %add535.i.i.i, %for.body232.i.i.i ]
  %shr233.i.i.i = lshr i32 %v0.i.i.1.i301, 27
  %shl234.i.i.i = shl i32 %v0.i.i.1.i301, 5
  %or235.i.i.i = or i32 %shr233.i.i.i, %shl234.i.i.i
  %xor236.i.i.i = xor i32 %v2.i.i.1.i303, %v3.i.i.1.i304
  %xor237.i.i.i = xor i32 %xor236.i.i.i, %v1.i.i.1.i302
  %add240.i.i.i = add nsw i32 %ii229.i.i.0.i306, 13
  %and241.i.i.i = and i32 %add240.i.i.i, 15
  %arrayidx243.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and241.i.i.i
  %64 = load i32* %arrayidx243.i.i.i, align 4
  %add244.i.i.i = add nsw i32 %ii229.i.i.0.i306, 8
  %and245.i.i.i = and i32 %add244.i.i.i, 15
  %arrayidx247.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and245.i.i.i
  %65 = load i32* %arrayidx247.i.i.i, align 4
  %xor248.i.i.i = xor i32 %65, %64
  %add249.i.i.i = add nsw i32 %ii229.i.i.0.i306, 2
  %and250.i.i.i = and i32 %add249.i.i.i, 15
  %arrayidx252.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and250.i.i.i
  %66 = load i32* %arrayidx252.i.i.i, align 4
  %xor253.i.i.i = xor i32 %xor248.i.i.i, %66
  %and254.i.i.i = and i32 %ii229.i.i.0.i306, 15
  %arrayidx256.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and254.i.i.i
  %67 = load i32* %arrayidx256.i.i.i, align 4
  %xor257.i.i.i = xor i32 %xor253.i.i.i, %67
  %shl258.i.i.i = shl i32 %xor257.i.i.i, 1
  %shr277.i.i.i = lshr i32 %xor257.i.i.i, 31
  %or278.i.i.i = or i32 %shl258.i.i.i, %shr277.i.i.i
  store i32 %or278.i.i.i, i32* %arrayidx256.i.i.i, align 4
  %add238.i.i.i = add i32 %v4.i.i.1.i305, 1859775393
  %add239.i.i.i = add i32 %add238.i.i.i, %xor237.i.i.i
  %add282.i.i.i = add i32 %add239.i.i.i, %or235.i.i.i
  %add283.i.i.i = add i32 %add282.i.i.i, %or278.i.i.i
  %shr284.i.i.i = lshr i32 %v1.i.i.1.i302, 2
  %shl285.i.i.i = shl i32 %v1.i.i.1.i302, 30
  %or286.i.i.i = or i32 %shr284.i.i.i, %shl285.i.i.i
  %shr287.i.i.i = lshr i32 %add283.i.i.i, 27
  %shl288.i.i.i = shl i32 %add283.i.i.i, 5
  %or289.i.i.i = or i32 %shr287.i.i.i, %shl288.i.i.i
  %xor290.i.i.i = xor i32 %v0.i.i.1.i301, %v2.i.i.1.i303
  %xor291.i.i.i = xor i32 %xor290.i.i.i, %or286.i.i.i
  %add294.i.i.i = add nsw i32 %ii229.i.i.0.i306, 1
  %add295.i.i.i = add nsw i32 %ii229.i.i.0.i306, 14
  %and296.i.i.i = and i32 %add295.i.i.i, 15
  %arrayidx298.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and296.i.i.i
  %68 = load i32* %arrayidx298.i.i.i, align 4
  %add300.i.i.i = add nsw i32 %ii229.i.i.0.i306, 9
  %and301.i.i.i = and i32 %add300.i.i.i, 15
  %arrayidx303.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and301.i.i.i
  %69 = load i32* %arrayidx303.i.i.i, align 4
  %xor304.i.i.i = xor i32 %69, %68
  %add306.i.i.i = add nsw i32 %ii229.i.i.0.i306, 3
  %and307.i.i.i = and i32 %add306.i.i.i, 15
  %arrayidx309.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and307.i.i.i
  %70 = load i32* %arrayidx309.i.i.i, align 4
  %xor310.i.i.i = xor i32 %xor304.i.i.i, %70
  %and312.i.i.i = and i32 %add294.i.i.i, 15
  %arrayidx314.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and312.i.i.i
  %71 = load i32* %arrayidx314.i.i.i, align 4
  %xor315.i.i.i = xor i32 %xor310.i.i.i, %71
  %shl316.i.i.i = shl i32 %xor315.i.i.i, 1
  %shr339.i.i.i = lshr i32 %xor315.i.i.i, 31
  %or340.i.i.i = or i32 %shl316.i.i.i, %shr339.i.i.i
  store i32 %or340.i.i.i, i32* %arrayidx314.i.i.i, align 4
  %add292.i.i.i = add i32 %v3.i.i.1.i304, 1859775393
  %add293.i.i.i = add i32 %add292.i.i.i, %xor291.i.i.i
  %add345.i.i.i = add i32 %add293.i.i.i, %or289.i.i.i
  %add346.i.i.i = add i32 %add345.i.i.i, %or340.i.i.i
  %shr347.i.i.i = lshr i32 %v0.i.i.1.i301, 2
  %shl348.i.i.i = shl i32 %v0.i.i.1.i301, 30
  %or349.i.i.i = or i32 %shr347.i.i.i, %shl348.i.i.i
  %shr350.i.i.i = lshr i32 %add346.i.i.i, 27
  %shl351.i.i.i = shl i32 %add346.i.i.i, 5
  %or352.i.i.i = or i32 %shr350.i.i.i, %shl351.i.i.i
  %xor353.i.i.i = xor i32 %or349.i.i.i, %or286.i.i.i
  %xor354.i.i.i = xor i32 %xor353.i.i.i, %add283.i.i.i
  %add358.i.i.i = add nsw i32 %ii229.i.i.0.i306, 15
  %and359.i.i.i = and i32 %add358.i.i.i, 15
  %arrayidx361.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and359.i.i.i
  %72 = load i32* %arrayidx361.i.i.i, align 4
  %add363.i.i.i = add nsw i32 %ii229.i.i.0.i306, 10
  %and364.i.i.i = and i32 %add363.i.i.i, 15
  %arrayidx366.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and364.i.i.i
  %73 = load i32* %arrayidx366.i.i.i, align 4
  %xor367.i.i.i = xor i32 %73, %72
  %add369.i.i.i = add nsw i32 %ii229.i.i.0.i306, 4
  %and370.i.i.i = and i32 %add369.i.i.i, 15
  %arrayidx372.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and370.i.i.i
  %74 = load i32* %arrayidx372.i.i.i, align 4
  %xor373.i.i.i = xor i32 %xor367.i.i.i, %74
  %75 = load i32* %arrayidx252.i.i.i, align 4
  %xor378.i.i.i = xor i32 %xor373.i.i.i, %75
  %shl379.i.i.i = shl i32 %xor378.i.i.i, 1
  %shr402.i.i.i = lshr i32 %xor378.i.i.i, 31
  %or403.i.i.i = or i32 %shl379.i.i.i, %shr402.i.i.i
  store i32 %or403.i.i.i, i32* %arrayidx252.i.i.i, align 4
  %add355.i.i.i = add i32 %v2.i.i.1.i303, 1859775393
  %add356.i.i.i = add i32 %add355.i.i.i, %xor354.i.i.i
  %add408.i.i.i = add i32 %add356.i.i.i, %or352.i.i.i
  %add409.i.i.i = add i32 %add408.i.i.i, %or403.i.i.i
  %shr410.i.i.i = lshr i32 %add283.i.i.i, 2
  %shl411.i.i.i = shl i32 %add283.i.i.i, 30
  %or412.i.i.i = or i32 %shr410.i.i.i, %shl411.i.i.i
  %shr413.i.i.i = lshr i32 %add409.i.i.i, 27
  %shl414.i.i.i = shl i32 %add409.i.i.i, 5
  %or415.i.i.i = or i32 %shr413.i.i.i, %shl414.i.i.i
  %xor416.i.i.i = xor i32 %or412.i.i.i, %or349.i.i.i
  %xor417.i.i.i = xor i32 %xor416.i.i.i, %add346.i.i.i
  %76 = load i32* %arrayidx256.i.i.i, align 4
  %add426.i.i.i = add nsw i32 %ii229.i.i.0.i306, 11
  %and427.i.i.i = and i32 %add426.i.i.i, 15
  %arrayidx429.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and427.i.i.i
  %77 = load i32* %arrayidx429.i.i.i, align 4
  %xor430.i.i.i = xor i32 %77, %76
  %add432.i.i.i = add nsw i32 %ii229.i.i.0.i306, 5
  %and433.i.i.i = and i32 %add432.i.i.i, 15
  %arrayidx435.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and433.i.i.i
  %78 = load i32* %arrayidx435.i.i.i, align 4
  %xor436.i.i.i = xor i32 %xor430.i.i.i, %78
  %79 = load i32* %arrayidx309.i.i.i, align 4
  %xor441.i.i.i = xor i32 %xor436.i.i.i, %79
  %shl442.i.i.i = shl i32 %xor441.i.i.i, 1
  %shr465.i.i.i = lshr i32 %xor441.i.i.i, 31
  %or466.i.i.i = or i32 %shl442.i.i.i, %shr465.i.i.i
  store i32 %or466.i.i.i, i32* %arrayidx309.i.i.i, align 4
  %add418.i.i.i = add i32 %or286.i.i.i, 1859775393
  %add419.i.i.i = add i32 %add418.i.i.i, %xor417.i.i.i
  %add471.i.i.i = add i32 %add419.i.i.i, %or415.i.i.i
  %add472.i.i.i = add i32 %add471.i.i.i, %or466.i.i.i
  %shr473.i.i.i = lshr i32 %add346.i.i.i, 2
  %shl474.i.i.i = shl i32 %add346.i.i.i, 30
  %or475.i.i.i = or i32 %shr473.i.i.i, %shl474.i.i.i
  %shr476.i.i.i = lshr i32 %add472.i.i.i, 27
  %shl477.i.i.i = shl i32 %add472.i.i.i, 5
  %or478.i.i.i = or i32 %shr476.i.i.i, %shl477.i.i.i
  %xor479.i.i.i = xor i32 %or475.i.i.i, %or412.i.i.i
  %xor480.i.i.i = xor i32 %xor479.i.i.i, %add409.i.i.i
  %80 = load i32* %arrayidx314.i.i.i, align 4
  %add489.i.i.i = add nsw i32 %ii229.i.i.0.i306, 12
  %and490.i.i.i = and i32 %add489.i.i.i, 15
  %arrayidx492.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and490.i.i.i
  %81 = load i32* %arrayidx492.i.i.i, align 4
  %xor493.i.i.i = xor i32 %81, %80
  %add495.i.i.i = add nsw i32 %ii229.i.i.0.i306, 6
  %and496.i.i.i = and i32 %add495.i.i.i, 15
  %arrayidx498.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and496.i.i.i
  %82 = load i32* %arrayidx498.i.i.i, align 4
  %xor499.i.i.i = xor i32 %xor493.i.i.i, %82
  %83 = load i32* %arrayidx372.i.i.i, align 4
  %xor504.i.i.i = xor i32 %xor499.i.i.i, %83
  %shl505.i.i.i = shl i32 %xor504.i.i.i, 1
  %shr528.i.i.i = lshr i32 %xor504.i.i.i, 31
  %or529.i.i.i = or i32 %shl505.i.i.i, %shr528.i.i.i
  store i32 %or529.i.i.i, i32* %arrayidx372.i.i.i, align 4
  %add481.i.i.i = add i32 %or349.i.i.i, 1859775393
  %add482.i.i.i = add i32 %add481.i.i.i, %xor480.i.i.i
  %add534.i.i.i = add i32 %add482.i.i.i, %or478.i.i.i
  %add535.i.i.i = add i32 %add534.i.i.i, %or529.i.i.i
  %shr536.i.i.i = lshr i32 %add409.i.i.i, 2
  %shl537.i.i.i = shl i32 %add409.i.i.i, 30
  %or538.i.i.i = or i32 %shr536.i.i.i, %shl537.i.i.i
  %cmp231.i.i.i = icmp slt i32 %add432.i.i.i, 40
  br i1 %cmp231.i.i.i, label %for.body232.i.i.i, label %for.body545.i.i.i

for.body545.i.i.i:                                ; preds = %for.body545.i.i.i, %for.body232.i.i.i
  %ii542.i.i.0.i300 = phi i32 [ %add757.i.i.i, %for.body545.i.i.i ], [ 40, %for.body232.i.i.i ]
  %v4.i.i.2.i299 = phi i32 [ %or734.i.i.i, %for.body545.i.i.i ], [ %or412.i.i.i, %for.body232.i.i.i ]
  %v3.i.i.2.i298 = phi i32 [ %or800.i.i.i, %for.body545.i.i.i ], [ %or475.i.i.i, %for.body232.i.i.i ]
  %v2.i.i.2.i297 = phi i32 [ %or866.i.i.i, %for.body545.i.i.i ], [ %or538.i.i.i, %for.body232.i.i.i ]
  %v1.i.i.2.i296 = phi i32 [ %add797.i.i.i, %for.body545.i.i.i ], [ %add472.i.i.i, %for.body232.i.i.i ]
  %v0.i.i.2.i295 = phi i32 [ %add863.i.i.i, %for.body545.i.i.i ], [ %add535.i.i.i, %for.body232.i.i.i ]
  %shr546.i.i.i = lshr i32 %v0.i.i.2.i295, 27
  %shl547.i.i.i = shl i32 %v0.i.i.2.i295, 5
  %or548.i.i.i = or i32 %shr546.i.i.i, %shl547.i.i.i
  %and550.i.i.i85 = xor i32 %v2.i.i.2.i297, %v3.i.i.2.i298
  %xor551.i.i.i = and i32 %and550.i.i.i85, %v1.i.i.2.i296
  %and552.i.i.i = and i32 %v2.i.i.2.i297, %v3.i.i.2.i298
  %xor553.i.i.i = xor i32 %xor551.i.i.i, %and552.i.i.i
  %add556.i.i.i = add nsw i32 %ii542.i.i.0.i300, 13
  %and557.i.i.i = and i32 %add556.i.i.i, 15
  %arrayidx559.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and557.i.i.i
  %84 = load i32* %arrayidx559.i.i.i, align 4
  %add560.i.i.i = add nsw i32 %ii542.i.i.0.i300, 8
  %and561.i.i.i = and i32 %add560.i.i.i, 15
  %arrayidx563.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and561.i.i.i
  %85 = load i32* %arrayidx563.i.i.i, align 4
  %xor564.i.i.i = xor i32 %85, %84
  %add565.i.i.i = add nsw i32 %ii542.i.i.0.i300, 2
  %and566.i.i.i = and i32 %add565.i.i.i, 15
  %arrayidx568.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and566.i.i.i
  %86 = load i32* %arrayidx568.i.i.i, align 4
  %xor569.i.i.i = xor i32 %xor564.i.i.i, %86
  %and570.i.i.i = and i32 %ii542.i.i.0.i300, 15
  %arrayidx572.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and570.i.i.i
  %87 = load i32* %arrayidx572.i.i.i, align 4
  %xor573.i.i.i = xor i32 %xor569.i.i.i, %87
  %shl574.i.i.i = shl i32 %xor573.i.i.i, 1
  %shr593.i.i.i = lshr i32 %xor573.i.i.i, 31
  %or594.i.i.i = or i32 %shl574.i.i.i, %shr593.i.i.i
  store i32 %or594.i.i.i, i32* %arrayidx572.i.i.i, align 4
  %add554.i.i.i = add i32 %v4.i.i.2.i299, -1894007588
  %add555.i.i.i = add i32 %add554.i.i.i, %xor553.i.i.i
  %add598.i.i.i = add i32 %add555.i.i.i, %or548.i.i.i
  %add599.i.i.i = add i32 %add598.i.i.i, %or594.i.i.i
  %shr600.i.i.i = lshr i32 %v1.i.i.2.i296, 2
  %shl601.i.i.i = shl i32 %v1.i.i.2.i296, 30
  %or602.i.i.i = or i32 %shr600.i.i.i, %shl601.i.i.i
  %shr603.i.i.i = lshr i32 %add599.i.i.i, 27
  %shl604.i.i.i = shl i32 %add599.i.i.i, 5
  %or605.i.i.i = or i32 %shr603.i.i.i, %shl604.i.i.i
  %and607.i.i.i90 = xor i32 %or602.i.i.i, %v2.i.i.2.i297
  %xor608.i.i.i = and i32 %and607.i.i.i90, %v0.i.i.2.i295
  %and609.i.i.i = and i32 %or602.i.i.i, %v2.i.i.2.i297
  %xor610.i.i.i = xor i32 %xor608.i.i.i, %and609.i.i.i
  %add613.i.i.i = add nsw i32 %ii542.i.i.0.i300, 1
  %add614.i.i.i = add nsw i32 %ii542.i.i.0.i300, 14
  %and615.i.i.i = and i32 %add614.i.i.i, 15
  %arrayidx617.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and615.i.i.i
  %88 = load i32* %arrayidx617.i.i.i, align 4
  %add619.i.i.i = add nsw i32 %ii542.i.i.0.i300, 9
  %and620.i.i.i = and i32 %add619.i.i.i, 15
  %arrayidx622.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and620.i.i.i
  %89 = load i32* %arrayidx622.i.i.i, align 4
  %xor623.i.i.i = xor i32 %89, %88
  %add625.i.i.i = add nsw i32 %ii542.i.i.0.i300, 3
  %and626.i.i.i = and i32 %add625.i.i.i, 15
  %arrayidx628.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and626.i.i.i
  %90 = load i32* %arrayidx628.i.i.i, align 4
  %xor629.i.i.i = xor i32 %xor623.i.i.i, %90
  %and631.i.i.i = and i32 %add613.i.i.i, 15
  %arrayidx633.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and631.i.i.i
  %91 = load i32* %arrayidx633.i.i.i, align 4
  %xor634.i.i.i = xor i32 %xor629.i.i.i, %91
  %shl635.i.i.i = shl i32 %xor634.i.i.i, 1
  %shr658.i.i.i = lshr i32 %xor634.i.i.i, 31
  %or659.i.i.i = or i32 %shl635.i.i.i, %shr658.i.i.i
  store i32 %or659.i.i.i, i32* %arrayidx633.i.i.i, align 4
  %add611.i.i.i = add i32 %v3.i.i.2.i298, -1894007588
  %add612.i.i.i = add i32 %add611.i.i.i, %xor610.i.i.i
  %add664.i.i.i = add i32 %add612.i.i.i, %or605.i.i.i
  %add665.i.i.i = add i32 %add664.i.i.i, %or659.i.i.i
  %shr666.i.i.i = lshr i32 %v0.i.i.2.i295, 2
  %shl667.i.i.i = shl i32 %v0.i.i.2.i295, 30
  %or668.i.i.i = or i32 %shr666.i.i.i, %shl667.i.i.i
  %shr669.i.i.i = lshr i32 %add665.i.i.i, 27
  %shl670.i.i.i = shl i32 %add665.i.i.i, 5
  %or671.i.i.i = or i32 %shr669.i.i.i, %shl670.i.i.i
  %and673.i.i.i95 = xor i32 %or668.i.i.i, %or602.i.i.i
  %xor674.i.i.i = and i32 %add599.i.i.i, %and673.i.i.i95
  %and675.i.i.i = and i32 %or668.i.i.i, %or602.i.i.i
  %xor676.i.i.i = xor i32 %xor674.i.i.i, %and675.i.i.i
  %add680.i.i.i = add nsw i32 %ii542.i.i.0.i300, 15
  %and681.i.i.i = and i32 %add680.i.i.i, 15
  %arrayidx683.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and681.i.i.i
  %92 = load i32* %arrayidx683.i.i.i, align 4
  %add685.i.i.i = add nsw i32 %ii542.i.i.0.i300, 10
  %and686.i.i.i = and i32 %add685.i.i.i, 15
  %arrayidx688.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and686.i.i.i
  %93 = load i32* %arrayidx688.i.i.i, align 4
  %xor689.i.i.i = xor i32 %93, %92
  %add691.i.i.i = add nsw i32 %ii542.i.i.0.i300, 4
  %and692.i.i.i = and i32 %add691.i.i.i, 15
  %arrayidx694.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and692.i.i.i
  %94 = load i32* %arrayidx694.i.i.i, align 4
  %xor695.i.i.i = xor i32 %xor689.i.i.i, %94
  %95 = load i32* %arrayidx568.i.i.i, align 4
  %xor700.i.i.i = xor i32 %xor695.i.i.i, %95
  %shl701.i.i.i = shl i32 %xor700.i.i.i, 1
  %shr724.i.i.i = lshr i32 %xor700.i.i.i, 31
  %or725.i.i.i = or i32 %shl701.i.i.i, %shr724.i.i.i
  store i32 %or725.i.i.i, i32* %arrayidx568.i.i.i, align 4
  %add677.i.i.i = add i32 %v2.i.i.2.i297, -1894007588
  %add678.i.i.i = add i32 %add677.i.i.i, %xor676.i.i.i
  %add730.i.i.i = add i32 %add678.i.i.i, %or671.i.i.i
  %add731.i.i.i = add i32 %add730.i.i.i, %or725.i.i.i
  %shr732.i.i.i = lshr i32 %add599.i.i.i, 2
  %shl733.i.i.i = shl i32 %add599.i.i.i, 30
  %or734.i.i.i = or i32 %shr732.i.i.i, %shl733.i.i.i
  %shr735.i.i.i = lshr i32 %add731.i.i.i, 27
  %shl736.i.i.i = shl i32 %add731.i.i.i, 5
  %or737.i.i.i = or i32 %shr735.i.i.i, %shl736.i.i.i
  %and739.i.i.i99 = xor i32 %or734.i.i.i, %or668.i.i.i
  %xor740.i.i.i = and i32 %add665.i.i.i, %and739.i.i.i99
  %and741.i.i.i = and i32 %or734.i.i.i, %or668.i.i.i
  %xor742.i.i.i = xor i32 %xor740.i.i.i, %and741.i.i.i
  %96 = load i32* %arrayidx572.i.i.i, align 4
  %add751.i.i.i = add nsw i32 %ii542.i.i.0.i300, 11
  %and752.i.i.i = and i32 %add751.i.i.i, 15
  %arrayidx754.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and752.i.i.i
  %97 = load i32* %arrayidx754.i.i.i, align 4
  %xor755.i.i.i = xor i32 %97, %96
  %add757.i.i.i = add nsw i32 %ii542.i.i.0.i300, 5
  %and758.i.i.i = and i32 %add757.i.i.i, 15
  %arrayidx760.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and758.i.i.i
  %98 = load i32* %arrayidx760.i.i.i, align 4
  %xor761.i.i.i = xor i32 %xor755.i.i.i, %98
  %99 = load i32* %arrayidx628.i.i.i, align 4
  %xor766.i.i.i = xor i32 %xor761.i.i.i, %99
  %shl767.i.i.i = shl i32 %xor766.i.i.i, 1
  %shr790.i.i.i = lshr i32 %xor766.i.i.i, 31
  %or791.i.i.i = or i32 %shl767.i.i.i, %shr790.i.i.i
  store i32 %or791.i.i.i, i32* %arrayidx628.i.i.i, align 4
  %add743.i.i.i = add i32 %or602.i.i.i, -1894007588
  %add744.i.i.i = add i32 %add743.i.i.i, %xor742.i.i.i
  %add796.i.i.i = add i32 %add744.i.i.i, %or737.i.i.i
  %add797.i.i.i = add i32 %add796.i.i.i, %or791.i.i.i
  %shr798.i.i.i = lshr i32 %add665.i.i.i, 2
  %shl799.i.i.i = shl i32 %add665.i.i.i, 30
  %or800.i.i.i = or i32 %shr798.i.i.i, %shl799.i.i.i
  %shr801.i.i.i = lshr i32 %add797.i.i.i, 27
  %shl802.i.i.i = shl i32 %add797.i.i.i, 5
  %or803.i.i.i = or i32 %shr801.i.i.i, %shl802.i.i.i
  %and805.i.i.i104 = xor i32 %or800.i.i.i, %or734.i.i.i
  %xor806.i.i.i = and i32 %add731.i.i.i, %and805.i.i.i104
  %and807.i.i.i = and i32 %or800.i.i.i, %or734.i.i.i
  %xor808.i.i.i = xor i32 %xor806.i.i.i, %and807.i.i.i
  %100 = load i32* %arrayidx633.i.i.i, align 4
  %add817.i.i.i = add nsw i32 %ii542.i.i.0.i300, 12
  %and818.i.i.i = and i32 %add817.i.i.i, 15
  %arrayidx820.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and818.i.i.i
  %101 = load i32* %arrayidx820.i.i.i, align 4
  %xor821.i.i.i = xor i32 %101, %100
  %add823.i.i.i = add nsw i32 %ii542.i.i.0.i300, 6
  %and824.i.i.i = and i32 %add823.i.i.i, 15
  %arrayidx826.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and824.i.i.i
  %102 = load i32* %arrayidx826.i.i.i, align 4
  %xor827.i.i.i = xor i32 %xor821.i.i.i, %102
  %103 = load i32* %arrayidx694.i.i.i, align 4
  %xor832.i.i.i = xor i32 %xor827.i.i.i, %103
  %shl833.i.i.i = shl i32 %xor832.i.i.i, 1
  %shr856.i.i.i = lshr i32 %xor832.i.i.i, 31
  %or857.i.i.i = or i32 %shl833.i.i.i, %shr856.i.i.i
  store i32 %or857.i.i.i, i32* %arrayidx694.i.i.i, align 4
  %add809.i.i.i = add i32 %or668.i.i.i, -1894007588
  %add810.i.i.i = add i32 %add809.i.i.i, %xor808.i.i.i
  %add862.i.i.i = add i32 %add810.i.i.i, %or803.i.i.i
  %add863.i.i.i = add i32 %add862.i.i.i, %or857.i.i.i
  %shr864.i.i.i = lshr i32 %add731.i.i.i, 2
  %shl865.i.i.i = shl i32 %add731.i.i.i, 30
  %or866.i.i.i = or i32 %shr864.i.i.i, %shl865.i.i.i
  %cmp544.i.i.i = icmp slt i32 %add757.i.i.i, 60
  br i1 %cmp544.i.i.i, label %for.body545.i.i.i, label %for.body873.i.i.i

for.body873.i.i.i:                                ; preds = %for.body873.i.i.i, %for.body545.i.i.i
  %ii870.i.i.0.i294 = phi i32 [ %add1073.i.i.i, %for.body873.i.i.i ], [ 60, %for.body545.i.i.i ]
  %v4.i.i.3.i293 = phi i32 [ %or1053.i.i.i, %for.body873.i.i.i ], [ %or734.i.i.i, %for.body545.i.i.i ]
  %v3.i.i.3.i292 = phi i32 [ %or1116.i.i.i, %for.body873.i.i.i ], [ %or800.i.i.i, %for.body545.i.i.i ]
  %v2.i.i.3.i291 = phi i32 [ %or1179.i.i.i, %for.body873.i.i.i ], [ %or866.i.i.i, %for.body545.i.i.i ]
  %v1.i.i.3.i290 = phi i32 [ %add1113.i.i.i, %for.body873.i.i.i ], [ %add797.i.i.i, %for.body545.i.i.i ]
  %v0.i.i.3.i289 = phi i32 [ %add1176.i.i.i, %for.body873.i.i.i ], [ %add863.i.i.i, %for.body545.i.i.i ]
  %shr874.i.i.i = lshr i32 %v0.i.i.3.i289, 27
  %shl875.i.i.i = shl i32 %v0.i.i.3.i289, 5
  %or876.i.i.i = or i32 %shr874.i.i.i, %shl875.i.i.i
  %xor877.i.i.i = xor i32 %v2.i.i.3.i291, %v3.i.i.3.i292
  %xor878.i.i.i = xor i32 %xor877.i.i.i, %v1.i.i.3.i290
  %add881.i.i.i = add nsw i32 %ii870.i.i.0.i294, 13
  %and882.i.i.i = and i32 %add881.i.i.i, 15
  %arrayidx884.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and882.i.i.i
  %104 = load i32* %arrayidx884.i.i.i, align 4
  %add885.i.i.i = add nsw i32 %ii870.i.i.0.i294, 8
  %and886.i.i.i = and i32 %add885.i.i.i, 15
  %arrayidx888.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and886.i.i.i
  %105 = load i32* %arrayidx888.i.i.i, align 4
  %xor889.i.i.i = xor i32 %105, %104
  %add890.i.i.i = add nsw i32 %ii870.i.i.0.i294, 2
  %and891.i.i.i = and i32 %add890.i.i.i, 15
  %arrayidx893.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and891.i.i.i
  %106 = load i32* %arrayidx893.i.i.i, align 4
  %xor894.i.i.i = xor i32 %xor889.i.i.i, %106
  %and895.i.i.i = and i32 %ii870.i.i.0.i294, 15
  %arrayidx897.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and895.i.i.i
  %107 = load i32* %arrayidx897.i.i.i, align 4
  %xor898.i.i.i = xor i32 %xor894.i.i.i, %107
  %shl899.i.i.i = shl i32 %xor898.i.i.i, 1
  %shr918.i.i.i = lshr i32 %xor898.i.i.i, 31
  %or919.i.i.i = or i32 %shl899.i.i.i, %shr918.i.i.i
  store i32 %or919.i.i.i, i32* %arrayidx897.i.i.i, align 4
  %add879.i.i.i = add i32 %v4.i.i.3.i293, -899497514
  %add880.i.i.i = add i32 %add879.i.i.i, %xor878.i.i.i
  %add923.i.i.i = add i32 %add880.i.i.i, %or876.i.i.i
  %add924.i.i.i = add i32 %add923.i.i.i, %or919.i.i.i
  %shr925.i.i.i = lshr i32 %v1.i.i.3.i290, 2
  %shl926.i.i.i = shl i32 %v1.i.i.3.i290, 30
  %or927.i.i.i = or i32 %shr925.i.i.i, %shl926.i.i.i
  %shr928.i.i.i = lshr i32 %add924.i.i.i, 27
  %shl929.i.i.i = shl i32 %add924.i.i.i, 5
  %or930.i.i.i = or i32 %shr928.i.i.i, %shl929.i.i.i
  %xor931.i.i.i = xor i32 %v0.i.i.3.i289, %v2.i.i.3.i291
  %xor932.i.i.i = xor i32 %xor931.i.i.i, %or927.i.i.i
  %add935.i.i.i = add nsw i32 %ii870.i.i.0.i294, 1
  %add936.i.i.i = add nsw i32 %ii870.i.i.0.i294, 14
  %and937.i.i.i = and i32 %add936.i.i.i, 15
  %arrayidx939.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and937.i.i.i
  %108 = load i32* %arrayidx939.i.i.i, align 4
  %add941.i.i.i = add nsw i32 %ii870.i.i.0.i294, 9
  %and942.i.i.i = and i32 %add941.i.i.i, 15
  %arrayidx944.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and942.i.i.i
  %109 = load i32* %arrayidx944.i.i.i, align 4
  %xor945.i.i.i = xor i32 %109, %108
  %add947.i.i.i = add nsw i32 %ii870.i.i.0.i294, 3
  %and948.i.i.i = and i32 %add947.i.i.i, 15
  %arrayidx950.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and948.i.i.i
  %110 = load i32* %arrayidx950.i.i.i, align 4
  %xor951.i.i.i = xor i32 %xor945.i.i.i, %110
  %and953.i.i.i = and i32 %add935.i.i.i, 15
  %arrayidx955.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and953.i.i.i
  %111 = load i32* %arrayidx955.i.i.i, align 4
  %xor956.i.i.i = xor i32 %xor951.i.i.i, %111
  %shl957.i.i.i = shl i32 %xor956.i.i.i, 1
  %shr980.i.i.i = lshr i32 %xor956.i.i.i, 31
  %or981.i.i.i = or i32 %shl957.i.i.i, %shr980.i.i.i
  store i32 %or981.i.i.i, i32* %arrayidx955.i.i.i, align 4
  %add933.i.i.i = add i32 %v3.i.i.3.i292, -899497514
  %add934.i.i.i = add i32 %add933.i.i.i, %xor932.i.i.i
  %add986.i.i.i = add i32 %add934.i.i.i, %or930.i.i.i
  %add987.i.i.i = add i32 %add986.i.i.i, %or981.i.i.i
  %shr988.i.i.i = lshr i32 %v0.i.i.3.i289, 2
  %shl989.i.i.i = shl i32 %v0.i.i.3.i289, 30
  %or990.i.i.i = or i32 %shr988.i.i.i, %shl989.i.i.i
  %shr991.i.i.i = lshr i32 %add987.i.i.i, 27
  %shl992.i.i.i = shl i32 %add987.i.i.i, 5
  %or993.i.i.i = or i32 %shr991.i.i.i, %shl992.i.i.i
  %xor994.i.i.i = xor i32 %or990.i.i.i, %or927.i.i.i
  %xor995.i.i.i = xor i32 %xor994.i.i.i, %add924.i.i.i
  %add999.i.i.i = add nsw i32 %ii870.i.i.0.i294, 15
  %and1000.i.i.i = and i32 %add999.i.i.i, 15
  %arrayidx1002.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1000.i.i.i
  %112 = load i32* %arrayidx1002.i.i.i, align 4
  %add1004.i.i.i = add nsw i32 %ii870.i.i.0.i294, 10
  %and1005.i.i.i = and i32 %add1004.i.i.i, 15
  %arrayidx1007.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1005.i.i.i
  %113 = load i32* %arrayidx1007.i.i.i, align 4
  %xor1008.i.i.i = xor i32 %113, %112
  %add1010.i.i.i = add nsw i32 %ii870.i.i.0.i294, 4
  %and1011.i.i.i = and i32 %add1010.i.i.i, 15
  %arrayidx1013.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1011.i.i.i
  %114 = load i32* %arrayidx1013.i.i.i, align 4
  %xor1014.i.i.i = xor i32 %xor1008.i.i.i, %114
  %115 = load i32* %arrayidx893.i.i.i, align 4
  %xor1019.i.i.i = xor i32 %xor1014.i.i.i, %115
  %shl1020.i.i.i = shl i32 %xor1019.i.i.i, 1
  %shr1043.i.i.i = lshr i32 %xor1019.i.i.i, 31
  %or1044.i.i.i = or i32 %shl1020.i.i.i, %shr1043.i.i.i
  store i32 %or1044.i.i.i, i32* %arrayidx893.i.i.i, align 4
  %add996.i.i.i = add i32 %v2.i.i.3.i291, -899497514
  %add997.i.i.i = add i32 %add996.i.i.i, %xor995.i.i.i
  %add1049.i.i.i = add i32 %add997.i.i.i, %or993.i.i.i
  %add1050.i.i.i = add i32 %add1049.i.i.i, %or1044.i.i.i
  %shr1051.i.i.i = lshr i32 %add924.i.i.i, 2
  %shl1052.i.i.i = shl i32 %add924.i.i.i, 30
  %or1053.i.i.i = or i32 %shr1051.i.i.i, %shl1052.i.i.i
  %shr1054.i.i.i = lshr i32 %add1050.i.i.i, 27
  %shl1055.i.i.i = shl i32 %add1050.i.i.i, 5
  %or1056.i.i.i = or i32 %shr1054.i.i.i, %shl1055.i.i.i
  %xor1057.i.i.i = xor i32 %or1053.i.i.i, %or990.i.i.i
  %xor1058.i.i.i = xor i32 %xor1057.i.i.i, %add987.i.i.i
  %116 = load i32* %arrayidx897.i.i.i, align 4
  %add1067.i.i.i = add nsw i32 %ii870.i.i.0.i294, 11
  %and1068.i.i.i = and i32 %add1067.i.i.i, 15
  %arrayidx1070.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1068.i.i.i
  %117 = load i32* %arrayidx1070.i.i.i, align 4
  %xor1071.i.i.i = xor i32 %117, %116
  %add1073.i.i.i = add nsw i32 %ii870.i.i.0.i294, 5
  %and1074.i.i.i = and i32 %add1073.i.i.i, 15
  %arrayidx1076.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1074.i.i.i
  %118 = load i32* %arrayidx1076.i.i.i, align 4
  %xor1077.i.i.i = xor i32 %xor1071.i.i.i, %118
  %119 = load i32* %arrayidx950.i.i.i, align 4
  %xor1082.i.i.i = xor i32 %xor1077.i.i.i, %119
  %shl1083.i.i.i = shl i32 %xor1082.i.i.i, 1
  %shr1106.i.i.i = lshr i32 %xor1082.i.i.i, 31
  %or1107.i.i.i = or i32 %shl1083.i.i.i, %shr1106.i.i.i
  store i32 %or1107.i.i.i, i32* %arrayidx950.i.i.i, align 4
  %add1059.i.i.i = add i32 %or927.i.i.i, -899497514
  %add1060.i.i.i = add i32 %add1059.i.i.i, %xor1058.i.i.i
  %add1112.i.i.i = add i32 %add1060.i.i.i, %or1056.i.i.i
  %add1113.i.i.i = add i32 %add1112.i.i.i, %or1107.i.i.i
  %shr1114.i.i.i = lshr i32 %add987.i.i.i, 2
  %shl1115.i.i.i = shl i32 %add987.i.i.i, 30
  %or1116.i.i.i = or i32 %shr1114.i.i.i, %shl1115.i.i.i
  %shr1117.i.i.i = lshr i32 %add1113.i.i.i, 27
  %shl1118.i.i.i = shl i32 %add1113.i.i.i, 5
  %or1119.i.i.i = or i32 %shr1117.i.i.i, %shl1118.i.i.i
  %xor1120.i.i.i = xor i32 %or1116.i.i.i, %or1053.i.i.i
  %xor1121.i.i.i = xor i32 %xor1120.i.i.i, %add1050.i.i.i
  %120 = load i32* %arrayidx955.i.i.i, align 4
  %add1130.i.i.i = add nsw i32 %ii870.i.i.0.i294, 12
  %and1131.i.i.i = and i32 %add1130.i.i.i, 15
  %arrayidx1133.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1131.i.i.i
  %121 = load i32* %arrayidx1133.i.i.i, align 4
  %xor1134.i.i.i = xor i32 %121, %120
  %add1136.i.i.i = add nsw i32 %ii870.i.i.0.i294, 6
  %and1137.i.i.i = and i32 %add1136.i.i.i, 15
  %arrayidx1139.i.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1137.i.i.i
  %122 = load i32* %arrayidx1139.i.i.i, align 4
  %xor1140.i.i.i = xor i32 %xor1134.i.i.i, %122
  %123 = load i32* %arrayidx1013.i.i.i, align 4
  %xor1145.i.i.i = xor i32 %xor1140.i.i.i, %123
  %shl1146.i.i.i = shl i32 %xor1145.i.i.i, 1
  %shr1169.i.i.i = lshr i32 %xor1145.i.i.i, 31
  %or1170.i.i.i = or i32 %shl1146.i.i.i, %shr1169.i.i.i
  store i32 %or1170.i.i.i, i32* %arrayidx1013.i.i.i, align 4
  %add1122.i.i.i = add i32 %or990.i.i.i, -899497514
  %add1123.i.i.i = add i32 %add1122.i.i.i, %xor1121.i.i.i
  %add1175.i.i.i = add i32 %add1123.i.i.i, %or1119.i.i.i
  %add1176.i.i.i = add i32 %add1175.i.i.i, %or1170.i.i.i
  %shr1177.i.i.i = lshr i32 %add1050.i.i.i, 2
  %shl1178.i.i.i = shl i32 %add1050.i.i.i, 30
  %or1179.i.i.i = or i32 %shr1177.i.i.i, %shl1178.i.i.i
  %cmp872.i.i.i = icmp slt i32 %add1073.i.i.i, 80
  br i1 %cmp872.i.i.i, label %for.body873.i.i.i, label %while.cond48.i.i.preheader.thread

while.cond48.i.i.preheader.thread:                ; preds = %for.body873.i.i.i
  %124 = load i32* %arrayidx3.i.i, align 4
  %add1185.i.i.i = add i32 %124, %add1176.i.i.i
  store i32 %add1185.i.i.i, i32* %arrayidx3.i.i, align 4
  %125 = load i32* %arrayidx5.i.i, align 4
  %add1188.i.i.i = add i32 %125, %add1113.i.i.i
  store i32 %add1188.i.i.i, i32* %arrayidx5.i.i, align 4
  %126 = load i32* %arrayidx7.i.i, align 4
  %add1191.i.i.i = add i32 %126, %or1179.i.i.i
  store i32 %add1191.i.i.i, i32* %arrayidx7.i.i, align 4
  %127 = load i32* %arrayidx9.i.i, align 4
  %add1194.i.i.i = add i32 %127, %or1116.i.i.i
  store i32 %add1194.i.i.i, i32* %arrayidx9.i.i, align 4
  %128 = load i32* %arrayidx11.i.i, align 4
  %add1197.i.i.i = add i32 %128, %or1053.i.i.i
  store i32 %add1197.i.i.i, i32* %arrayidx11.i.i, align 4
  br label %while.body50.i.i

while.cond48.i.i.preheader:                       ; preds = %while.end.i.i
  %add46.i.i = add i32 %shr27.i.i, 1
  %cmp49.i.i287 = icmp ult i32 %add46.i.i, 14
  br i1 %cmp49.i.i287, label %while.body50.i.i, label %while.end54.i.i

while.body50.i.i:                                 ; preds = %while.body50.i.i, %while.cond48.i.i.preheader, %while.cond48.i.i.preheader.thread
  %i.i.0.i288 = phi i32 [ %inc.i.i, %while.body50.i.i ], [ %add46.i.i, %while.cond48.i.i.preheader ], [ 0, %while.cond48.i.i.preheader.thread ]
  %inc.i.i = add i32 %i.i.0.i288, 1
  %arrayidx53.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %i.i.0.i288
  store i32 0, i32* %arrayidx53.i.i, align 4
  %exitcond425 = icmp eq i32 %inc.i.i, 14
  br i1 %exitcond425, label %while.end54.i.i, label %while.body50.i.i

while.end54.i.i:                                  ; preds = %while.body50.i.i, %while.cond48.i.i.preheader
  %129 = load i32* %arrayidx.i.i, align 4
  %shl57.i.i = shl i32 %129, 3
  %130 = load i32* %arrayidx2.i.i, align 4
  %shr60.i.i = lshr i32 %130, 29
  %or61.i.i = or i32 %shr60.i.i, %shl57.i.i
  %arrayidx63.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 14
  store i32 %or61.i.i, i32* %arrayidx63.i.i, align 4
  %shl66.i.i = shl i32 %130, 3
  %arrayidx68.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 15
  store i32 %shl66.i.i, i32* %arrayidx68.i.i, align 4
  %131 = load i32* %arrayidx3.i.i, align 4
  %132 = load i32* %arrayidx5.i.i, align 4
  %133 = load i32* %arrayidx7.i.i, align 4
  %134 = load i32* %arrayidx9.i.i, align 4
  %135 = load i32* %arrayidx11.i.i, align 4
  %shr.i105.i.i271 = lshr i32 %131, 27
  %shl.i106.i.i272 = shl i32 %131, 5
  %or.i107.i.i273 = or i32 %shr.i105.i.i271, %shl.i106.i.i272
  %and.i108.i.i274 = and i32 %133, %132
  %neg.i109.i.i275 = xor i32 %132, -1
  %and9.i110.i.i276 = and i32 %134, %neg.i109.i.i275
  %xor.i111.i.i277 = xor i32 %and9.i110.i.i276, %and.i108.i.i274
  %add.i112.i.i278 = add i32 %or.i107.i.i273, 1518500249
  %add10.i113.i.i279 = add i32 %add.i112.i.i278, %xor.i111.i.i277
  br label %for.body.i189.i.i

for.body.i189.i.i:                                ; preds = %for.body.i189.i.i, %while.end54.i.i
  %add10.i113.i.i286 = phi i32 [ %add10.i113.i.i279, %while.end54.i.i ], [ %add10.i113.i.i, %for.body.i189.i.i ]
  %ii.i87.i.0.i285 = phi i32 [ 0, %while.end54.i.i ], [ %add85.i190.i.i, %for.body.i189.i.i ]
  %v4.i86.i.0.i284 = phi i32 [ %135, %while.end54.i.i ], [ %or50.i154.i.i, %for.body.i189.i.i ]
  %v3.i85.i.0.i283 = phi i32 [ %134, %while.end54.i.i ], [ %or67.i171.i.i, %for.body.i189.i.i ]
  %v2.i84.i.0.i282 = phi i32 [ %133, %while.end54.i.i ], [ %or84.i188.i.i, %for.body.i189.i.i ]
  %v1.i83.i.0.i281 = phi i32 [ %132, %while.end54.i.i ], [ %add64.i168.i.i, %for.body.i189.i.i ]
  %v0.i82.i.0.i280 = phi i32 [ %131, %while.end54.i.i ], [ %add81.i185.i.i, %for.body.i189.i.i ]
  %arrayidx11.i115.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %ii.i87.i.0.i285
  %136 = load i32* %arrayidx11.i115.i.i, align 4
  %add12.i116.i.i = add i32 %v4.i86.i.0.i284, %add10.i113.i.i286
  %add13.i117.i.i = add i32 %add12.i116.i.i, %136
  %shr14.i118.i.i = lshr i32 %v1.i83.i.0.i281, 2
  %shl15.i119.i.i = shl i32 %v1.i83.i.0.i281, 30
  %or16.i120.i.i = or i32 %shr14.i118.i.i, %shl15.i119.i.i
  %shr17.i121.i.i = lshr i32 %add13.i117.i.i, 27
  %shl18.i122.i.i = shl i32 %add13.i117.i.i, 5
  %or19.i123.i.i = or i32 %shr17.i121.i.i, %shl18.i122.i.i
  %and20.i124.i.i = and i32 %or16.i120.i.i, %v0.i82.i.0.i280
  %neg21.i125.i.i = xor i32 %v0.i82.i.0.i280, -1
  %and22.i126.i.i = and i32 %v2.i84.i.0.i282, %neg21.i125.i.i
  %xor23.i127.i.i = xor i32 %and20.i124.i.i, %and22.i126.i.i
  %add26.i130.i.i = add nsw i32 %ii.i87.i.0.i285, 1
  %arrayidx28.i132.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add26.i130.i.i
  %137 = load i32* %arrayidx28.i132.i.i, align 4
  %add24.i128.i.i = add i32 %v3.i85.i.0.i283, 1518500249
  %add25.i129.i.i = add i32 %add24.i128.i.i, %137
  %add29.i133.i.i = add i32 %add25.i129.i.i, %xor23.i127.i.i
  %add30.i134.i.i = add i32 %add29.i133.i.i, %or19.i123.i.i
  %shr31.i135.i.i = lshr i32 %v0.i82.i.0.i280, 2
  %shl32.i136.i.i = shl i32 %v0.i82.i.0.i280, 30
  %or33.i137.i.i = or i32 %shr31.i135.i.i, %shl32.i136.i.i
  %shr34.i138.i.i = lshr i32 %add30.i134.i.i, 27
  %shl35.i139.i.i = shl i32 %add30.i134.i.i, 5
  %or36.i140.i.i = or i32 %shr34.i138.i.i, %shl35.i139.i.i
  %and37.i141.i.i = and i32 %add13.i117.i.i, %or33.i137.i.i
  %neg38.i142.i.i = xor i32 %add13.i117.i.i, -1
  %and39.i143.i.i = and i32 %or16.i120.i.i, %neg38.i142.i.i
  %xor40.i144.i.i = xor i32 %and37.i141.i.i, %and39.i143.i.i
  %add43.i147.i.i = add nsw i32 %ii.i87.i.0.i285, 2
  %arrayidx45.i149.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add43.i147.i.i
  %138 = load i32* %arrayidx45.i149.i.i, align 4
  %add41.i145.i.i = add i32 %v2.i84.i.0.i282, 1518500249
  %add42.i146.i.i = add i32 %add41.i145.i.i, %138
  %add46.i150.i.i = add i32 %add42.i146.i.i, %xor40.i144.i.i
  %add47.i151.i.i = add i32 %add46.i150.i.i, %or36.i140.i.i
  %shr48.i152.i.i = lshr i32 %add13.i117.i.i, 2
  %shl49.i153.i.i = shl i32 %add13.i117.i.i, 30
  %or50.i154.i.i = or i32 %shr48.i152.i.i, %shl49.i153.i.i
  %shr51.i155.i.i = lshr i32 %add47.i151.i.i, 27
  %shl52.i156.i.i = shl i32 %add47.i151.i.i, 5
  %or53.i157.i.i = or i32 %shr51.i155.i.i, %shl52.i156.i.i
  %and54.i158.i.i = and i32 %add30.i134.i.i, %or50.i154.i.i
  %neg55.i159.i.i = xor i32 %add30.i134.i.i, -1
  %and56.i160.i.i = and i32 %or33.i137.i.i, %neg55.i159.i.i
  %xor57.i161.i.i = xor i32 %and54.i158.i.i, %and56.i160.i.i
  %add60.i164.i.i = add nsw i32 %ii.i87.i.0.i285, 3
  %arrayidx62.i166.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add60.i164.i.i
  %139 = load i32* %arrayidx62.i166.i.i, align 4
  %add58.i162.i.i = add i32 %or16.i120.i.i, 1518500249
  %add59.i163.i.i = add i32 %add58.i162.i.i, %139
  %add63.i167.i.i = add i32 %add59.i163.i.i, %xor57.i161.i.i
  %add64.i168.i.i = add i32 %add63.i167.i.i, %or53.i157.i.i
  %shr65.i169.i.i = lshr i32 %add30.i134.i.i, 2
  %shl66.i170.i.i = shl i32 %add30.i134.i.i, 30
  %or67.i171.i.i = or i32 %shr65.i169.i.i, %shl66.i170.i.i
  %shr68.i172.i.i = lshr i32 %add64.i168.i.i, 27
  %shl69.i173.i.i = shl i32 %add64.i168.i.i, 5
  %or70.i174.i.i = or i32 %shr68.i172.i.i, %shl69.i173.i.i
  %and71.i175.i.i = and i32 %add47.i151.i.i, %or67.i171.i.i
  %neg72.i176.i.i = xor i32 %add47.i151.i.i, -1
  %and73.i177.i.i = and i32 %or50.i154.i.i, %neg72.i176.i.i
  %xor74.i178.i.i = xor i32 %and71.i175.i.i, %and73.i177.i.i
  %add77.i181.i.i = add nsw i32 %ii.i87.i.0.i285, 4
  %arrayidx79.i183.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %add77.i181.i.i
  %140 = load i32* %arrayidx79.i183.i.i, align 4
  %add75.i179.i.i = add i32 %or33.i137.i.i, 1518500249
  %add76.i180.i.i = add i32 %add75.i179.i.i, %140
  %add80.i184.i.i = add i32 %add76.i180.i.i, %xor74.i178.i.i
  %add81.i185.i.i = add i32 %add80.i184.i.i, %or70.i174.i.i
  %shr82.i186.i.i = lshr i32 %add47.i151.i.i, 2
  %shl83.i187.i.i = shl i32 %add47.i151.i.i, 30
  %or84.i188.i.i = or i32 %shr82.i186.i.i, %shl83.i187.i.i
  %add85.i190.i.i = add nsw i32 %ii.i87.i.0.i285, 5
  %cmp.i103.i.i = icmp slt i32 %add85.i190.i.i, 15
  %shr.i105.i.i = lshr i32 %add81.i185.i.i, 27
  %shl.i106.i.i = shl i32 %add81.i185.i.i, 5
  %or.i107.i.i = or i32 %shr.i105.i.i, %shl.i106.i.i
  %and.i108.i.i = and i32 %add64.i168.i.i, %or84.i188.i.i
  %neg.i109.i.i = xor i32 %add64.i168.i.i, -1
  %and9.i110.i.i = and i32 %or67.i171.i.i, %neg.i109.i.i
  %xor.i111.i.i = xor i32 %and9.i110.i.i, %and.i108.i.i
  %add.i112.i.i = add i32 %xor.i111.i.i, 1518500249
  %add10.i113.i.i = add i32 %add.i112.i.i, %or.i107.i.i
  br i1 %cmp.i103.i.i, label %for.body.i189.i.i, label %for.end.i329.i.i

for.end.i329.i.i:                                 ; preds = %for.body.i189.i.i
  %arraydecay.i92.i.i = getelementptr inbounds [16 x i32]* %wbuf29.i.i, i32 0, i32 0
  %add96.i201.i.i = add i32 %or50.i154.i.i, %shl66.i.i
  %add97.i202.i.i = add i32 %add96.i201.i.i, %add10.i113.i.i
  %shr98.i203.i.i = lshr i32 %add64.i168.i.i, 2
  %shl99.i204.i.i = shl i32 %add64.i168.i.i, 30
  %or100.i205.i.i = or i32 %shr98.i203.i.i, %shl99.i204.i.i
  %shr101.i206.i.i = lshr i32 %add97.i202.i.i, 27
  %shl102.i207.i.i = shl i32 %add97.i202.i.i, 5
  %or103.i208.i.i = or i32 %shr101.i206.i.i, %shl102.i207.i.i
  %and104.i209.i.i = and i32 %add81.i185.i.i, %or100.i205.i.i
  %neg105.i210.i.i = xor i32 %add81.i185.i.i, -1
  %and106.i211.i.i = and i32 %or84.i188.i.i, %neg105.i210.i.i
  %xor107.i212.i.i = xor i32 %and104.i209.i.i, %and106.i211.i.i
  %arrayidx110.i215.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 13
  %141 = load i32* %arrayidx110.i215.i.i, align 4
  %arrayidx111.i216.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 8
  %142 = load i32* %arrayidx111.i216.i.i, align 4
  %xor112.i217.i.i = xor i32 %142, %141
  %arrayidx113.i218.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 2
  %143 = load i32* %arrayidx113.i218.i.i, align 4
  %xor114.i219.i.i = xor i32 %xor112.i217.i.i, %143
  %144 = load i32* %arraydecay.i92.i.i, align 4
  %xor116.i220.i.i = xor i32 %xor114.i219.i.i, %144
  %shl117.i221.i.i = shl i32 %xor116.i220.i.i, 1
  %shr125.i228.i.i = lshr i32 %xor116.i220.i.i, 31
  %or126.i229.i.i = or i32 %shl117.i221.i.i, %shr125.i228.i.i
  store i32 %or126.i229.i.i, i32* %arraydecay.i92.i.i, align 4
  %add108.i213.i.i = add i32 %or67.i171.i.i, 1518500249
  %add109.i214.i.i = add i32 %add108.i213.i.i, %xor107.i212.i.i
  %add128.i230.i.i = add i32 %add109.i214.i.i, %or103.i208.i.i
  %add129.i231.i.i = add i32 %add128.i230.i.i, %or126.i229.i.i
  %shr130.i232.i.i = lshr i32 %add81.i185.i.i, 2
  %shl131.i233.i.i = shl i32 %add81.i185.i.i, 30
  %or132.i234.i.i = or i32 %shr130.i232.i.i, %shl131.i233.i.i
  %shr133.i235.i.i = lshr i32 %add129.i231.i.i, 27
  %shl134.i236.i.i = shl i32 %add129.i231.i.i, 5
  %or135.i237.i.i = or i32 %shr133.i235.i.i, %shl134.i236.i.i
  %and136.i238.i.i = and i32 %add97.i202.i.i, %or132.i234.i.i
  %neg137.i239.i.i = xor i32 %add97.i202.i.i, -1
  %and138.i240.i.i = and i32 %or100.i205.i.i, %neg137.i239.i.i
  %xor139.i241.i.i = xor i32 %and136.i238.i.i, %and138.i240.i.i
  %arrayidx143.i245.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 9
  %145 = load i32* %arrayidx143.i245.i.i, align 4
  %xor144.i246.i.i = xor i32 %145, %or61.i.i
  %arrayidx145.i247.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 3
  %146 = load i32* %arrayidx145.i247.i.i, align 4
  %xor146.i248.i.i = xor i32 %xor144.i246.i.i, %146
  %arrayidx147.i249.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 1
  %147 = load i32* %arrayidx147.i249.i.i, align 4
  %xor148.i250.i.i = xor i32 %xor146.i248.i.i, %147
  %shl149.i251.i.i = shl i32 %xor148.i250.i.i, 1
  %shr157.i259.i.i = lshr i32 %xor148.i250.i.i, 31
  %or158.i260.i.i = or i32 %shl149.i251.i.i, %shr157.i259.i.i
  store i32 %or158.i260.i.i, i32* %arrayidx147.i249.i.i, align 4
  %add140.i242.i.i = add i32 %or84.i188.i.i, 1518500249
  %add141.i243.i.i = add i32 %add140.i242.i.i, %xor139.i241.i.i
  %add160.i262.i.i = add i32 %add141.i243.i.i, %or135.i237.i.i
  %add161.i263.i.i = add i32 %add160.i262.i.i, %or158.i260.i.i
  %shr162.i264.i.i = lshr i32 %add97.i202.i.i, 2
  %shl163.i265.i.i = shl i32 %add97.i202.i.i, 30
  %or164.i266.i.i = or i32 %shr162.i264.i.i, %shl163.i265.i.i
  %shr165.i267.i.i = lshr i32 %add161.i263.i.i, 27
  %shl166.i268.i.i = shl i32 %add161.i263.i.i, 5
  %or167.i269.i.i = or i32 %shr165.i267.i.i, %shl166.i268.i.i
  %and168.i270.i.i = and i32 %add129.i231.i.i, %or164.i266.i.i
  %neg169.i271.i.i = xor i32 %add129.i231.i.i, -1
  %and170.i272.i.i = and i32 %or132.i234.i.i, %neg169.i271.i.i
  %xor171.i273.i.i = xor i32 %and168.i270.i.i, %and170.i272.i.i
  %arrayidx175.i277.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 10
  %148 = load i32* %arrayidx175.i277.i.i, align 4
  %arrayidx177.i279.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 4
  %149 = load i32* %arrayidx177.i279.i.i, align 4
  %xor176.i278.i.i = xor i32 %143, %shl66.i.i
  %xor178.i280.i.i = xor i32 %xor176.i278.i.i, %148
  %xor180.i282.i.i = xor i32 %xor178.i280.i.i, %149
  %shl181.i283.i.i = shl i32 %xor180.i282.i.i, 1
  %shr189.i291.i.i = lshr i32 %xor180.i282.i.i, 31
  %or190.i292.i.i = or i32 %shl181.i283.i.i, %shr189.i291.i.i
  store i32 %or190.i292.i.i, i32* %arrayidx113.i218.i.i, align 4
  %add172.i274.i.i = add i32 %or100.i205.i.i, 1518500249
  %add173.i275.i.i = add i32 %add172.i274.i.i, %xor171.i273.i.i
  %add192.i294.i.i = add i32 %add173.i275.i.i, %or190.i292.i.i
  %add193.i295.i.i = add i32 %add192.i294.i.i, %or167.i269.i.i
  %shr194.i296.i.i = lshr i32 %add129.i231.i.i, 2
  %shl195.i297.i.i = shl i32 %add129.i231.i.i, 30
  %or196.i298.i.i = or i32 %shr194.i296.i.i, %shl195.i297.i.i
  %shr197.i299.i.i = lshr i32 %add193.i295.i.i, 27
  %shl198.i300.i.i = shl i32 %add193.i295.i.i, 5
  %or199.i301.i.i = or i32 %shr197.i299.i.i, %shl198.i300.i.i
  %and200.i302.i.i = and i32 %add161.i263.i.i, %or196.i298.i.i
  %neg201.i303.i.i = xor i32 %add161.i263.i.i, -1
  %and202.i304.i.i = and i32 %or164.i266.i.i, %neg201.i303.i.i
  %xor203.i305.i.i = xor i32 %and200.i302.i.i, %and202.i304.i.i
  %arrayidx207.i308.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 11
  %150 = load i32* %arrayidx207.i308.i.i, align 4
  %arrayidx209.i310.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 5
  %151 = load i32* %arrayidx209.i310.i.i, align 4
  %xor208.i309.i.i = xor i32 %or126.i229.i.i, %146
  %xor210.i311.i.i = xor i32 %xor208.i309.i.i, %150
  %xor212.i313.i.i = xor i32 %xor210.i311.i.i, %151
  %shl213.i314.i.i = shl i32 %xor212.i313.i.i, 1
  %shr221.i321.i.i = lshr i32 %xor212.i313.i.i, 31
  %or222.i322.i.i = or i32 %shl213.i314.i.i, %shr221.i321.i.i
  store i32 %or222.i322.i.i, i32* %arrayidx145.i247.i.i, align 4
  %add204.i306.i.i = add i32 %or132.i234.i.i, 1518500249
  %add205.i307.i.i = add i32 %add204.i306.i.i, %xor203.i305.i.i
  %add224.i324.i.i = add i32 %add205.i307.i.i, %or222.i322.i.i
  %add225.i325.i.i = add i32 %add224.i324.i.i, %or199.i301.i.i
  %shr226.i326.i.i = lshr i32 %add161.i263.i.i, 2
  %shl227.i327.i.i = shl i32 %add161.i263.i.i, 30
  %or228.i328.i.i = or i32 %shr226.i326.i.i, %shl227.i327.i.i
  br label %for.body232.i638.i.i

for.body232.i638.i.i:                             ; preds = %for.body232.i638.i.i, %for.end.i329.i.i
  %ii229.i88.i.0.i270 = phi i32 [ 20, %for.end.i329.i.i ], [ %add432.i531.i.i, %for.body232.i638.i.i ]
  %v4.i86.i.1.i269 = phi i32 [ %or164.i266.i.i, %for.end.i329.i.i ], [ %or412.i511.i.i, %for.body232.i638.i.i ]
  %v3.i85.i.1.i268 = phi i32 [ %or196.i298.i.i, %for.end.i329.i.i ], [ %or475.i574.i.i, %for.body232.i638.i.i ]
  %v2.i84.i.1.i267 = phi i32 [ %or228.i328.i.i, %for.end.i329.i.i ], [ %or538.i637.i.i, %for.body232.i638.i.i ]
  %v1.i83.i.1.i266 = phi i32 [ %add193.i295.i.i, %for.end.i329.i.i ], [ %add472.i571.i.i, %for.body232.i638.i.i ]
  %v0.i82.i.1.i265 = phi i32 [ %add225.i325.i.i, %for.end.i329.i.i ], [ %add535.i634.i.i, %for.body232.i638.i.i ]
  %shr233.i332.i.i = lshr i32 %v0.i82.i.1.i265, 27
  %shl234.i333.i.i = shl i32 %v0.i82.i.1.i265, 5
  %or235.i334.i.i = or i32 %shr233.i332.i.i, %shl234.i333.i.i
  %xor236.i335.i.i = xor i32 %v2.i84.i.1.i267, %v3.i85.i.1.i268
  %xor237.i336.i.i = xor i32 %xor236.i335.i.i, %v1.i83.i.1.i266
  %add240.i339.i.i = add nsw i32 %ii229.i88.i.0.i270, 13
  %and241.i340.i.i = and i32 %add240.i339.i.i, 15
  %arrayidx243.i342.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and241.i340.i.i
  %152 = load i32* %arrayidx243.i342.i.i, align 4
  %add244.i343.i.i = add nsw i32 %ii229.i88.i.0.i270, 8
  %and245.i344.i.i = and i32 %add244.i343.i.i, 15
  %arrayidx247.i346.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and245.i344.i.i
  %153 = load i32* %arrayidx247.i346.i.i, align 4
  %xor248.i347.i.i = xor i32 %153, %152
  %add249.i348.i.i = add nsw i32 %ii229.i88.i.0.i270, 2
  %and250.i349.i.i = and i32 %add249.i348.i.i, 15
  %arrayidx252.i351.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and250.i349.i.i
  %154 = load i32* %arrayidx252.i351.i.i, align 4
  %xor253.i352.i.i = xor i32 %xor248.i347.i.i, %154
  %and254.i353.i.i = and i32 %ii229.i88.i.0.i270, 15
  %arrayidx256.i355.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and254.i353.i.i
  %155 = load i32* %arrayidx256.i355.i.i, align 4
  %xor257.i356.i.i = xor i32 %xor253.i352.i.i, %155
  %shl258.i357.i.i = shl i32 %xor257.i356.i.i, 1
  %shr277.i376.i.i = lshr i32 %xor257.i356.i.i, 31
  %or278.i377.i.i = or i32 %shl258.i357.i.i, %shr277.i376.i.i
  store i32 %or278.i377.i.i, i32* %arrayidx256.i355.i.i, align 4
  %add238.i337.i.i = add i32 %v4.i86.i.1.i269, 1859775393
  %add239.i338.i.i = add i32 %add238.i337.i.i, %xor237.i336.i.i
  %add282.i381.i.i = add i32 %add239.i338.i.i, %or235.i334.i.i
  %add283.i382.i.i = add i32 %add282.i381.i.i, %or278.i377.i.i
  %shr284.i383.i.i = lshr i32 %v1.i83.i.1.i266, 2
  %shl285.i384.i.i = shl i32 %v1.i83.i.1.i266, 30
  %or286.i385.i.i = or i32 %shr284.i383.i.i, %shl285.i384.i.i
  %shr287.i386.i.i = lshr i32 %add283.i382.i.i, 27
  %shl288.i387.i.i = shl i32 %add283.i382.i.i, 5
  %or289.i388.i.i = or i32 %shr287.i386.i.i, %shl288.i387.i.i
  %xor290.i389.i.i = xor i32 %v0.i82.i.1.i265, %v2.i84.i.1.i267
  %xor291.i390.i.i = xor i32 %xor290.i389.i.i, %or286.i385.i.i
  %add294.i393.i.i = add nsw i32 %ii229.i88.i.0.i270, 1
  %add295.i394.i.i = add nsw i32 %ii229.i88.i.0.i270, 14
  %and296.i395.i.i = and i32 %add295.i394.i.i, 15
  %arrayidx298.i397.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and296.i395.i.i
  %156 = load i32* %arrayidx298.i397.i.i, align 4
  %add300.i399.i.i = add nsw i32 %ii229.i88.i.0.i270, 9
  %and301.i400.i.i = and i32 %add300.i399.i.i, 15
  %arrayidx303.i402.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and301.i400.i.i
  %157 = load i32* %arrayidx303.i402.i.i, align 4
  %xor304.i403.i.i = xor i32 %157, %156
  %add306.i405.i.i = add nsw i32 %ii229.i88.i.0.i270, 3
  %and307.i406.i.i = and i32 %add306.i405.i.i, 15
  %arrayidx309.i408.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and307.i406.i.i
  %158 = load i32* %arrayidx309.i408.i.i, align 4
  %xor310.i409.i.i = xor i32 %xor304.i403.i.i, %158
  %and312.i411.i.i = and i32 %add294.i393.i.i, 15
  %arrayidx314.i413.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and312.i411.i.i
  %159 = load i32* %arrayidx314.i413.i.i, align 4
  %xor315.i414.i.i = xor i32 %xor310.i409.i.i, %159
  %shl316.i415.i.i = shl i32 %xor315.i414.i.i, 1
  %shr339.i438.i.i = lshr i32 %xor315.i414.i.i, 31
  %or340.i439.i.i = or i32 %shl316.i415.i.i, %shr339.i438.i.i
  store i32 %or340.i439.i.i, i32* %arrayidx314.i413.i.i, align 4
  %add292.i391.i.i = add i32 %v3.i85.i.1.i268, 1859775393
  %add293.i392.i.i = add i32 %add292.i391.i.i, %xor291.i390.i.i
  %add345.i444.i.i = add i32 %add293.i392.i.i, %or289.i388.i.i
  %add346.i445.i.i = add i32 %add345.i444.i.i, %or340.i439.i.i
  %shr347.i446.i.i = lshr i32 %v0.i82.i.1.i265, 2
  %shl348.i447.i.i = shl i32 %v0.i82.i.1.i265, 30
  %or349.i448.i.i = or i32 %shr347.i446.i.i, %shl348.i447.i.i
  %shr350.i449.i.i = lshr i32 %add346.i445.i.i, 27
  %shl351.i450.i.i = shl i32 %add346.i445.i.i, 5
  %or352.i451.i.i = or i32 %shr350.i449.i.i, %shl351.i450.i.i
  %xor353.i452.i.i = xor i32 %or349.i448.i.i, %or286.i385.i.i
  %xor354.i453.i.i = xor i32 %xor353.i452.i.i, %add283.i382.i.i
  %add358.i457.i.i = add nsw i32 %ii229.i88.i.0.i270, 15
  %and359.i458.i.i = and i32 %add358.i457.i.i, 15
  %arrayidx361.i460.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and359.i458.i.i
  %160 = load i32* %arrayidx361.i460.i.i, align 4
  %add363.i462.i.i = add nsw i32 %ii229.i88.i.0.i270, 10
  %and364.i463.i.i = and i32 %add363.i462.i.i, 15
  %arrayidx366.i465.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and364.i463.i.i
  %161 = load i32* %arrayidx366.i465.i.i, align 4
  %xor367.i466.i.i = xor i32 %161, %160
  %add369.i468.i.i = add nsw i32 %ii229.i88.i.0.i270, 4
  %and370.i469.i.i = and i32 %add369.i468.i.i, 15
  %arrayidx372.i471.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and370.i469.i.i
  %162 = load i32* %arrayidx372.i471.i.i, align 4
  %xor373.i472.i.i = xor i32 %xor367.i466.i.i, %162
  %163 = load i32* %arrayidx252.i351.i.i, align 4
  %xor378.i477.i.i = xor i32 %xor373.i472.i.i, %163
  %shl379.i478.i.i = shl i32 %xor378.i477.i.i, 1
  %shr402.i501.i.i = lshr i32 %xor378.i477.i.i, 31
  %or403.i502.i.i = or i32 %shl379.i478.i.i, %shr402.i501.i.i
  store i32 %or403.i502.i.i, i32* %arrayidx252.i351.i.i, align 4
  %add355.i454.i.i = add i32 %v2.i84.i.1.i267, 1859775393
  %add356.i455.i.i = add i32 %add355.i454.i.i, %xor354.i453.i.i
  %add408.i507.i.i = add i32 %add356.i455.i.i, %or352.i451.i.i
  %add409.i508.i.i = add i32 %add408.i507.i.i, %or403.i502.i.i
  %shr410.i509.i.i = lshr i32 %add283.i382.i.i, 2
  %shl411.i510.i.i = shl i32 %add283.i382.i.i, 30
  %or412.i511.i.i = or i32 %shr410.i509.i.i, %shl411.i510.i.i
  %shr413.i512.i.i = lshr i32 %add409.i508.i.i, 27
  %shl414.i513.i.i = shl i32 %add409.i508.i.i, 5
  %or415.i514.i.i = or i32 %shr413.i512.i.i, %shl414.i513.i.i
  %xor416.i515.i.i = xor i32 %or412.i511.i.i, %or349.i448.i.i
  %xor417.i516.i.i = xor i32 %xor416.i515.i.i, %add346.i445.i.i
  %164 = load i32* %arrayidx256.i355.i.i, align 4
  %add426.i525.i.i = add nsw i32 %ii229.i88.i.0.i270, 11
  %and427.i526.i.i = and i32 %add426.i525.i.i, 15
  %arrayidx429.i528.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and427.i526.i.i
  %165 = load i32* %arrayidx429.i528.i.i, align 4
  %xor430.i529.i.i = xor i32 %165, %164
  %add432.i531.i.i = add nsw i32 %ii229.i88.i.0.i270, 5
  %and433.i532.i.i = and i32 %add432.i531.i.i, 15
  %arrayidx435.i534.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and433.i532.i.i
  %166 = load i32* %arrayidx435.i534.i.i, align 4
  %xor436.i535.i.i = xor i32 %xor430.i529.i.i, %166
  %167 = load i32* %arrayidx309.i408.i.i, align 4
  %xor441.i540.i.i = xor i32 %xor436.i535.i.i, %167
  %shl442.i541.i.i = shl i32 %xor441.i540.i.i, 1
  %shr465.i564.i.i = lshr i32 %xor441.i540.i.i, 31
  %or466.i565.i.i = or i32 %shl442.i541.i.i, %shr465.i564.i.i
  store i32 %or466.i565.i.i, i32* %arrayidx309.i408.i.i, align 4
  %add418.i517.i.i = add i32 %or286.i385.i.i, 1859775393
  %add419.i518.i.i = add i32 %add418.i517.i.i, %xor417.i516.i.i
  %add471.i570.i.i = add i32 %add419.i518.i.i, %or415.i514.i.i
  %add472.i571.i.i = add i32 %add471.i570.i.i, %or466.i565.i.i
  %shr473.i572.i.i = lshr i32 %add346.i445.i.i, 2
  %shl474.i573.i.i = shl i32 %add346.i445.i.i, 30
  %or475.i574.i.i = or i32 %shr473.i572.i.i, %shl474.i573.i.i
  %shr476.i575.i.i = lshr i32 %add472.i571.i.i, 27
  %shl477.i576.i.i = shl i32 %add472.i571.i.i, 5
  %or478.i577.i.i = or i32 %shr476.i575.i.i, %shl477.i576.i.i
  %xor479.i578.i.i = xor i32 %or475.i574.i.i, %or412.i511.i.i
  %xor480.i579.i.i = xor i32 %xor479.i578.i.i, %add409.i508.i.i
  %168 = load i32* %arrayidx314.i413.i.i, align 4
  %add489.i588.i.i = add nsw i32 %ii229.i88.i.0.i270, 12
  %and490.i589.i.i = and i32 %add489.i588.i.i, 15
  %arrayidx492.i591.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and490.i589.i.i
  %169 = load i32* %arrayidx492.i591.i.i, align 4
  %xor493.i592.i.i = xor i32 %169, %168
  %add495.i594.i.i = add nsw i32 %ii229.i88.i.0.i270, 6
  %and496.i595.i.i = and i32 %add495.i594.i.i, 15
  %arrayidx498.i597.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and496.i595.i.i
  %170 = load i32* %arrayidx498.i597.i.i, align 4
  %xor499.i598.i.i = xor i32 %xor493.i592.i.i, %170
  %171 = load i32* %arrayidx372.i471.i.i, align 4
  %xor504.i603.i.i = xor i32 %xor499.i598.i.i, %171
  %shl505.i604.i.i = shl i32 %xor504.i603.i.i, 1
  %shr528.i627.i.i = lshr i32 %xor504.i603.i.i, 31
  %or529.i628.i.i = or i32 %shl505.i604.i.i, %shr528.i627.i.i
  store i32 %or529.i628.i.i, i32* %arrayidx372.i471.i.i, align 4
  %add481.i580.i.i = add i32 %or349.i448.i.i, 1859775393
  %add482.i581.i.i = add i32 %add481.i580.i.i, %xor480.i579.i.i
  %add534.i633.i.i = add i32 %add482.i581.i.i, %or478.i577.i.i
  %add535.i634.i.i = add i32 %add534.i633.i.i, %or529.i628.i.i
  %shr536.i635.i.i = lshr i32 %add409.i508.i.i, 2
  %shl537.i636.i.i = shl i32 %add409.i508.i.i, 30
  %or538.i637.i.i = or i32 %shr536.i635.i.i, %shl537.i636.i.i
  %cmp231.i330.i.i = icmp slt i32 %add432.i531.i.i, 40
  br i1 %cmp231.i330.i.i, label %for.body232.i638.i.i, label %for.body545.i964.i.i

for.body545.i964.i.i:                             ; preds = %for.body545.i964.i.i, %for.body232.i638.i.i
  %ii542.i89.i.0.i264 = phi i32 [ %add757.i854.i.i, %for.body545.i964.i.i ], [ 40, %for.body232.i638.i.i ]
  %v4.i86.i.2.i263 = phi i32 [ %or734.i831.i.i, %for.body545.i964.i.i ], [ %or412.i511.i.i, %for.body232.i638.i.i ]
  %v3.i85.i.2.i262 = phi i32 [ %or800.i897.i.i, %for.body545.i964.i.i ], [ %or475.i574.i.i, %for.body232.i638.i.i ]
  %v2.i84.i.2.i261 = phi i32 [ %or866.i963.i.i, %for.body545.i964.i.i ], [ %or538.i637.i.i, %for.body232.i638.i.i ]
  %v1.i83.i.2.i260 = phi i32 [ %add797.i894.i.i, %for.body545.i964.i.i ], [ %add472.i571.i.i, %for.body232.i638.i.i ]
  %v0.i82.i.2.i259 = phi i32 [ %add863.i960.i.i, %for.body545.i964.i.i ], [ %add535.i634.i.i, %for.body232.i638.i.i ]
  %shr546.i643.i.i = lshr i32 %v0.i82.i.2.i259, 27
  %shl547.i644.i.i = shl i32 %v0.i82.i.2.i259, 5
  %or548.i645.i.i = or i32 %shr546.i643.i.i, %shl547.i644.i.i
  %and550.i647.i.i23 = xor i32 %v2.i84.i.2.i261, %v3.i85.i.2.i262
  %xor551.i648.i.i = and i32 %and550.i647.i.i23, %v1.i83.i.2.i260
  %and552.i649.i.i = and i32 %v2.i84.i.2.i261, %v3.i85.i.2.i262
  %xor553.i650.i.i = xor i32 %xor551.i648.i.i, %and552.i649.i.i
  %add556.i653.i.i = add nsw i32 %ii542.i89.i.0.i264, 13
  %and557.i654.i.i = and i32 %add556.i653.i.i, 15
  %arrayidx559.i656.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and557.i654.i.i
  %172 = load i32* %arrayidx559.i656.i.i, align 4
  %add560.i657.i.i = add nsw i32 %ii542.i89.i.0.i264, 8
  %and561.i658.i.i = and i32 %add560.i657.i.i, 15
  %arrayidx563.i660.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and561.i658.i.i
  %173 = load i32* %arrayidx563.i660.i.i, align 4
  %xor564.i661.i.i = xor i32 %173, %172
  %add565.i662.i.i = add nsw i32 %ii542.i89.i.0.i264, 2
  %and566.i663.i.i = and i32 %add565.i662.i.i, 15
  %arrayidx568.i665.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and566.i663.i.i
  %174 = load i32* %arrayidx568.i665.i.i, align 4
  %xor569.i666.i.i = xor i32 %xor564.i661.i.i, %174
  %and570.i667.i.i = and i32 %ii542.i89.i.0.i264, 15
  %arrayidx572.i669.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and570.i667.i.i
  %175 = load i32* %arrayidx572.i669.i.i, align 4
  %xor573.i670.i.i = xor i32 %xor569.i666.i.i, %175
  %shl574.i671.i.i = shl i32 %xor573.i670.i.i, 1
  %shr593.i690.i.i = lshr i32 %xor573.i670.i.i, 31
  %or594.i691.i.i = or i32 %shl574.i671.i.i, %shr593.i690.i.i
  store i32 %or594.i691.i.i, i32* %arrayidx572.i669.i.i, align 4
  %add554.i651.i.i = add i32 %v4.i86.i.2.i263, -1894007588
  %add555.i652.i.i = add i32 %add554.i651.i.i, %xor553.i650.i.i
  %add598.i695.i.i = add i32 %add555.i652.i.i, %or548.i645.i.i
  %add599.i696.i.i = add i32 %add598.i695.i.i, %or594.i691.i.i
  %shr600.i697.i.i = lshr i32 %v1.i83.i.2.i260, 2
  %shl601.i698.i.i = shl i32 %v1.i83.i.2.i260, 30
  %or602.i699.i.i = or i32 %shr600.i697.i.i, %shl601.i698.i.i
  %shr603.i700.i.i = lshr i32 %add599.i696.i.i, 27
  %shl604.i701.i.i = shl i32 %add599.i696.i.i, 5
  %or605.i702.i.i = or i32 %shr603.i700.i.i, %shl604.i701.i.i
  %and607.i704.i.i28 = xor i32 %or602.i699.i.i, %v2.i84.i.2.i261
  %xor608.i705.i.i = and i32 %and607.i704.i.i28, %v0.i82.i.2.i259
  %and609.i706.i.i = and i32 %or602.i699.i.i, %v2.i84.i.2.i261
  %xor610.i707.i.i = xor i32 %xor608.i705.i.i, %and609.i706.i.i
  %add613.i710.i.i = add nsw i32 %ii542.i89.i.0.i264, 1
  %add614.i711.i.i = add nsw i32 %ii542.i89.i.0.i264, 14
  %and615.i712.i.i = and i32 %add614.i711.i.i, 15
  %arrayidx617.i714.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and615.i712.i.i
  %176 = load i32* %arrayidx617.i714.i.i, align 4
  %add619.i716.i.i = add nsw i32 %ii542.i89.i.0.i264, 9
  %and620.i717.i.i = and i32 %add619.i716.i.i, 15
  %arrayidx622.i719.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and620.i717.i.i
  %177 = load i32* %arrayidx622.i719.i.i, align 4
  %xor623.i720.i.i = xor i32 %177, %176
  %add625.i722.i.i = add nsw i32 %ii542.i89.i.0.i264, 3
  %and626.i723.i.i = and i32 %add625.i722.i.i, 15
  %arrayidx628.i725.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and626.i723.i.i
  %178 = load i32* %arrayidx628.i725.i.i, align 4
  %xor629.i726.i.i = xor i32 %xor623.i720.i.i, %178
  %and631.i728.i.i = and i32 %add613.i710.i.i, 15
  %arrayidx633.i730.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and631.i728.i.i
  %179 = load i32* %arrayidx633.i730.i.i, align 4
  %xor634.i731.i.i = xor i32 %xor629.i726.i.i, %179
  %shl635.i732.i.i = shl i32 %xor634.i731.i.i, 1
  %shr658.i755.i.i = lshr i32 %xor634.i731.i.i, 31
  %or659.i756.i.i = or i32 %shl635.i732.i.i, %shr658.i755.i.i
  store i32 %or659.i756.i.i, i32* %arrayidx633.i730.i.i, align 4
  %add611.i708.i.i = add i32 %v3.i85.i.2.i262, -1894007588
  %add612.i709.i.i = add i32 %add611.i708.i.i, %xor610.i707.i.i
  %add664.i761.i.i = add i32 %add612.i709.i.i, %or605.i702.i.i
  %add665.i762.i.i = add i32 %add664.i761.i.i, %or659.i756.i.i
  %shr666.i763.i.i = lshr i32 %v0.i82.i.2.i259, 2
  %shl667.i764.i.i = shl i32 %v0.i82.i.2.i259, 30
  %or668.i765.i.i = or i32 %shr666.i763.i.i, %shl667.i764.i.i
  %shr669.i766.i.i = lshr i32 %add665.i762.i.i, 27
  %shl670.i767.i.i = shl i32 %add665.i762.i.i, 5
  %or671.i768.i.i = or i32 %shr669.i766.i.i, %shl670.i767.i.i
  %and673.i770.i.i33 = xor i32 %or668.i765.i.i, %or602.i699.i.i
  %xor674.i771.i.i = and i32 %add599.i696.i.i, %and673.i770.i.i33
  %and675.i772.i.i = and i32 %or668.i765.i.i, %or602.i699.i.i
  %xor676.i773.i.i = xor i32 %xor674.i771.i.i, %and675.i772.i.i
  %add680.i777.i.i = add nsw i32 %ii542.i89.i.0.i264, 15
  %and681.i778.i.i = and i32 %add680.i777.i.i, 15
  %arrayidx683.i780.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and681.i778.i.i
  %180 = load i32* %arrayidx683.i780.i.i, align 4
  %add685.i782.i.i = add nsw i32 %ii542.i89.i.0.i264, 10
  %and686.i783.i.i = and i32 %add685.i782.i.i, 15
  %arrayidx688.i785.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and686.i783.i.i
  %181 = load i32* %arrayidx688.i785.i.i, align 4
  %xor689.i786.i.i = xor i32 %181, %180
  %add691.i788.i.i = add nsw i32 %ii542.i89.i.0.i264, 4
  %and692.i789.i.i = and i32 %add691.i788.i.i, 15
  %arrayidx694.i791.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and692.i789.i.i
  %182 = load i32* %arrayidx694.i791.i.i, align 4
  %xor695.i792.i.i = xor i32 %xor689.i786.i.i, %182
  %183 = load i32* %arrayidx568.i665.i.i, align 4
  %xor700.i797.i.i = xor i32 %xor695.i792.i.i, %183
  %shl701.i798.i.i = shl i32 %xor700.i797.i.i, 1
  %shr724.i821.i.i = lshr i32 %xor700.i797.i.i, 31
  %or725.i822.i.i = or i32 %shl701.i798.i.i, %shr724.i821.i.i
  store i32 %or725.i822.i.i, i32* %arrayidx568.i665.i.i, align 4
  %add677.i774.i.i = add i32 %v2.i84.i.2.i261, -1894007588
  %add678.i775.i.i = add i32 %add677.i774.i.i, %xor676.i773.i.i
  %add730.i827.i.i = add i32 %add678.i775.i.i, %or671.i768.i.i
  %add731.i828.i.i = add i32 %add730.i827.i.i, %or725.i822.i.i
  %shr732.i829.i.i = lshr i32 %add599.i696.i.i, 2
  %shl733.i830.i.i = shl i32 %add599.i696.i.i, 30
  %or734.i831.i.i = or i32 %shr732.i829.i.i, %shl733.i830.i.i
  %shr735.i832.i.i = lshr i32 %add731.i828.i.i, 27
  %shl736.i833.i.i = shl i32 %add731.i828.i.i, 5
  %or737.i834.i.i = or i32 %shr735.i832.i.i, %shl736.i833.i.i
  %and739.i836.i.i37 = xor i32 %or734.i831.i.i, %or668.i765.i.i
  %xor740.i837.i.i = and i32 %add665.i762.i.i, %and739.i836.i.i37
  %and741.i838.i.i = and i32 %or734.i831.i.i, %or668.i765.i.i
  %xor742.i839.i.i = xor i32 %xor740.i837.i.i, %and741.i838.i.i
  %184 = load i32* %arrayidx572.i669.i.i, align 4
  %add751.i848.i.i = add nsw i32 %ii542.i89.i.0.i264, 11
  %and752.i849.i.i = and i32 %add751.i848.i.i, 15
  %arrayidx754.i851.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and752.i849.i.i
  %185 = load i32* %arrayidx754.i851.i.i, align 4
  %xor755.i852.i.i = xor i32 %185, %184
  %add757.i854.i.i = add nsw i32 %ii542.i89.i.0.i264, 5
  %and758.i855.i.i = and i32 %add757.i854.i.i, 15
  %arrayidx760.i857.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and758.i855.i.i
  %186 = load i32* %arrayidx760.i857.i.i, align 4
  %xor761.i858.i.i = xor i32 %xor755.i852.i.i, %186
  %187 = load i32* %arrayidx628.i725.i.i, align 4
  %xor766.i863.i.i = xor i32 %xor761.i858.i.i, %187
  %shl767.i864.i.i = shl i32 %xor766.i863.i.i, 1
  %shr790.i887.i.i = lshr i32 %xor766.i863.i.i, 31
  %or791.i888.i.i = or i32 %shl767.i864.i.i, %shr790.i887.i.i
  store i32 %or791.i888.i.i, i32* %arrayidx628.i725.i.i, align 4
  %add743.i840.i.i = add i32 %or602.i699.i.i, -1894007588
  %add744.i841.i.i = add i32 %add743.i840.i.i, %xor742.i839.i.i
  %add796.i893.i.i = add i32 %add744.i841.i.i, %or737.i834.i.i
  %add797.i894.i.i = add i32 %add796.i893.i.i, %or791.i888.i.i
  %shr798.i895.i.i = lshr i32 %add665.i762.i.i, 2
  %shl799.i896.i.i = shl i32 %add665.i762.i.i, 30
  %or800.i897.i.i = or i32 %shr798.i895.i.i, %shl799.i896.i.i
  %shr801.i898.i.i = lshr i32 %add797.i894.i.i, 27
  %shl802.i899.i.i = shl i32 %add797.i894.i.i, 5
  %or803.i900.i.i = or i32 %shr801.i898.i.i, %shl802.i899.i.i
  %and805.i902.i.i42 = xor i32 %or800.i897.i.i, %or734.i831.i.i
  %xor806.i903.i.i = and i32 %add731.i828.i.i, %and805.i902.i.i42
  %and807.i904.i.i = and i32 %or800.i897.i.i, %or734.i831.i.i
  %xor808.i905.i.i = xor i32 %xor806.i903.i.i, %and807.i904.i.i
  %188 = load i32* %arrayidx633.i730.i.i, align 4
  %add817.i914.i.i = add nsw i32 %ii542.i89.i.0.i264, 12
  %and818.i915.i.i = and i32 %add817.i914.i.i, 15
  %arrayidx820.i917.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and818.i915.i.i
  %189 = load i32* %arrayidx820.i917.i.i, align 4
  %xor821.i918.i.i = xor i32 %189, %188
  %add823.i920.i.i = add nsw i32 %ii542.i89.i.0.i264, 6
  %and824.i921.i.i = and i32 %add823.i920.i.i, 15
  %arrayidx826.i923.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and824.i921.i.i
  %190 = load i32* %arrayidx826.i923.i.i, align 4
  %xor827.i924.i.i = xor i32 %xor821.i918.i.i, %190
  %191 = load i32* %arrayidx694.i791.i.i, align 4
  %xor832.i929.i.i = xor i32 %xor827.i924.i.i, %191
  %shl833.i930.i.i = shl i32 %xor832.i929.i.i, 1
  %shr856.i953.i.i = lshr i32 %xor832.i929.i.i, 31
  %or857.i954.i.i = or i32 %shl833.i930.i.i, %shr856.i953.i.i
  store i32 %or857.i954.i.i, i32* %arrayidx694.i791.i.i, align 4
  %add809.i906.i.i = add i32 %or668.i765.i.i, -1894007588
  %add810.i907.i.i = add i32 %add809.i906.i.i, %xor808.i905.i.i
  %add862.i959.i.i = add i32 %add810.i907.i.i, %or803.i900.i.i
  %add863.i960.i.i = add i32 %add862.i959.i.i, %or857.i954.i.i
  %shr864.i961.i.i = lshr i32 %add731.i828.i.i, 2
  %shl865.i962.i.i = shl i32 %add731.i828.i.i, 30
  %or866.i963.i.i = or i32 %shr864.i961.i.i, %shl865.i962.i.i
  %cmp544.i641.i.i = icmp slt i32 %add757.i854.i.i, 60
  br i1 %cmp544.i641.i.i, label %for.body545.i964.i.i, label %for.body873.i1275.i.i

for.body873.i1275.i.i:                            ; preds = %for.body873.i1275.i.i, %for.body545.i964.i.i
  %ii870.i90.i.0.i258 = phi i32 [ %add1073.i1168.i.i, %for.body873.i1275.i.i ], [ 60, %for.body545.i964.i.i ]
  %v4.i86.i.3.i257 = phi i32 [ %or1053.i1148.i.i, %for.body873.i1275.i.i ], [ %or734.i831.i.i, %for.body545.i964.i.i ]
  %v3.i85.i.3.i256 = phi i32 [ %or1116.i1211.i.i, %for.body873.i1275.i.i ], [ %or800.i897.i.i, %for.body545.i964.i.i ]
  %v2.i84.i.3.i255 = phi i32 [ %or1179.i1274.i.i, %for.body873.i1275.i.i ], [ %or866.i963.i.i, %for.body545.i964.i.i ]
  %v1.i83.i.3.i254 = phi i32 [ %add1113.i1208.i.i, %for.body873.i1275.i.i ], [ %add797.i894.i.i, %for.body545.i964.i.i ]
  %v0.i82.i.3.i253 = phi i32 [ %add1176.i1271.i.i, %for.body873.i1275.i.i ], [ %add863.i960.i.i, %for.body545.i964.i.i ]
  %shr874.i969.i.i = lshr i32 %v0.i82.i.3.i253, 27
  %shl875.i970.i.i = shl i32 %v0.i82.i.3.i253, 5
  %or876.i971.i.i = or i32 %shr874.i969.i.i, %shl875.i970.i.i
  %xor877.i972.i.i = xor i32 %v2.i84.i.3.i255, %v3.i85.i.3.i256
  %xor878.i973.i.i = xor i32 %xor877.i972.i.i, %v1.i83.i.3.i254
  %add881.i976.i.i = add nsw i32 %ii870.i90.i.0.i258, 13
  %and882.i977.i.i = and i32 %add881.i976.i.i, 15
  %arrayidx884.i979.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and882.i977.i.i
  %192 = load i32* %arrayidx884.i979.i.i, align 4
  %add885.i980.i.i = add nsw i32 %ii870.i90.i.0.i258, 8
  %and886.i981.i.i = and i32 %add885.i980.i.i, 15
  %arrayidx888.i983.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and886.i981.i.i
  %193 = load i32* %arrayidx888.i983.i.i, align 4
  %xor889.i984.i.i = xor i32 %193, %192
  %add890.i985.i.i = add nsw i32 %ii870.i90.i.0.i258, 2
  %and891.i986.i.i = and i32 %add890.i985.i.i, 15
  %arrayidx893.i988.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and891.i986.i.i
  %194 = load i32* %arrayidx893.i988.i.i, align 4
  %xor894.i989.i.i = xor i32 %xor889.i984.i.i, %194
  %and895.i990.i.i = and i32 %ii870.i90.i.0.i258, 15
  %arrayidx897.i992.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and895.i990.i.i
  %195 = load i32* %arrayidx897.i992.i.i, align 4
  %xor898.i993.i.i = xor i32 %xor894.i989.i.i, %195
  %shl899.i994.i.i = shl i32 %xor898.i993.i.i, 1
  %shr918.i1013.i.i = lshr i32 %xor898.i993.i.i, 31
  %or919.i1014.i.i = or i32 %shl899.i994.i.i, %shr918.i1013.i.i
  store i32 %or919.i1014.i.i, i32* %arrayidx897.i992.i.i, align 4
  %add879.i974.i.i = add i32 %v4.i86.i.3.i257, -899497514
  %add880.i975.i.i = add i32 %add879.i974.i.i, %xor878.i973.i.i
  %add923.i1018.i.i = add i32 %add880.i975.i.i, %or876.i971.i.i
  %add924.i1019.i.i = add i32 %add923.i1018.i.i, %or919.i1014.i.i
  %shr925.i1020.i.i = lshr i32 %v1.i83.i.3.i254, 2
  %shl926.i1021.i.i = shl i32 %v1.i83.i.3.i254, 30
  %or927.i1022.i.i = or i32 %shr925.i1020.i.i, %shl926.i1021.i.i
  %shr928.i1023.i.i = lshr i32 %add924.i1019.i.i, 27
  %shl929.i1024.i.i = shl i32 %add924.i1019.i.i, 5
  %or930.i1025.i.i = or i32 %shr928.i1023.i.i, %shl929.i1024.i.i
  %xor931.i1026.i.i = xor i32 %v0.i82.i.3.i253, %v2.i84.i.3.i255
  %xor932.i1027.i.i = xor i32 %xor931.i1026.i.i, %or927.i1022.i.i
  %add935.i1030.i.i = add nsw i32 %ii870.i90.i.0.i258, 1
  %add936.i1031.i.i = add nsw i32 %ii870.i90.i.0.i258, 14
  %and937.i1032.i.i = and i32 %add936.i1031.i.i, 15
  %arrayidx939.i1034.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and937.i1032.i.i
  %196 = load i32* %arrayidx939.i1034.i.i, align 4
  %add941.i1036.i.i = add nsw i32 %ii870.i90.i.0.i258, 9
  %and942.i1037.i.i = and i32 %add941.i1036.i.i, 15
  %arrayidx944.i1039.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and942.i1037.i.i
  %197 = load i32* %arrayidx944.i1039.i.i, align 4
  %xor945.i1040.i.i = xor i32 %197, %196
  %add947.i1042.i.i = add nsw i32 %ii870.i90.i.0.i258, 3
  %and948.i1043.i.i = and i32 %add947.i1042.i.i, 15
  %arrayidx950.i1045.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and948.i1043.i.i
  %198 = load i32* %arrayidx950.i1045.i.i, align 4
  %xor951.i1046.i.i = xor i32 %xor945.i1040.i.i, %198
  %and953.i1048.i.i = and i32 %add935.i1030.i.i, 15
  %arrayidx955.i1050.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and953.i1048.i.i
  %199 = load i32* %arrayidx955.i1050.i.i, align 4
  %xor956.i1051.i.i = xor i32 %xor951.i1046.i.i, %199
  %shl957.i1052.i.i = shl i32 %xor956.i1051.i.i, 1
  %shr980.i1075.i.i = lshr i32 %xor956.i1051.i.i, 31
  %or981.i1076.i.i = or i32 %shl957.i1052.i.i, %shr980.i1075.i.i
  store i32 %or981.i1076.i.i, i32* %arrayidx955.i1050.i.i, align 4
  %add933.i1028.i.i = add i32 %v3.i85.i.3.i256, -899497514
  %add934.i1029.i.i = add i32 %add933.i1028.i.i, %xor932.i1027.i.i
  %add986.i1081.i.i = add i32 %add934.i1029.i.i, %or930.i1025.i.i
  %add987.i1082.i.i = add i32 %add986.i1081.i.i, %or981.i1076.i.i
  %shr988.i1083.i.i = lshr i32 %v0.i82.i.3.i253, 2
  %shl989.i1084.i.i = shl i32 %v0.i82.i.3.i253, 30
  %or990.i1085.i.i = or i32 %shr988.i1083.i.i, %shl989.i1084.i.i
  %shr991.i1086.i.i = lshr i32 %add987.i1082.i.i, 27
  %shl992.i1087.i.i = shl i32 %add987.i1082.i.i, 5
  %or993.i1088.i.i = or i32 %shr991.i1086.i.i, %shl992.i1087.i.i
  %xor994.i1089.i.i = xor i32 %or990.i1085.i.i, %or927.i1022.i.i
  %xor995.i1090.i.i = xor i32 %xor994.i1089.i.i, %add924.i1019.i.i
  %add999.i1094.i.i = add nsw i32 %ii870.i90.i.0.i258, 15
  %and1000.i1095.i.i = and i32 %add999.i1094.i.i, 15
  %arrayidx1002.i1097.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1000.i1095.i.i
  %200 = load i32* %arrayidx1002.i1097.i.i, align 4
  %add1004.i1099.i.i = add nsw i32 %ii870.i90.i.0.i258, 10
  %and1005.i1100.i.i = and i32 %add1004.i1099.i.i, 15
  %arrayidx1007.i1102.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1005.i1100.i.i
  %201 = load i32* %arrayidx1007.i1102.i.i, align 4
  %xor1008.i1103.i.i = xor i32 %201, %200
  %add1010.i1105.i.i = add nsw i32 %ii870.i90.i.0.i258, 4
  %and1011.i1106.i.i = and i32 %add1010.i1105.i.i, 15
  %arrayidx1013.i1108.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1011.i1106.i.i
  %202 = load i32* %arrayidx1013.i1108.i.i, align 4
  %xor1014.i1109.i.i = xor i32 %xor1008.i1103.i.i, %202
  %203 = load i32* %arrayidx893.i988.i.i, align 4
  %xor1019.i1114.i.i = xor i32 %xor1014.i1109.i.i, %203
  %shl1020.i1115.i.i = shl i32 %xor1019.i1114.i.i, 1
  %shr1043.i1138.i.i = lshr i32 %xor1019.i1114.i.i, 31
  %or1044.i1139.i.i = or i32 %shl1020.i1115.i.i, %shr1043.i1138.i.i
  store i32 %or1044.i1139.i.i, i32* %arrayidx893.i988.i.i, align 4
  %add996.i1091.i.i = add i32 %v2.i84.i.3.i255, -899497514
  %add997.i1092.i.i = add i32 %add996.i1091.i.i, %xor995.i1090.i.i
  %add1049.i1144.i.i = add i32 %add997.i1092.i.i, %or993.i1088.i.i
  %add1050.i1145.i.i = add i32 %add1049.i1144.i.i, %or1044.i1139.i.i
  %shr1051.i1146.i.i = lshr i32 %add924.i1019.i.i, 2
  %shl1052.i1147.i.i = shl i32 %add924.i1019.i.i, 30
  %or1053.i1148.i.i = or i32 %shr1051.i1146.i.i, %shl1052.i1147.i.i
  %shr1054.i1149.i.i = lshr i32 %add1050.i1145.i.i, 27
  %shl1055.i1150.i.i = shl i32 %add1050.i1145.i.i, 5
  %or1056.i1151.i.i = or i32 %shr1054.i1149.i.i, %shl1055.i1150.i.i
  %xor1057.i1152.i.i = xor i32 %or1053.i1148.i.i, %or990.i1085.i.i
  %xor1058.i1153.i.i = xor i32 %xor1057.i1152.i.i, %add987.i1082.i.i
  %204 = load i32* %arrayidx897.i992.i.i, align 4
  %add1067.i1162.i.i = add nsw i32 %ii870.i90.i.0.i258, 11
  %and1068.i1163.i.i = and i32 %add1067.i1162.i.i, 15
  %arrayidx1070.i1165.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1068.i1163.i.i
  %205 = load i32* %arrayidx1070.i1165.i.i, align 4
  %xor1071.i1166.i.i = xor i32 %205, %204
  %add1073.i1168.i.i = add nsw i32 %ii870.i90.i.0.i258, 5
  %and1074.i1169.i.i = and i32 %add1073.i1168.i.i, 15
  %arrayidx1076.i1171.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1074.i1169.i.i
  %206 = load i32* %arrayidx1076.i1171.i.i, align 4
  %xor1077.i1172.i.i = xor i32 %xor1071.i1166.i.i, %206
  %207 = load i32* %arrayidx950.i1045.i.i, align 4
  %xor1082.i1177.i.i = xor i32 %xor1077.i1172.i.i, %207
  %shl1083.i1178.i.i = shl i32 %xor1082.i1177.i.i, 1
  %shr1106.i1201.i.i = lshr i32 %xor1082.i1177.i.i, 31
  %or1107.i1202.i.i = or i32 %shl1083.i1178.i.i, %shr1106.i1201.i.i
  store i32 %or1107.i1202.i.i, i32* %arrayidx950.i1045.i.i, align 4
  %add1059.i1154.i.i = add i32 %or927.i1022.i.i, -899497514
  %add1060.i1155.i.i = add i32 %add1059.i1154.i.i, %xor1058.i1153.i.i
  %add1112.i1207.i.i = add i32 %add1060.i1155.i.i, %or1056.i1151.i.i
  %add1113.i1208.i.i = add i32 %add1112.i1207.i.i, %or1107.i1202.i.i
  %shr1114.i1209.i.i = lshr i32 %add987.i1082.i.i, 2
  %shl1115.i1210.i.i = shl i32 %add987.i1082.i.i, 30
  %or1116.i1211.i.i = or i32 %shr1114.i1209.i.i, %shl1115.i1210.i.i
  %shr1117.i1212.i.i = lshr i32 %add1113.i1208.i.i, 27
  %shl1118.i1213.i.i = shl i32 %add1113.i1208.i.i, 5
  %or1119.i1214.i.i = or i32 %shr1117.i1212.i.i, %shl1118.i1213.i.i
  %xor1120.i1215.i.i = xor i32 %or1116.i1211.i.i, %or1053.i1148.i.i
  %xor1121.i1216.i.i = xor i32 %xor1120.i1215.i.i, %add1050.i1145.i.i
  %208 = load i32* %arrayidx955.i1050.i.i, align 4
  %add1130.i1225.i.i = add nsw i32 %ii870.i90.i.0.i258, 12
  %and1131.i1226.i.i = and i32 %add1130.i1225.i.i, 15
  %arrayidx1133.i1228.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1131.i1226.i.i
  %209 = load i32* %arrayidx1133.i1228.i.i, align 4
  %xor1134.i1229.i.i = xor i32 %209, %208
  %add1136.i1231.i.i = add nsw i32 %ii870.i90.i.0.i258, 6
  %and1137.i1232.i.i = and i32 %add1136.i1231.i.i, 15
  %arrayidx1139.i1234.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 2, i32 %and1137.i1232.i.i
  %210 = load i32* %arrayidx1139.i1234.i.i, align 4
  %xor1140.i1235.i.i = xor i32 %xor1134.i1229.i.i, %210
  %211 = load i32* %arrayidx1013.i1108.i.i, align 4
  %xor1145.i1240.i.i = xor i32 %xor1140.i1235.i.i, %211
  %shl1146.i1241.i.i = shl i32 %xor1145.i1240.i.i, 1
  %shr1169.i1264.i.i = lshr i32 %xor1145.i1240.i.i, 31
  %or1170.i1265.i.i = or i32 %shl1146.i1241.i.i, %shr1169.i1264.i.i
  store i32 %or1170.i1265.i.i, i32* %arrayidx1013.i1108.i.i, align 4
  %add1122.i1217.i.i = add i32 %or990.i1085.i.i, -899497514
  %add1123.i1218.i.i = add i32 %add1122.i1217.i.i, %xor1121.i1216.i.i
  %add1175.i1270.i.i = add i32 %add1123.i1218.i.i, %or1119.i1214.i.i
  %add1176.i1271.i.i = add i32 %add1175.i1270.i.i, %or1170.i1265.i.i
  %shr1177.i1272.i.i = lshr i32 %add1050.i1145.i.i, 2
  %shl1178.i1273.i.i = shl i32 %add1050.i1145.i.i, 30
  %or1179.i1274.i.i = or i32 %shr1177.i1272.i.i, %shl1178.i1273.i.i
  %cmp872.i967.i.i = icmp slt i32 %add1073.i1168.i.i, 80
  br i1 %cmp872.i967.i.i, label %for.body873.i1275.i.i, label %sha1_compile.exit1292.i.i

sha1_compile.exit1292.i.i:                        ; preds = %for.body873.i1275.i.i
  %212 = load i32* %arrayidx3.i.i, align 4
  %add1185.i1279.i.i = add i32 %212, %add1176.i1271.i.i
  store i32 %add1185.i1279.i.i, i32* %arrayidx3.i.i, align 4
  %213 = load i32* %arrayidx5.i.i, align 4
  %add1188.i1282.i.i = add i32 %213, %add1113.i1208.i.i
  store i32 %add1188.i1282.i.i, i32* %arrayidx5.i.i, align 4
  %214 = load i32* %arrayidx7.i.i, align 4
  %add1191.i1285.i.i = add i32 %214, %or1179.i1274.i.i
  store i32 %add1191.i1285.i.i, i32* %arrayidx7.i.i, align 4
  %215 = load i32* %arrayidx9.i.i, align 4
  %add1194.i1288.i.i = add i32 %215, %or1116.i1211.i.i
  store i32 %add1194.i1288.i.i, i32* %arrayidx9.i.i, align 4
  %216 = load i32* %arrayidx11.i.i, align 4
  %add1197.i1291.i.i = add i32 %216, %or1053.i1148.i.i
  store i32 %add1197.i1291.i.i, i32* %arrayidx11.i.i, align 4
  br label %for.body.i.i

for.body.i.i:                                     ; preds = %for.body.i.i, %sha1_compile.exit1292.i.i
  %i.i.1.i252 = phi i32 [ 0, %sha1_compile.exit1292.i.i ], [ %inc79.i.i, %for.body.i.i ]
  %shr70.i.i = lshr i32 %i.i.1.i252, 2
  %arrayidx72.i.i = getelementptr inbounds %struct.sha1_ctx_s* %ctx.i, i32 0, i32 1, i32 %shr70.i.i
  %217 = load i32* %arrayidx72.i.i, align 4
  %neg73.i.i = shl i32 %i.i.1.i252, 3
  %and74.i.i = and i32 %neg73.i.i, 24
  %mul75.i.i = xor i32 %and74.i.i, 24
  %shr76.i.i = lshr i32 %217, %mul75.i.i
  %conv.i.i = trunc i32 %shr76.i.i to i8
  %arrayidx78.i.i = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 %i.i.1.i252
  store i8 %conv.i.i, i8* %arrayidx78.i.i, align 1
  %inc79.i.i = add i32 %i.i.1.i252, 1
  %exitcond = icmp eq i32 %inc79.i.i, 20
  br i1 %exitcond, label %rng_spawn.exit, label %for.body.i.i

rng_spawn.exit:                                   ; preds = %for.body.i.i
  %diff = sub i32 %0, %1
  %218 = load i32* %9, align 4
  %219 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 0
  store i32 %218, i32* %219, align 8
  %220 = load i32* %13, align 4
  %221 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 1
  store i32 %220, i32* %221, align 4
  %222 = load i32* %14, align 4
  %223 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 2
  store i32 %222, i32* %223, align 8
  %224 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 0
  %225 = load i8* %224, align 1
  %226 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 0
  store i8 %225, i8* %226, align 4
  %227 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 1
  %228 = load i8* %227, align 1
  %229 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 1
  store i8 %228, i8* %229, align 1
  %230 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 2
  %231 = load i8* %230, align 1
  %232 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 2
  store i8 %231, i8* %232, align 2
  %233 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 3
  %234 = load i8* %233, align 1
  %235 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 3
  store i8 %234, i8* %235, align 1
  %236 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 4
  %237 = load i8* %236, align 1
  %238 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 4
  store i8 %237, i8* %238, align 4
  %239 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 5
  %240 = load i8* %239, align 1
  %241 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 5
  store i8 %240, i8* %241, align 1
  %242 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 6
  %243 = load i8* %242, align 1
  %244 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 6
  store i8 %243, i8* %244, align 2
  %245 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 7
  %246 = load i8* %245, align 1
  %247 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 7
  store i8 %246, i8* %247, align 1
  %248 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 8
  %249 = load i8* %248, align 1
  %250 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 8
  store i8 %249, i8* %250, align 4
  %251 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 9
  %252 = load i8* %251, align 1
  %253 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 9
  store i8 %252, i8* %253, align 1
  %254 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 10
  %255 = load i8* %254, align 1
  %256 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 10
  store i8 %255, i8* %256, align 2
  %257 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 11
  %258 = load i8* %257, align 1
  %259 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 11
  store i8 %258, i8* %259, align 1
  %260 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 12
  %261 = load i8* %260, align 1
  %262 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 12
  store i8 %261, i8* %262, align 4
  %263 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 13
  %264 = load i8* %263, align 1
  %265 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 13
  store i8 %264, i8* %265, align 1
  %266 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 14
  %267 = load i8* %266, align 1
  %268 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 14
  store i8 %267, i8* %268, align 2
  %269 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 15
  %270 = load i8* %269, align 1
  %271 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 15
  store i8 %270, i8* %271, align 1
  %272 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 16
  %273 = load i8* %272, align 1
  %274 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 16
  store i8 %273, i8* %274, align 4
  %275 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 17
  %276 = load i8* %275, align 1
  %277 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 17
  store i8 %276, i8* %277, align 1
  %278 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 18
  %279 = load i8* %278, align 1
  %280 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 18
  store i8 %279, i8* %280, align 2
  %281 = getelementptr inbounds %struct.node_t* %6, i32 0, i32 3, i32 0, i32 19
  %282 = load i8* %281, align 1
  %283 = getelementptr inbounds %struct.node_t* %8, i32 0, i32 3, i32 0, i32 19
  store i8 %282, i8* %283, align 1
  %falloc_reg = call i32 @llvm.falloc(i32 0)
  call void @llvm.fbind(i32 14, i32 %falloc_reg)
  %falloc_reg1 = call i32 @llvm.falloc(i32 0)
  call void @llvm.fbind(i32 6, i32 %falloc_reg1)
  %memframe_reg = call i8* @llvm.getmemframe(i32 %falloc_reg)
  %284 = bitcast %struct.node_t* %8 to i8*
  br label %for.body.i

for.body.i:                                       ; preds = %for.body.i, %rng_spawn.exit
  %indvars.iv.i = phi i32 [ %indvars.iv.next.i, %for.body.i ], [ 0, %rng_spawn.exit ]
  %arrayidx.i = getelementptr inbounds i8* %284, i32 %indvars.iv.i
  %285 = load i8* %arrayidx.i, align 1
  %arrayidx2.i = getelementptr inbounds i8* %memframe_reg, i32 %indvars.iv.i
  store i8 %285, i8* %arrayidx2.i, align 1
  %indvars.iv.next.i = add i32 %indvars.iv.i, 1
  %exitcond.i = icmp eq i32 %indvars.iv.next.i, 32
  br i1 %exitcond.i, label %redefine_memcpy.exit, label %for.body.i

redefine_memcpy.exit:                             ; preds = %for.body.i
  call void @llvm.writecm(i32 %falloc_reg, i32 0, i32 %falloc_reg1)
  call void @llvm.writecm(i32 %falloc_reg1, i32 20, i32 %diff)
  call void @llvm.writecm(i32 %falloc_reg1, i32 16, i32 %2)
  ret void
}

; Function Attrs: nounwind
declare void @llvm.writecm(i32, i32, i32) #1

; Function Attrs: nounwind readnone
declare i8* @llvm.getmemframe(i32) #3

; Function Attrs: nounwind
declare void @llvm.sync(i32, i32, i32) #1

; Function Attrs: nounwind
declare void @llvm.fdelete(i32) #1

; Function Attrs: nounwind
declare void @llvm.writecmp(i32, i32) #1

; Function Attrs: nounwind
declare i32 @llvm.falloc(i32) #1

; Function Attrs: nounwind
declare void @llvm.fbind(i32, i32) #1

attributes #0 = { alwaysinline nounwind ssp }
attributes #1 = { nounwind }
attributes #2 = { nounwind ssp }
attributes #3 = { nounwind readnone }

!hyperop.annotations = !{!0, !2, !3, !4, !5, !7, !9, !11, !12, !13, !14, !16, !17}

!0 = metadata !{metadata !"redefine_start111", metadata !"Id=4", metadata !"Affinity=0", metadata !"Static=yes", metadata !"Frame=0", metadata !"Entry=yes", metadata !"Exit=no", metadata !"Predicated=no", metadata !"Barrier=yes", metadata !1}
!1 = metadata !{}
!2 = metadata !{metadata !"uts_function2111", metadata !"Id=5", metadata !"Affinity=0", metadata !"Static=yes", metadata !"Frame=1", metadata !"Entry=no", metadata !"Exit=yes", metadata !"Predicated=no", metadata !"Barrier=no", metadata !1}
!3 = metadata !{metadata !"uts_function41111", metadata !"Id=7", metadata !"Affinity=0", metadata !"Static=yes", metadata !"Frame=2", metadata !"Entry=no", metadata !"Exit=no", metadata !"Predicated=yes", metadata !"Barrier=no", metadata !1}
!4 = metadata !{metadata !"uts_function51111", metadata !"Id=8", metadata !"Affinity=0", metadata !"Static=yes", metadata !"Frame=3", metadata !"Entry=no", metadata !"Exit=no", metadata !"Predicated=yes", metadata !"Barrier=no", metadata !1}
!5 = metadata !{metadata !"uts_function6111", metadata !"Id=9", metadata !"Affinity=0", metadata !"Static=yes", metadata !"Frame=4", metadata !"Entry=no", metadata !"Exit=no", metadata !"Predicated=no", metadata !"Barrier=yes", metadata !6}
!6 = metadata !{metadata !"1=32"}
!7 = metadata !{metadata !"uts_function_for.body71111", metadata !"Id=10", metadata !"Affinity=0", metadata !"Static=yes", metadata !"Frame=5", metadata !"Entry=no", metadata !"Exit=no", metadata !"Predicated=no", metadata !"Barrier=yes", metadata !8}
!8 = metadata !{metadata !"3=4"}
!9 = metadata !{metadata !"uts_function_for.body811111", metadata !"Id=11", metadata !"Affinity=0", metadata !"Static=no", metadata !"Frame=0", metadata !"Entry=no", metadata !"Exit=no", metadata !"Predicated=yes", metadata !"Barrier=no", metadata !10}
!10 = metadata !{metadata !"6=32", metadata !"7=32"}
!11 = metadata !{metadata !"uts_function9111", metadata !"Id=6", metadata !"Affinity=0", metadata !"Static=no", metadata !"Frame=0", metadata !"Entry=no", metadata !"Exit=no", metadata !"Predicated=no", metadata !"Barrier=no", metadata !1}
!12 = metadata !{metadata !"uts_function111111", metadata !"Id=12", metadata !"Affinity=0", metadata !"Static=no", metadata !"Frame=0", metadata !"Entry=no", metadata !"Exit=no", metadata !"Predicated=yes", metadata !"Barrier=no", metadata !1}
!13 = metadata !{metadata !"uts_function121111", metadata !"Id=13", metadata !"Affinity=0", metadata !"Static=no", metadata !"Frame=0", metadata !"Entry=no", metadata !"Exit=no", metadata !"Predicated=yes", metadata !"Barrier=no", metadata !1}
!14 = metadata !{metadata !"uts_function131111", metadata !"Id=14", metadata !"Affinity=0", metadata !"Static=no", metadata !"Frame=0", metadata !"Entry=no", metadata !"Exit=no", metadata !"Predicated=no", metadata !"Barrier=no", metadata !15}
!15 = metadata !{metadata !"2=32"}
!16 = metadata !{metadata !"uts_function_for.body141111", metadata !"Id=15", metadata !"Affinity=0", metadata !"Static=no", metadata !"Frame=0", metadata !"Entry=no", metadata !"Exit=no", metadata !"Predicated=no", metadata !"Barrier=yes", metadata !8}
!17 = metadata !{metadata !"uts_function_for.body1511111", metadata !"Id=16", metadata !"Affinity=0", metadata !"Static=no", metadata !"Frame=0", metadata !"Entry=no", metadata !"Exit=no", metadata !"Predicated=yes", metadata !"Barrier=no", metadata !10}
