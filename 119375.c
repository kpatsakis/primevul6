  string RewriteOptimizedNodesGroup(const OptimizedNodesGroup& group) override {
    VLOG(2) << "Collapse Add/AddN: root=" << group.root_node->name()
            << " op=" << group.root_node->op()
            << " num_optimized_nodes=" << group.optimized_nodes.size()
            << " num_inputs=" << group.inputs.size();

    // Do not optimize any of the nodes that are part of this group.
    MarkAllMembersWithTag(group, kAddOpsRewriteTag);

    // All new nodes will be placed under the scope of a root node.
    auto root_scope_and_name = ParseNodeScopeAndName(group.root_node->name());

    // Find what shapes are present in the inputs of absorbed nodes.
    std::unordered_map<string, std::vector<InputAndShape>> shape_sig_to_inputs;
    for (const auto& input : group.inputs) {
      shape_sig_to_inputs[ShapeSignature(input.shape)].push_back(input);
    }

    using SigKV = decltype(shape_sig_to_inputs)::value_type;
    VLOG(3) << "Add/AddN group has " << shape_sig_to_inputs.size()
            << " unique shapes: "
            << absl::StrJoin(shape_sig_to_inputs, ", ",
                             [](string* out, SigKV p) {
                               strings::StrAppend(out, p.first);
                             });

    // Collect all the shapes from representative elements.
    std::vector<TensorShapeProto> shapes;
    shapes.reserve(shape_sig_to_inputs.size());
    for (const auto& el : shape_sig_to_inputs)
      shapes.push_back(el.second[0].shape);

    // If all inputs have the same shape, rewrite whole group with a single AddN
    if (shapes.size() == 1) {
      string node_name = UniqueOptimizedNodeName(root_scope_and_name);
      AddInputsOfSymbolicallyEqualShape(*group.root_node, node_name,
                                        group.inputs);
      return node_name;
    }

    // For inputs of different shapes:
    // 1. Rewrite inputs of the same shape using AddN (leaf nodes)
    // 2. Build a tree of Add nodes, minimizing cost of broadcast
    std::sort(shapes.begin(), shapes.end(),
              [](const TensorShapeProto& left, const TensorShapeProto& right) {
                return CompareSymbolicallyShapedTensorSizes(left, right);
              });

    // optimized name for leaf AddN nodes
    auto leaf_node_name = [&root_scope_and_name, this](int i) {
      return UniqueOptimizedNodeName(root_scope_and_name,
                                     strings::StrCat("Leaf_", i));
    };
    // optimized name for internal nodes of a tree built up from AddN leaves
    auto internal_node_name = [&root_scope_and_name, this](int i) {
      return UniqueOptimizedNodeName(root_scope_and_name,
                                     strings::StrCat("Internal_", i));
    };

    // Add/AddN nodes that must be added to the tree
    std::deque<InputAndShape> add_ops;

    // Prepare leaf AddN nodes for inputs of equal shape
    for (int i = 0, end = shapes.size(); i < end; ++i) {
      const auto node_name = leaf_node_name(i);
      const auto& inputs = shape_sig_to_inputs[ShapeSignature(shapes[i])];
      add_ops.push_back(AddInputsOfSymbolicallyEqualShape(*group.root_node,
                                                          node_name, inputs));
    }

    // Build up a tree of Add ops
    int internal_nodes = 0;
    do {
      const InputAndShape lhs = add_ops.front();
      add_ops.pop_front();
      const InputAndShape rhs = add_ops.front();
      add_ops.pop_front();
      string name = add_ops.empty()
                        ? UniqueOptimizedNodeName(root_scope_and_name)
                        : internal_node_name(internal_nodes++);
      InputAndShape add = AddAggregatedInputs(*group.root_node, name, lhs, rhs);
      add_ops.push_front(add);
    } while (add_ops.size() > 1);

    InputAndShape optimized_root_node = add_ops.front();
    return optimized_root_node.input;
  }