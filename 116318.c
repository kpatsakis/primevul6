  SpaceToBatchNDContext(TfLiteContext* context, TfLiteNode* node) {
    input = GetInput(context, node, 0);
    block_shape = GetInput(context, node, 1);
    paddings = GetInput(context, node, 2);
    output = GetOutput(context, node, 0);
  }