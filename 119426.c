  Status TrySimplify(NodeDef* node, string* simplified_node_name) override {
    TF_RETURN_IF_ERROR(EnsureNodeIsSupported(node));

    bool common_factor_is_denominator = false;
    std::set<string> common_factors;
    std::vector<string> ctrl_deps;
    TF_RETURN_IF_ERROR(GetCommonFactors(
        node, &common_factors, &common_factor_is_denominator, &ctrl_deps));

    if (common_factors.size() == 1) {
      const string& common_factor = *common_factors.begin();

      // Gather up the non-shared factors
      bool shapes_match = true;
      std::vector<string> unique_factors;
      TF_RETURN_IF_ERROR(GetUniqueFactors(node, common_factor,
                                          common_factor_is_denominator,
                                          &shapes_match, &unique_factors));

      if (shapes_match) {
        NodeDef* input_0;
        TF_RETURN_IF_ERROR(GetInputNode(node->input(0), &input_0));

        // Use a copy of the first node for the outer multiplication/division.
        NodeDef* new_outer_node = AddCopyNode(
            OuterNodeName(node, common_factor_is_denominator), input_0);
        // And a copy of aggregation node as one of the inner operands
        NodeDef* new_add_node = AddCopyNode(InnerAddNodeName(node), node);

        new_outer_node->set_device(node->device());
        if (common_factor_is_denominator) {
          new_outer_node->set_input(0, new_add_node->name());
          new_outer_node->set_input(1, common_factor);
        } else {
          new_outer_node->set_input(0, common_factor);
          new_outer_node->set_input(1, new_add_node->name());
        }

        ctx().node_map->AddOutput(common_factor, new_outer_node->name());
        ctx().node_map->AddOutput(new_add_node->name(), new_outer_node->name());

        // Hoist non-shared factors up into the new AddN node.
        for (int i = 0, end = unique_factors.size(); i < end; ++i) {
          const string& unique_factor_i = unique_factors[i];
          new_add_node->set_input(i, unique_factor_i);
          ctx().node_map->AddOutput(unique_factor_i, new_add_node->name());
        }

        // Add control deps on add node
        for (const string& ctrl_dep : ctrl_deps) {
          *new_add_node->add_input() = ctrl_dep;
          ctx().node_map->AddOutput(NodeName(ctrl_dep), new_add_node->name());
        }

        // optimize new inner aggregation node
        AddToOptimizationQueue(new_add_node);
        // do not optimize the same node twice
        rewritten_nodes_.insert(node->name());
        *simplified_node_name = new_outer_node->name();
      }
    }
    return Status::OK();
  }