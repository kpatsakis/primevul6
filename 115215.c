  OpContext(TfLiteContext* context, TfLiteNode* node) {
    params = reinterpret_cast<TfLiteSplitVParams*>(node->builtin_data);
    input = GetInput(context, node, 0);
    size_splits = GetInput(context, node, 1);
    axis = GetInput(context, node, 2);
  }