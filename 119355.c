  InputAndShape UpdateInputs(const string& input_0, const string& input_1,
                             NodeDef* node) {
    string old_input_0 = node->input(0);
    string old_input_1 = node->input(1);

    // Update inputs only if they changed
    if (old_input_0 != input_0 || old_input_1 != input_1) {
      node->set_input(0, input_0);
      node->set_input(1, input_1);
      // Invalidate node properties (shape)
      ctx().graph_properties->ClearOutputProperties(node->name());
      ctx().graph_properties->ClearInputProperties(node->name());
      // Update the node map
      ctx().node_map->RemoveOutput(NodeName(old_input_0), node->name());
      ctx().node_map->RemoveOutput(NodeName(old_input_1), node->name());
      ctx().node_map->AddOutput(NodeName(input_0), node->name());
      ctx().node_map->AddOutput(NodeName(input_1), node->name());
      // Add updated node to optimization queue
      AddToOptimizationQueue(node);
    }

    TensorShapeProto shape;  // shape is not important at this point
    return InputAndShape(node->name(), shape);
  }