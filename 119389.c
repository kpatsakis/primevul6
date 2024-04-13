  void UpdateConsumers(NodeDef* node, const string& new_input) {
    const string& node_name = node->name();
    const auto consumers = ctx().node_map->GetOutputs(node_name);
    for (NodeDef* consumer : consumers) {
      for (int i = 0; i < consumer->input_size(); ++i) {
        if (consumer->input(i) == node_name &&
            consumer->name() != NodeName(new_input)) {
          consumer->set_input(i, new_input);
          ctx().node_map->UpdateInput(consumer->name(), node_name, new_input);
        }
      }
      AddToOptimizationQueue(consumer);
    }
  }