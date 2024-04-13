  string RewriteOptimizedNodesGroup(const OptimizedNodesGroup& group) override {
    VLOG(2) << "Minimize broadcast: root=" << group.root_node->name()
            << " op=" << group.root_node->op()
            << " num_optimized_nodes=" << group.optimized_nodes.size();

    // Do not optimize any of the nodes that are part of this group.
    MarkAllMembersWithTag(group, kMinimizeBroadcastsTag);

    if (CountUniqueShapes(group.inputs) <= 1) {
      VLOG(3) << "Skip min-bcast group with single unique shape";
      // nothing to optimize when all shapes are the same
      return group.root_node->name();
    }

    auto num_nodes = /*root*/ 1 + group.optimized_nodes.size();
    auto num_inputs = group.inputs.size();
    CHECK_EQ(num_nodes, num_inputs - 1)
        << "Can't build a tree with " << num_inputs << " inputs, using "
        << num_nodes << "binary op nodes.";

    std::deque<InputAndShape> add_ops(group.inputs.begin(), group.inputs.end());
    std::deque<NodeDef*> optimized_nodes(group.optimized_nodes.begin(),
                                         group.optimized_nodes.end());

    // sort inputs by it's shape from smallest to largest
    std::stable_sort(add_ops.begin(), add_ops.end(),
                     [](const InputAndShape& lhs, const InputAndShape& rhs) {
                       return CompareSymbolicallyShapedTensorSizes(lhs.shape,
                                                                   rhs.shape);
                     });

    // If there is an odd number of inputs, last one is the largest, and we want
    // to attach it to the root node, to build a well balanced tree.
    std::deque<InputAndShape> add_ops_leftover;
    if (add_ops.size() % 2 != 0) {
      add_ops_leftover.push_back(add_ops.back());
      add_ops.pop_back();
    }

    // At this point it's guaranteed that add_ops have even number of inputs.
    do {
      const InputAndShape lhs = add_ops.front();
      add_ops.pop_front();
      const InputAndShape rhs = add_ops.front();
      add_ops.pop_front();

      NodeDef* node;
      if (!optimized_nodes.empty()) {
        // re-purpose optimized nodes to build a new tree
        node = optimized_nodes.back();
        optimized_nodes.pop_back();
      } else {
        // or use root node if none optimized nodes left
        node = group.root_node;
      }
      InputAndShape updated_node = UpdateInputs(lhs.input, rhs.input, node);

      // Pushing updated node to the back of a deque will create a wide and
      // short tree, pushing to the front will create a tall tree. We prefer to
      // get a wide tree, it minimizes the potential number of temporary tensors
      // required to keep in memory, though sometimes we can go up to prevent
      // propagating a broadcast from leaves to the root. Example:
      //
      // inputs: [s, s, s, M] (s - scalar, M - matrix)
      // @* - op with broadcast
      //
      //  (only push_back)           @*     (push_front first op)
      //                            /  \
      //       @*                  @    M
      //     /   \                / \
      //    @     @*      ->     @   s
      //   / \   / \            / \
      //  s   s s   M          s   s
      if (add_ops.size() >= 2 &&
          CompareSymbolicallyShapedTensorSizes(add_ops.at(0).shape,
                                               add_ops.at(1).shape)) {
        add_ops.push_front(updated_node);
      } else {
        add_ops.push_back(updated_node);
      }
    } while (add_ops.size() > 1);
    CHECK_EQ(1, add_ops.size());

    // attach the largest tensor to the root op
    if (!add_ops_leftover.empty()) {
      const InputAndShape lhs = add_ops.front();
      add_ops.pop_front();
      const InputAndShape rhs = add_ops_leftover.front();
      InputAndShape updated_node =
          UpdateInputs(lhs.input, rhs.input, group.root_node);
      add_ops.push_back(updated_node);
    }

    return add_ops.front().input;
  }