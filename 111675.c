TfLiteStatus Prepare(TfLiteContext* context, TfLiteNode* node) {
  TF_LITE_ENSURE_EQ(context, NumInputs(node), 5);
  TF_LITE_ENSURE_EQ(context, NumOutputs(node), 1);

  const TfLiteTensor* ids;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 0, &ids));
  TF_LITE_ENSURE_EQ(context, NumDimensions(ids), 1);
  TF_LITE_ENSURE_EQ(context, ids->type, kTfLiteInt32);

  const TfLiteTensor* indices;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 1, &indices));
  TF_LITE_ENSURE_EQ(context, NumDimensions(indices), 2);
  TF_LITE_ENSURE_EQ(context, indices->type, kTfLiteInt32);

  const TfLiteTensor* shape;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 2, &shape));
  TF_LITE_ENSURE_EQ(context, NumDimensions(shape), 1);
  TF_LITE_ENSURE_EQ(context, shape->type, kTfLiteInt32);

  const TfLiteTensor* weights;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 3, &weights));
  TF_LITE_ENSURE_EQ(context, NumDimensions(weights), 1);
  TF_LITE_ENSURE_EQ(context, weights->type, kTfLiteFloat32);

  TF_LITE_ENSURE_EQ(context, SizeOfDimension(indices, 0),
                    SizeOfDimension(ids, 0));
  TF_LITE_ENSURE_EQ(context, SizeOfDimension(indices, 0),
                    SizeOfDimension(weights, 0));

  const TfLiteTensor* value;
  TF_LITE_ENSURE_OK(context, GetInputSafe(context, node, 4, &value));
  TF_LITE_ENSURE(context, NumDimensions(value) >= 2);

  // Mark the output as a dynamic tensor.
  TfLiteTensor* output;
  TF_LITE_ENSURE_OK(context, GetOutputSafe(context, node, 0, &output));
  TF_LITE_ENSURE_TYPES_EQ(context, output->type, kTfLiteFloat32);
  output->allocation_type = kTfLiteDynamic;

  return kTfLiteOk;
}