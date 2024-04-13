TfLiteStatus Subgraph::OpPrepare(const TfLiteRegistration& op_reg,
                                 TfLiteNode* node) {
  if (op_reg.prepare == nullptr) {
    // Check if it's an unresolved custom op.
    if (IsUnresolvedCustomOp(op_reg)) {
      if (IsFlexOp(op_reg.custom_name)) {
        ReportError(
            "Regular TensorFlow ops are not supported by this interpreter. "
            "Make sure you apply/link the Flex delegate before inference.");
      } else {
        ReportError("Encountered unresolved custom op: %s.",
                    op_reg.custom_name ? op_reg.custom_name : "UnknownOp");
      }
      return kTfLiteError;
    }
    // Resolved ops can have a null Prepare function.
    return kTfLiteOk;
  }
  return op_reg.prepare(&context_, node);
}