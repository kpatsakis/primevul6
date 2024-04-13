  Status TrySimplify(NodeDef* root, string* simplified_node_name) override {
    TF_RETURN_IF_ERROR(CheckAttrExists(*root, "T"));
    DataType dtype = root->attr().at("T").type();

    // Keep a trace of all supported input nodes that can be fused together.
    std::vector<string> op_nodes = {root->name()};
    std::vector<string> op_names = {root->op()};

    // Check if we should follow input(0) while building an op composition.
    const auto predicate_fn = [&](const NodeDef& input) {
      if (input.name() == root->name()) return true;

      bool follow_input_node =
          dtype == GetDataTypeFromAttr(input, "T") &&
          NumNonControlDataOutputs(input, *ctx().node_map) == 1 &&
          CanOptimize(input);

      if (follow_input_node) {
        op_nodes.push_back(input.name());
        op_names.push_back(input.op());
      }

      return follow_input_node;
    };

    NodeDef* last_op = GetTailOfChain(
        *root, *ctx().node_map, /*follow_control_input*/ false, predicate_fn);

    // We were not able to find a chain that can be replaced.
    if (op_names.size() == 1) return Status::OK();

    // Do not add fused nodes to any other chain.
    std::for_each(op_nodes.begin(), op_nodes.end(),
                  [this](const string& name) { AddToFusedNodes(name); });

    // Reverse the trace to get correct composition computation order.
    std::reverse(op_names.begin(), op_names.end());

    VLOG(2) << "Fuse unary ops: root=" << root->name() << " op_names=["
            << absl::StrJoin(op_names, ", ") << "]";

    NodeDef* composition_node = ctx().optimized_graph->add_node();
    composition_node->set_name(OptimizedNodeName(*root));
    composition_node->set_op("_UnaryOpsComposition");
    composition_node->add_input(last_op->input(0));
    composition_node->set_device(root->device());

    auto attr = composition_node->mutable_attr();
    SetAttrValue(dtype, &(*attr)["T"]);
    SetAttrValue(op_names, &(*attr)["op_names"]);

    ctx().node_map->AddNode(composition_node->name(), composition_node);
    ctx().node_map->AddOutput(NodeName(last_op->input(0)),
                              composition_node->name());

    *simplified_node_name = composition_node->name();

    return Status::OK();
  }