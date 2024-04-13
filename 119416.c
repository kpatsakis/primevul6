  bool HasAllInputsBroadcastableToShape(
      const NodeDef& node, const OpInfo::TensorProperties& properties) const {
    auto is_broadcastable = [this, &properties](const string& input) {
      const OpInfo::TensorProperties* input_props;
      Status has_input_properties = GetTensorProperties(input, &input_props);
      return has_input_properties.ok() &&
             ShapesBroadcastable(properties, *input_props);
    };
    return std::all_of(node.input().begin(), node.input().end(),
                       is_broadcastable);
  }