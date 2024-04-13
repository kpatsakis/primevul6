Status ArithmeticOptimizer::Optimize(Cluster* /*cluster*/,
                                     const GrapplerItem& item,
                                     GraphDef* optimized_graph) {
  // Set up helper data structures.
  nodes_to_preserve_ = item.NodesToPreserve();
  fetch_nodes_known_ = !item.fetch.empty();
  GrapplerItem optimized_item(item);
  optimized_graph_ = &optimized_item.graph;

  node_map_.reset(new NodeMap(optimized_graph_));
  for (const auto& feed : item.feed) {
    feed_nodes_.insert(NodeName(feed.first));
  }

  // // Disable restricted graph rewrites.
  options_.unary_ops_composition &=
      item.optimization_options().allow_non_differentiable_rewrites;

  // Perform topological sort on the graph in order to help DedupComputations
  // and AddOpsRewrite to optimize larger subgraphs starting from the roots
  // with more inputs.
  TF_RETURN_IF_ERROR(TopologicalSort(optimized_graph_));
  GRAPPLER_RETURN_IF_DEADLINE_EXCEEDED();

  graph_properties_.reset(new GraphProperties(optimized_item));
  const bool assume_valid_feeds = opt_level_ == RewriterConfig::AGGRESSIVE;
  const Status status =
      graph_properties_->InferStatically(assume_valid_feeds,
                                         /*aggressive_shape_inference=*/false,
                                         /*include_tensor_values=*/false);
  const bool can_use_shapes = status.ok();
  if (!can_use_shapes) {
    VLOG(1) << "Shape inference failed." << status.error_message();
  }

  // Perform the optimizations.
  TF_RETURN_IF_ERROR(SimplifyArithmeticOps(can_use_shapes));

  optimized_graph->Swap(optimized_graph_);
  return Status::OK();
}