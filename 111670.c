static Status ValidateFunctionNotRecursive(const FunctionDef& function) {
  const auto& function_name = function.signature().name();
  for (const auto& node : function.node_def()) {
    if (node.op() == function_name) {
      return errors::FailedPrecondition(
          "Function ", function_name,
          " is self recursive and TensorFlow does not support this scenario.");
    }
  }

  return Status::OK();
}