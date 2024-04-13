  Status HoistUnaryOpChain(const int prefix_length, const ChainLinkSet& tails,
                           std::set<string>* ctrl_inputs, NodeDef* root_node) {
    VLOG(3) << "Hoist unary op chain:"
            << " root=" << root_node->DebugString()
            << " prefix_length=" << prefix_length << " ctrl_inputs=["
            << absl::StrJoin(*ctrl_inputs, ", ") << "]";

    if (tails.empty()) {
      return Status::OK();
    }
    AddToOptimizationQueue(root_node);
    optimized_nodes_.insert(root_node->name());
    if (node_is_concat_) {
      AddControlInputs(ctrl_inputs, root_node);
      return HoistChainForConcat(prefix_length, tails, root_node);
    } else {
      return HoistChainForSplit(prefix_length, tails, ctrl_inputs, root_node);
    }
  }