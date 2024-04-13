static Status ValidateSavedTensors(const GraphDef& graph_def) {
  for (const auto& node : graph_def.node()) {
    TF_RETURN_IF_ERROR(ValidateNode(node));
  }

  if (graph_def.has_library()) {
    const FunctionDefLibrary& library = graph_def.library();
    for (const auto& function : library.function()) {
      for (const auto& node : function.node_def()) {
        TF_RETURN_IF_ERROR(ValidateNode(node));
      }

      // Also check that there is no recursivity in the library
      // TODO(mihaimaruseac): Do more than self-recursivity
      TF_RETURN_IF_ERROR(ValidateFunctionNotRecursive(function));
    }
  }

  return Status::OK();
}