Status ArithmeticOptimizer::SimplifyArithmeticOps(bool can_use_shapes) {
  SetVector<NodeDef*> nodes_to_simplify;
  nodes_to_simplify.Reserve(optimized_graph_->node_size());
  for (int i = 0; i < optimized_graph_->node_size(); ++i) {
    nodes_to_simplify.PushBack(optimized_graph_->mutable_node(i));
  }

  const GraphOptimizerContext ctx(&nodes_to_preserve_, optimized_graph_,
                                  graph_properties_.get(), node_map_.get(),
                                  &feed_nodes_, opt_level_);
  const ArithmeticOptimizerContext ctx_ext(&nodes_to_simplify);

  // Stop pipeline after first stage returning non-empty simplified tensor
  // name.
  const auto stop = [](const string& result) { return !result.empty(); };
  GraphOptimizerStagePipeline<string> pipeline(stop);

  if (options_.combine_add_to_addn && can_use_shapes)
    pipeline.AddStage<AddOpsRewriteStage>(ctx, ctx_ext);
  if (options_.fold_conjugate_into_transpose)
    pipeline.AddStage<FoldConjugateIntoTranspose>(ctx, ctx_ext);
  if (options_.fold_multiply_into_conv)
    pipeline.AddStage<FoldMultiplyIntoConv>(ctx, ctx_ext);
  if (options_.fold_transpose_into_matmul)
    pipeline.AddStage<FoldTransposeIntoMatMul>(ctx, ctx_ext);
  if (options_.hoist_common_factor_out_of_aggregation && can_use_shapes)
    pipeline.AddStage<HoistCommonFactorOutOfAggregation>(ctx, ctx_ext);
  if (options_.minimize_broadcasts && can_use_shapes)
    pipeline.AddStage<MinimizeBroadcasts>(ctx, ctx_ext);
  if (options_.remove_identity_transpose && can_use_shapes)
    pipeline.AddStage<RemoveIdentityTranspose>(ctx, ctx_ext);
  if (options_.remove_involution)
    pipeline.AddStage<RemoveInvolution>(ctx, ctx_ext);
  if (options_.remove_redundant_bitcast)
    pipeline.AddStage<RemoveRedundantBitcastStage>(ctx, ctx_ext);
  if (options_.remove_redundant_cast)
    pipeline.AddStage<RemoveRedundantCastStage>(ctx, ctx_ext);
  if (options_.replace_pack_with_tile_reshape)
    pipeline.AddStage<ReplacePackWithTileReshape>(ctx, ctx_ext);
  if (options_.replace_mul_with_tile && can_use_shapes)
    pipeline.AddStage<ReplaceMulWithBroadcastByTile>(ctx, ctx_ext);
  if (options_.reduce_upsampling_dims)
    pipeline.AddStage<ReduceUpsamplingDims>(ctx, ctx_ext);
  if (options_.remove_redundant_reshape)
    pipeline.AddStage<RemoveRedundantReshapeOrBroadcastTo>(ctx, ctx_ext);
  if (options_.remove_negation)
    pipeline.AddStage<RemoveNegationStage>(ctx, ctx_ext);
  if (options_.replace_mul_with_square)
    pipeline.AddStage<ReplaceMulWithSquare>(ctx, ctx_ext);
  if (options_.remove_logical_not)
    pipeline.AddStage<RemoveLogicalNotStage>(ctx, ctx_ext);
  if (options_.reorder_cast_like_and_value_preserving)
    pipeline.AddStage<ReorderCastLikeAndValuePreserving>(ctx, ctx_ext);
  if (options_.reorder_reshape_around_unary)
    pipeline.AddStage<ReorderReshapeAroundUnary>(ctx, ctx_ext);
  if (options_.simplify_aggregation)
    pipeline.AddStage<SimplifyAggregation>(ctx, ctx_ext);
  if (options_.hoist_cwise_unary_chains)
    pipeline.AddStage<HoistCWiseUnaryChainsStage>(ctx, ctx_ext);
  if (options_.convert_sqrt_div_to_rsqrt_mul)
    pipeline.AddStage<SqrtDivToRsqrtMulStage>(ctx, ctx_ext);
  if (options_.remove_idempotent)
    pipeline.AddStage<RemoveIdempotentStage>(ctx, ctx_ext);
  if (options_.convert_pow) pipeline.AddStage<ConvertPowStage>(ctx, ctx_ext);
  if (options_.convert_log1p)
    pipeline.AddStage<ConvertLog1pStage>(ctx, ctx_ext);
  if (options_.convert_log_softmax)
    pipeline.AddStage<LogSoftmaxStage>(ctx, ctx_ext);
  if (options_.optimize_max_or_min_of_monotonic)
    pipeline.AddStage<OptimizeMaxOrMinOfMonotonicStage>(ctx, ctx_ext);
  if (options_.convert_expm1)
    pipeline.AddStage<ConvertExpm1Stage>(ctx, ctx_ext);
  if (options_.unary_ops_composition)
    pipeline.AddStage<UnaryOpsComposition>(ctx, ctx_ext);
  if (options_.remove_stack_slice_same_axis)
    pipeline.AddStage<RemoveStackSliceSameAxis>(ctx, ctx_ext);
  if (options_.simplify_embedding_lookup)
    pipeline.AddStage<SimplifyEmbeddingLookupStage>(ctx, ctx_ext);
  if (options_.remove_cast_into_segment_reduction)
    pipeline.AddStage<RemoveCastIntoSegmentReductionStage>(ctx, ctx_ext);
  if (options_.fuse_squared_diff)
    pipeline.AddStage<FuseSquaredDiffStage>(ctx, ctx_ext);

  VLOG(1) << "Run " << pipeline.NumStages() << " arithmetic optimizer stages: "
          << absl::StrJoin(pipeline.StageNames(), ", ");

  while (!nodes_to_simplify.Empty()) {
    GRAPPLER_RETURN_IF_DEADLINE_EXCEEDED();
    NodeDef* node = nodes_to_simplify.PopBack();

    string simplified_tensor = "";
    bool optimized = pipeline.PassThroughAllStages(node, &simplified_tensor);

    // If the node was not optimized by any of the stages, go to the next one.
    if (!optimized) continue;

    // re-wire consumers of an old node to the new one
    if (NodeName(simplified_tensor) != node->name()) {
      // Always consider simplified_tensor for further optimizations.
      NodeDef* simplified_node = node_map_->GetNode(simplified_tensor);
      if (simplified_node != nullptr) {
        nodes_to_simplify.PushBack(simplified_node);
      }
      // When `node` is simplified to another node rather than in-place, the
      // consumers of `node` are already redirected to `simplified_tensor`.
      // Re-push the consumers into `nodes_to_simplify` for further
      // optimizations.
      const std::vector<NodeDef*> consumers =
          node_map_->GetOutputsOrderedByNodeName(node->name());
      for (NodeDef* consumer : consumers) {
        // Update `consumer`'s use of `node` to `input`'s operand.
        for (int i = 0; i < consumer->input_size(); ++i) {
          int operand_pos;
          string operand_node_name =
              ParseNodeName(consumer->input(i), &operand_pos);
          if (operand_node_name == node->name()) {
            *consumer->mutable_input(i) =
                (operand_pos < 0
                     ? AsControlDependency(NodeName(simplified_tensor))
                     : simplified_tensor);
          }
        }
        node_map_->UpdateInput(consumer->name(), node->name(),
                               simplified_tensor);
        nodes_to_simplify.PushBack(consumer);
      }
    }
  }
  return Status::OK();
}