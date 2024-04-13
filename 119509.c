  Status HoistChainForSplit(const int prefix_length, const ChainLinkSet& tails,
                            std::set<string>* ctrl_inputs,
                            NodeDef* split_node) {
    // Create a new chain before the split node to process the input tensor.
    const string& split_name = split_node->name();
    auto root_scope_and_name = ParseNodeScopeAndName(split_name);

    // We use the first tail node in the set as a template to get the list of
    // ops to apply (starting from the end).
    NodeDef* cur_tail = tails.begin()->node;
    NodeDef* cur_copy = AddCopyNode(
        OptimizedNodeName(root_scope_and_name, cur_tail->name()), cur_tail);
    cur_copy->clear_input();

    // Update the split to take its input from the tail of the new chain.
    const int value_slot = split_node->op() == "SplitV" ? 0 : 1;
    const string orig_input = split_node->input(value_slot);
    split_node->set_input(value_slot, cur_copy->name());
    ctx().node_map->UpdateInput(split_node->name(), orig_input,
                                cur_copy->name());
    TF_RETURN_IF_ERROR(GetInputNode(cur_tail->input(0), &cur_tail));

    // Now walk backwards creating the rest of the chain.
    while (cur_tail != split_node) {
      NodeDef* new_copy = AddCopyNode(
          OptimizedNodeName(root_scope_and_name, cur_tail->name()), cur_tail);
      new_copy->clear_input();
      cur_copy->add_input(new_copy->name());
      ctx().node_map->AddOutput(new_copy->name(), cur_copy->name());
      cur_copy = new_copy;
      TF_RETURN_IF_ERROR(GetInputNode(cur_tail->input(0), &cur_tail));
    }
    // Connect the original input to the head of the new chain.
    cur_copy->add_input(orig_input);
    ctx().node_map->UpdateOutput(NodeName(orig_input), split_name,
                                 cur_copy->name());
    // Make sure all the control inputs are satisfied before running the first
    // node in the new chain.
    AddControlInputs(ctrl_inputs, cur_copy);

    // Connect all consumers of the tail nodes directly to the
    // output port of Split from which the chain started.
    for (const auto& link : tails) {
      UpdateConsumers(link.node,
                      link.port_origin == 0
                          ? split_name
                          : strings::StrCat(split_name, ":", link.port_origin));
    }
    return Status::OK();
  }