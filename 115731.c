TfLiteStatus ReportOpError(TfLiteContext* context, const TfLiteNode& node,
                           const TfLiteRegistration& registration,
                           int node_index, const char* message) {
  context->ReportError(
      context, "Node number %d (%s) %s.\n", node_index,
      registration.custom_name
          ? registration.custom_name
          : EnumNameBuiltinOperator(
                static_cast<BuiltinOperator>(registration.builtin_code)),
      message);
  return kTfLiteError;
}