  void ForwardControlDependencies(
      NodeDef* target_node, const std::vector<const NodeDef*>& src_nodes) {
    for (const auto& src : src_nodes) {
      for (int i = src->input_size() - 1; i >= 0; --i) {
        if (IsControlInput(src->input(i))) {
          *target_node->add_input() = src->input(i);
          ctx().node_map->AddOutput(NodeName(src->input(i)),
                                    target_node->name());
        } else {
          break;
        }
      }
    }
    DedupControlInputs(target_node);
  }