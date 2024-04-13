  bool DrivesControlDependency(const NodeDef& node) const {
    for (const NodeDef* output : ctx().node_map->GetOutputs(node.name())) {
      for (int i = 0; i < output->input_size(); ++i) {
        const TensorId tensor = ParseTensorName(output->input(i));
        if (tensor.node() == node.name() && tensor.index() < 0) {
          return true;
        }
      }
    }
    return false;
  }